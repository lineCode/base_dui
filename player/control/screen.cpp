#include "stdafx.h"
#include "screen.h"
#include "../shared/closure.h"

//run in ui theard 
void Screen::BitbltCallback(HBITMAP hbmp, int width, int height)
{
	if (m_bStoped)		//关闭程序时 有可能这个任务还在ui线程最后，Screen先析构了，引起崩溃
	{
		printf("Screen::BitbltCallback m_bStoped = true\n");
		return;
	}
		
	if (m_pManager)
	{
		HDC hdc = m_pManager->GetPaintDC();
		m_diCurrFrame.rcBmpPart = { 0, 0, width, height };
		if (m_diCurrFrame.pImageInfo)
		{
			assert(m_diCurrFrame.pImageInfo->hBitmap);
			::DeleteObject(m_diCurrFrame.pImageInfo->hBitmap);
			delete m_diCurrFrame.pImageInfo;
			m_diCurrFrame.pImageInfo = nullptr;
		}

		ImageInfo* pImageInfo = new ImageInfo;
		memset(pImageInfo, 0, sizeof(ImageInfo));
		
		pImageInfo->hBitmap = hbmp;
		pImageInfo->nX = width;
		pImageInfo->nY = height;
		m_diCurrFrame.pImageInfo = pImageInfo;
		if (!m_diCurrFrame.bLoaded)
			m_diCurrFrame.bLoaded = true;
		Render::DrawImage(hdc, m_pManager, m_rcItem, m_rcPaint, m_diCurrFrame);
	}
}

//run in video play theard 
void Screen::StartPlayCallback(std::string file)
{
	assert(m_play_status == PS_STOP);
	m_bStoped = false;
	AVCodec *codec = NULL;
	AVCodecContext *ctx = NULL;
	AVCodecParameters *origin_par = NULL;
	AVFrame *fr = NULL;
	uint8_t *byte_buffer = NULL;
	AVPacket pkt = {};
	AVFormatContext *fmt_ctx = NULL;
	int number_of_written_bytes = -1;
	int video_stream = -1;
	int got_frame = 0;
	int byte_buffer_size;
	int i = 0;
	int result = -1;
	int end_of_stream = 0;

	AVPixelFormat src_pixfmt = AV_PIX_FMT_YUV420P;
	AVPixelFormat dst_pixfmt = AV_PIX_FMT_BGRA;			//AV_PIX_FMT_RGBA AV_PIX_FMT_RGB24
	struct SwsContext *img_convert_ctx = NULL;
	uint8_t *temp_buffer = NULL;


	result = avformat_open_input(&fmt_ctx, file.c_str(), NULL, NULL);
	if (result < 0) {
		av_log(NULL, AV_LOG_ERROR, "Can't open file\n");
		assert(0);
		goto err;
	}

	result = avformat_find_stream_info(fmt_ctx, NULL);
	if (result < 0) {
		av_log(NULL, AV_LOG_ERROR, "Can't get stream info\n");
		assert(0);
		goto err;
	}

	video_stream = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (video_stream < 0) {
		av_log(NULL, AV_LOG_ERROR, "Can't find video stream in input file\n");
		assert(0);
		goto err;
	}

	origin_par = fmt_ctx->streams[video_stream]->codecpar;

	codec = avcodec_find_decoder(origin_par->codec_id);
	if (!codec) {
		av_log(NULL, AV_LOG_ERROR, "Can't find decoder\n");
		assert(0);
		goto err;
	}

	ctx = avcodec_alloc_context3(codec);
	if (!ctx) {
		av_log(NULL, AV_LOG_ERROR, "Can't allocate decoder context\n");
		assert(0);
		goto err;
	}

	result = avcodec_parameters_to_context(ctx, origin_par);
	if (result) {
		av_log(NULL, AV_LOG_ERROR, "Can't copy decoder context\n");
		assert(0);
		goto err;
	}

	result = avcodec_open2(ctx, codec, NULL);
	if (result < 0) {
		av_log(ctx, AV_LOG_ERROR, "Can't open decoder\n");
		assert(0);
		goto err;
	}

	fr = av_frame_alloc();
	if (!fr) {
		av_log(NULL, AV_LOG_ERROR, "Can't allocate frame\n");
		assert(0);
		goto err;
	}

	byte_buffer_size = av_image_get_buffer_size(ctx->pix_fmt, ctx->width, ctx->height, 16);
	byte_buffer = (uint8_t*)av_malloc(byte_buffer_size);
	if (!byte_buffer) {
		av_log(NULL, AV_LOG_ERROR, "Can't allocate buffer\n");
		assert(0);
		goto err;
	}

	printf("#tb %d: %d/%d\n", video_stream, fmt_ctx->streams[video_stream]->time_base.num, fmt_ctx->streams[video_stream]->time_base.den);
	av_init_packet(&pkt);
#if 1
	
	int src_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(src_pixfmt));
	int dst_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(dst_pixfmt));
	int rescale_method = SWS_BICUBIC;
	int src_w = ctx->width, src_h = ctx->height;
	int dst_w = src_w, dst_h = src_h;
	//int dst_w = m_rcItem.right - m_rcItem.left, dst_h = m_rcItem.bottom - m_rcItem.top;
	uint8_t *src_data[4] = {};
	int src_linesize[4] = {};
	uint8_t *dst_data[4] = {};
	int dst_linesize[4] = {};

	temp_buffer = (uint8_t *)malloc(src_w * src_h * src_bpp / 8);
	result = av_image_alloc(src_data, src_linesize, src_w, src_h, src_pixfmt, 1);
	if (result< 0) {
		printf("Could not allocate source image\n");
		assert(0);
		goto err;
	}
	
	result = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pixfmt, 1);
	if (result< 0) {
		printf("Could not allocate destination image\n");
		assert(0);
		goto err;
	}

	img_convert_ctx = sws_alloc_context();
	if (!img_convert_ctx) {
		printf("Could not sws_alloc_context\n");
		assert(0);
		goto err;
	}
	//Show AVOption
	av_opt_show2(img_convert_ctx, stdout, AV_OPT_FLAG_VIDEO_PARAM, 0);
	//Set Value
	av_opt_set_int(img_convert_ctx, "sws_flags", SWS_BICUBIC | SWS_PRINT_INFO, 0);
	av_opt_set_int(img_convert_ctx, "srcw", src_w, 0);
	av_opt_set_int(img_convert_ctx, "srch", src_h, 0);
	av_opt_set_int(img_convert_ctx, "src_format", src_pixfmt, 0);
	//'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255)
	av_opt_set_int(img_convert_ctx, "src_range", 1, 0);
	av_opt_set_int(img_convert_ctx, "dstw", dst_w, 0);
	av_opt_set_int(img_convert_ctx, "dsth", dst_h, 0);
	av_opt_set_int(img_convert_ctx, "dst_format", dst_pixfmt, 0);
	av_opt_set_int(img_convert_ctx, "dst_range", 1, 0);
	result = sws_init_context(img_convert_ctx, NULL, NULL);
	if (result< 0) {
		printf("Could not sws_init_context\n");
		assert(0);
		goto err;
	}
#endif

	m_play_status = PS_PLAY;
	m_curr_file = file;
	do {
		while (m_play_status == PS_PAUSE)
			Sleep(100);
		if (!end_of_stream)
			if (av_read_frame(fmt_ctx, &pkt) < 0)
				end_of_stream = 1;
		if (end_of_stream) {
			pkt.data = NULL;
			pkt.size = 0;
		}
		if (pkt.stream_index == video_stream || end_of_stream) {
			got_frame = 0;
			if (pkt.pts == AV_NOPTS_VALUE)
				pkt.pts = pkt.dts = i;
#if 1		//ffmpeg不建议用这个函数了
			result = avcodec_decode_video2(ctx, fr, &got_frame, &pkt);
#else
			result = avcodec_send_packet(ctx, &pkt);
			avcodec_receive_frame(ctx, fr);
#endif
			if (result < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error decoding frame\n");
				//assert(0);
				goto err;
			}
			if (got_frame) {
				number_of_written_bytes = av_image_copy_to_buffer(byte_buffer, byte_buffer_size,
					(const uint8_t* const *)fr->data, (const int*)fr->linesize,
					ctx->pix_fmt, ctx->width, ctx->height, 1);
				if (number_of_written_bytes < 0) {
					av_log(NULL, AV_LOG_ERROR, "Can't copy image to buffer\n");
					//assert(0);
					goto err;
				}
				if ((i%100) == 0)
					printf("%d, %10"PRId64", %10"PRId64", %8"PRId64", %8d, 0x%08lx\n", video_stream, fr->pts, fr->pkt_dts, fr->pkt_duration,
						number_of_written_bytes, av_adler32_update(0, (const uint8_t*)byte_buffer, number_of_written_bytes));

				result = sws_scale(img_convert_ctx, (const uint8_t* const *)fr->data, (const int*)fr->linesize, 0, 768, dst_data, dst_linesize);
				if (result < 0) {
					av_log(NULL, AV_LOG_ERROR, "Error scale frame\n");
					//assert(0);
					goto err;
				}

				HBITMAP hbmp = ::CreateBitmap(dst_w, dst_h, 1, 32, dst_data[0]);
				if (!hbmp)
				{
					printf("CreateBitmap faile\n ");
					//assert(0);
					goto err;
				}
				else
					shared::Post2UI(std::bind(&Screen::BitbltCallback, this, hbmp, dst_w, dst_h));
			}
			av_packet_unref(&pkt);
			av_init_packet(&pkt);
		}
		i++;
		Sleep(20);
	} while ((m_play_status == PS_PLAY || m_play_status == PS_PAUSE) && (!end_of_stream || got_frame));

err:
	sws_freeContext(img_convert_ctx);
	free(temp_buffer);

	av_packet_unref(&pkt);
	av_frame_free(&fr);
	avcodec_close(ctx);
	avformat_close_input(&fmt_ctx);
	avcodec_free_context(&ctx);
	av_freep(&byte_buffer);

	if (m_play_status == PS_PLAY)
		m_play_status == PS_STOP;
	m_bStoped = true;
	return;
}