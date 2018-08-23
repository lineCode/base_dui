// ffmpeg_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>

bool BaseSaveBitmap(HBITMAP hbitmap, const std::wstring save_path)
{
	HDC     hDC;

	int     iBits;		//当前分辨率下每象素所占字节数         

	WORD     wBitCount;	//位图中每象素所占字节数         
	//定义调色板大小，     位图中像素字节大小     ，位图文件大小     ，     写入文件字节数             
	DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//位图属性结构             
	BITMAP     Bitmap;
	//位图文件头结构         
	BITMAPFILEHEADER     bmfHdr;
	//位图信息头结构             
	BITMAPINFOHEADER     bi;
	//指向位图信息头结构                 
	LPBITMAPINFOHEADER     lpbi;
	//定义文件，分配内存句柄，调色板句柄             
	HANDLE     fh, hDib, hPal, hOldPal = NULL;

	//计算位图文件每个像素所占字节数             
	hDC = ::CreateDC(L"DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL)     *     GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else  if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	GetObject(hbitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//为位图内容分配内存             
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//     处理调色板                 
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}

	//     获取该调色板下新的像素值             
	GetDIBits(hDC, hbitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	//恢复调色板                 
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//创建位图文件                 
	fh = CreateFile(save_path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
	{
		GlobalUnlock(hDib);
		GlobalFree(hDib);
		return false;
	}


	//     设置位图文件头             
	bmfHdr.bfType = 0x4D42;     //     "BM"             
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;

	::WriteFile(fh, (LPVOID)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);	//     写入位图文件头     
	::WriteFile(fh, (LPVOID)lpbi, dwDIBSize, &dwWritten, NULL);			//     写入位图文件其余内容      

	//清除                 
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return true;
}

static int video_decode_example(const char *input_filename)
{
	AVCodec *codec = NULL;
	AVCodecContext *ctx = NULL;
	AVCodecParameters *origin_par = NULL;
	AVFrame *fr = NULL;
	uint8_t *byte_buffer = NULL;
	AVPacket pkt;
	AVFormatContext *fmt_ctx = NULL;
	int number_of_written_bytes;
	int video_stream;
	int got_frame = 0;
	int byte_buffer_size;
	int i = 0;
	int result;
	int end_of_stream = 0;
#if 1
	AVPixelFormat src_pixfmt = AV_PIX_FMT_YUV420P;
	AVPixelFormat dst_pixfmt = AV_PIX_FMT_RGBA;	//AV_PIX_FMT_RGBA AV_PIX_FMT_RGB24
	int src_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(src_pixfmt));
	int dst_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(dst_pixfmt));
	int rescale_method = SWS_BICUBIC;
	struct SwsContext *img_convert_ctx;
	uint8_t *temp_buffer = (uint8_t *)malloc(1366*768*src_bpp / 8);

	uint8_t *src_data[4];
	int src_linesize[4];
	uint8_t *dst_data[4];
	int dst_linesize[4];
	result = av_image_alloc(src_data, src_linesize, 1366, 768, src_pixfmt, 1);
	if (result< 0) {
		printf("Could not allocate source image\n");
		return -1;
	}
	int dst_w = 1366 / 2, dst_h = 768 / 2;
	result = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pixfmt, 1);
	if (result< 0) {
		printf("Could not allocate destination image\n");
		return -1;
	}

	img_convert_ctx = sws_alloc_context();
	//Show AVOption
	av_opt_show2(img_convert_ctx, stdout, AV_OPT_FLAG_VIDEO_PARAM, 0);
	//Set Value
	av_opt_set_int(img_convert_ctx, "sws_flags", SWS_BICUBIC | SWS_PRINT_INFO, 0);
	av_opt_set_int(img_convert_ctx, "srcw", 1366, 0);
	av_opt_set_int(img_convert_ctx, "srch", 768, 0);
	av_opt_set_int(img_convert_ctx, "src_format", src_pixfmt, 0);
	//'0' for MPEG (Y:0-235);'1' for JPEG (Y:0-255)
	av_opt_set_int(img_convert_ctx, "src_range", 1, 0);
	av_opt_set_int(img_convert_ctx, "dstw", dst_w, 0);
	av_opt_set_int(img_convert_ctx, "dsth", dst_h, 0);
	av_opt_set_int(img_convert_ctx, "dst_format", dst_pixfmt, 0);
	av_opt_set_int(img_convert_ctx, "dst_range", 1, 0);
	sws_init_context(img_convert_ctx, NULL, NULL);

#endif
	result = avformat_open_input(&fmt_ctx, input_filename, NULL, NULL);
	if (result < 0) {
		av_log(NULL, AV_LOG_ERROR, "Can't open file\n");
		return result;
	}

	result = avformat_find_stream_info(fmt_ctx, NULL);
	if (result < 0) {
		av_log(NULL, AV_LOG_ERROR, "Can't get stream info\n");
		return result;
	}

	video_stream = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (video_stream < 0) {
		av_log(NULL, AV_LOG_ERROR, "Can't find video stream in input file\n");
		return -1;
	}

	origin_par = fmt_ctx->streams[video_stream]->codecpar;

	codec = avcodec_find_decoder(origin_par->codec_id);
	if (!codec) {
		av_log(NULL, AV_LOG_ERROR, "Can't find decoder\n");
		return -1;
	}

	ctx = avcodec_alloc_context3(codec);
	if (!ctx) {
		av_log(NULL, AV_LOG_ERROR, "Can't allocate decoder context\n");
		return AVERROR(ENOMEM);
	}

	result = avcodec_parameters_to_context(ctx, origin_par);
	if (result) {
		av_log(NULL, AV_LOG_ERROR, "Can't copy decoder context\n");
		return result;
	}

	result = avcodec_open2(ctx, codec, NULL);
	if (result < 0) {
		av_log(ctx, AV_LOG_ERROR, "Can't open decoder\n");
		return result;
	}

	fr = av_frame_alloc();
	if (!fr) {
		av_log(NULL, AV_LOG_ERROR, "Can't allocate frame\n");
		return AVERROR(ENOMEM);
	}

	byte_buffer_size = av_image_get_buffer_size(ctx->pix_fmt, ctx->width, ctx->height, 16);
	byte_buffer = (uint8_t*)av_malloc(byte_buffer_size);
	if (!byte_buffer) {
		av_log(NULL, AV_LOG_ERROR, "Can't allocate buffer\n");
		return AVERROR(ENOMEM);
	}

	printf("#tb %d: %d/%d\n", video_stream, fmt_ctx->streams[video_stream]->time_base.num, fmt_ctx->streams[video_stream]->time_base.den);
	i = 0;
	av_init_packet(&pkt);
	do {
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
#if 1
			result = avcodec_decode_video2(ctx, fr, &got_frame, &pkt);
#else
			result = avcodec_send_packet(ctx, &pkt);
			avcodec_receive_frame(ctx, fr);
#endif
			if (result < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error decoding frame\n");
				return result;
			}
			if (got_frame) {
				number_of_written_bytes = av_image_copy_to_buffer(byte_buffer, byte_buffer_size,
					(const uint8_t* const *)fr->data, (const int*)fr->linesize,
					ctx->pix_fmt, ctx->width, ctx->height, 1);
				if (number_of_written_bytes < 0) {
					av_log(NULL, AV_LOG_ERROR, "Can't copy image to buffer\n");
					return number_of_written_bytes;
				}
				printf("%d, %10"PRId64", %10"PRId64", %8"PRId64", %8d, 0x%08lx\n", video_stream,
					fr->pts, fr->pkt_dts, fr->pkt_duration,
					number_of_written_bytes, av_adler32_update(0, (const uint8_t*)byte_buffer, number_of_written_bytes));

				result = sws_scale(img_convert_ctx, (const uint8_t* const *)fr->data, (const int*)fr->linesize, 0, 768, dst_data, dst_linesize);
				if (result < 0) {
					av_log(NULL, AV_LOG_ERROR, "Error scale frame\n");
					return result;
				}

				HBITMAP hbmp = ::CreateBitmap(dst_w, dst_h, 1, 32, dst_data[0]); 
				if (!hbmp)
				{
					printf("CreateBitmapIndirect faile\n ");
				}
				else
					BaseSaveBitmap(hbmp, L"c:\\1.bmp");
			}
			av_packet_unref(&pkt);
			av_init_packet(&pkt);
		}
		i++;
	} while (!end_of_stream || got_frame);

	sws_freeContext(img_convert_ctx);
	free(temp_buffer);

	av_packet_unref(&pkt);
	av_frame_free(&fr);
	avcodec_close(ctx);
	avformat_close_input(&fmt_ctx);
	avcodec_free_context(&ctx);
	av_freep(&byte_buffer);
	return 0;
}

int main(int argc, char **argv)
{
	/*if (argc < 2)
	{
		av_log(NULL, AV_LOG_ERROR, "Incorrect input\n");
		return 1;
	}*/

	if (video_decode_example("../bin/other/video.mp4") != 0)
		return 1;

	return 0;
}

