#include "stdafx.h"

///////////////////////////////////////////////////////////////////////////////////////
DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{ 
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
typedef struct
{ 
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);
///////////////////////////////////////////////////////////////////////////////////////

namespace dui
{

	GifAnim::GifAnim(void)
	{
		m_pGifImage			=	NULL;
		m_pPropertyItem		=	NULL;
		m_nFrameCount		=	0;	
		m_nFramePosition	=	0;	
		m_bIsAutoPlay		=	true;
		m_bIsAutoSize		=	false;
		m_bIsPlaying		=	false;

	}


	GifAnim::~GifAnim(void)
	{
		DeleteGif();
		m_pManager->KillTimer( this, EVENT_TIEM_ID );

	}

	LPCTSTR GifAnim::GetClass() const
	{
		return DUI_CTR_GIFANIM;
	}

	LPVOID GifAnim::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_GIFANIM) == 0 ) return static_cast<GifAnim*>(this);
		return Control::GetInterface(pstrName);
	}

	void GifAnim::Init()
	{
		InitGifImage();
	}

	bool GifAnim::DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl)
	{
		if ( NULL == m_pGifImage )
		{		
			InitGifImage();
		}
		DrawFrame( hDC );
        return true;
	}

	void GifAnim::DoEvent( Event& event )
	{
		if( event.Type == UIEVENT_TIMER )
			OnTimer( (UINT_PTR)event.wParam );
	}

	void GifAnim::SetVisible(bool bVisible /* = true */)
	{
		Control::SetVisible(bVisible);
		if (bVisible)
			PlayGif();
		else
			StopGif();
	}

	void GifAnim::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
		else if( _tcscmp(pstrName, _T("autoplay")) == 0 ) {
			SetAutoPlay(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcscmp(pstrName, _T("autosize")) == 0 ) {
			SetAutoSize(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else
			Control::SetAttribute(pstrName, pstrValue);
	}

	void GifAnim::SetBkImage(LPCTSTR pStrImage)
	{
		if( m_sBkImage == pStrImage || NULL == pStrImage) return;

		m_sBkImage = pStrImage;

		StopGif();
		DeleteGif();

		Invalidate();

	}

	LPCTSTR GifAnim::GetBkImage()
	{
		return m_sBkImage.c_str();
	}

	void GifAnim::SetAutoPlay(bool bIsAuto)
	{
		m_bIsAutoPlay = bIsAuto;
	}

	bool GifAnim::IsAutoPlay() const
	{
		return m_bIsAutoPlay;
	}

	void GifAnim::SetAutoSize(bool bIsAuto)
	{
		m_bIsAutoSize = bIsAuto;
	}

	bool GifAnim::IsAutoSize() const
	{
		return m_bIsAutoSize;
	}

	void GifAnim::PlayGif()
	{
		if (m_bIsPlaying || m_pGifImage == NULL)
		{
			return;
		}

		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		if ( lPause == 0 ) lPause = 100;
		m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );

		m_bIsPlaying = true;
	}

	void GifAnim::PauseGif()
	{
		if (!m_bIsPlaying || m_pGifImage == NULL)
		{
			return;
		}

		m_pManager->KillTimer(this, EVENT_TIEM_ID);
		this->Invalidate();
		m_bIsPlaying = false;
	}

	void GifAnim::StopGif()
	{
		if (!m_bIsPlaying)
		{
			return;
		}

		m_pManager->KillTimer(this, EVENT_TIEM_ID);
		m_nFramePosition = 0;
		this->Invalidate();
		m_bIsPlaying = false;
	}

	void GifAnim::InitGifImage()
	{
		m_pGifImage = LoadGifFromFile(GetBkImage());
		if ( NULL == m_pGifImage ) return;
		UINT nCount	= 0;
		nCount	=	m_pGifImage->GetFrameDimensionsCount();
		GUID* pDimensionIDs	=	new GUID[ nCount ];
		m_pGifImage->GetFrameDimensionsList( pDimensionIDs, nCount );
		m_nFrameCount	=	m_pGifImage->GetFrameCount( &pDimensionIDs[0] );
		int nSize		=	m_pGifImage->GetPropertyItemSize( PropertyTagFrameDelay );
		m_pPropertyItem	=	(Gdiplus::PropertyItem*) malloc( nSize );
		m_pGifImage->GetPropertyItem( PropertyTagFrameDelay, nSize, m_pPropertyItem );
		delete  pDimensionIDs;
		pDimensionIDs = NULL;

		if (m_bIsAutoSize)
		{
			SetFixedWidth(m_pGifImage->GetWidth());
			SetFixedHeight(m_pGifImage->GetHeight());
		}
		if (m_bIsAutoPlay && nSize > 0)
		{
			PlayGif();
		}
	}

	void GifAnim::DeleteGif()
	{
		if ( m_pGifImage != NULL )
		{
			delete m_pGifImage;
			m_pGifImage = NULL;
		}

		if ( m_pPropertyItem != NULL )
		{
			free( m_pPropertyItem );
			m_pPropertyItem = NULL;
		}
		m_nFrameCount		=	0;	
		m_nFramePosition	=	0;	
	}

	void GifAnim::OnTimer( UINT_PTR idEvent )
	{
		if ( idEvent != EVENT_TIEM_ID )
			return;
		m_pManager->KillTimer( this, EVENT_TIEM_ID );
		this->Invalidate();

		m_nFramePosition = (++m_nFramePosition) % m_nFrameCount;

		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		if ( lPause == 0 ) lPause = 100;
		m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );
	}

	void GifAnim::DrawFrame( HDC hDC )
	{
		if ( NULL == hDC || NULL == m_pGifImage ) return;
		GUID pageGuid = Gdiplus::FrameDimensionTime;
		Gdiplus::Graphics graphics( hDC );
		graphics.DrawImage( m_pGifImage, m_rcItem.left, m_rcItem.top, m_rcItem.right-m_rcItem.left, m_rcItem.bottom-m_rcItem.top );
		m_pGifImage->SelectActiveFrame( &pageGuid, m_nFramePosition );
	}

	Gdiplus::Image* GifAnim::LoadGifFromFile(LPCTSTR pstrGifPath)
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;

		do 
		{
			String sFile;
			if (m_pManager){
				sFile = m_pManager->GetThisResPath();
			}
			else{
				sFile = UIManager::GetGlobalResDir();
			}
			/*String sFile = UIManager::GetGlobalResDir();*/
			if (UIManager::GetResourceZip().empty()) {
				sFile += pstrGifPath;
				HANDLE hFile = ::CreateFile(sFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
					FILE_ATTRIBUTE_NORMAL, NULL);
				if( hFile == INVALID_HANDLE_VALUE ) break;
				dwSize = ::GetFileSize(hFile, NULL);
				if( dwSize == 0 ) break;

				DWORD dwRead = 0;
				pData = new BYTE[ dwSize ];
				::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
				::CloseHandle( hFile );

				if( dwRead != dwSize ) {
					delete[] pData;
					pData = NULL;
					break;
				}
			}
			else {
				sFile += UIManager::GetResourceZip();
				HZIP hz = NULL;
				if( UIManager::IsCachedResourceZip() ) hz = (HZIP)UIManager::GetResourceZipHandle();
				else hz = OpenZip((void*)sFile.c_str(), 0, 2);
				if( hz == NULL ) break;
				ZIPENTRY ze; 
				int i; 
				if( FindZipItem(hz, pstrGifPath, true, &i, &ze) != 0 ) break;
				dwSize = ze.unc_size;
				if( dwSize == 0 ) break;
				pData = new BYTE[ dwSize ];
				int res = UnzipItem(hz, i, pData, dwSize, 3);
				if( res != 0x00000000 && res != 0x00000600) {
					delete[] pData;
					pData = NULL;
					if( !UIManager::IsCachedResourceZip() ) CloseZip(hz);
					break;
				}
				if( !UIManager::IsCachedResourceZip() ) CloseZip(hz);
			}

		} while (0);

		while (!pData)
		{
			//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
			HANDLE hFile = ::CreateFile(pstrGifPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) break;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) break;

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				pData = NULL;
			}
			break;
		}
		if (!pData)
		{
			return NULL;
		}

		Gdiplus::Image* pImage = LoadGifFromMemory(pData, dwSize);
		delete pData;
		return pImage;
	}

	Gdiplus::Image* GifAnim::LoadGifFromMemory( LPVOID pBuf,size_t dwSize )
	{
		HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
		BYTE* pMem = (BYTE*)::GlobalLock(hMem);

		memcpy(pMem, pBuf, dwSize);

		IStream* pStm = NULL;
		::CreateStreamOnHGlobal(hMem, TRUE, &pStm);
		Gdiplus::Image *pImg = Gdiplus::Image::FromStream(pStm);
		if(!pImg || pImg->GetLastStatus() != Gdiplus::Ok)
		{
			pStm->Release();
			::GlobalUnlock(hMem);
			return 0;
		}
		pStm->Release();	//djj[20170428] : add
		
		return pImg;
	}


}
