#include "stdafx.h"


namespace dui
{
    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    STRINGorID::STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
    {
    }

    STRINGorID::STRINGorID(unsigned int nID) : m_lpstr(MAKEINTRESOURCE(nID))
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

	DuiPoint::DuiPoint()
	{
		x = y = 0;
	}

	DuiPoint::DuiPoint(const POINT& src)
	{
		x = src.x;
		y = src.y;
	}

	DuiPoint::DuiPoint(long _x, long _y)
	{
		x = _x;
		y = _y;
	}

	DuiPoint::DuiPoint(LPARAM lParam)
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
	}

    DuiPoint::DuiPoint(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) x = y = 0;
        LPTSTR pstr = NULL;
        x = y = _tcstol(pstrValue, &pstr, 10); ASSERT(pstr);
        y = _tcstol(pstr + 1, &pstr, 10);      ASSERT(pstr);
    }

    String DuiPoint::ToString()
    {
		TCHAR cPoint[64] = {};
		::wsprintf(cPoint, _T("%ld,%ld"), x, y);
		return String(cPoint);
    }

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	DuiSize::DuiSize()
	{
		cx = cy = 0;
	}

	DuiSize::DuiSize(const SIZE& src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	DuiSize::DuiSize(const RECT rc)
	{
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
	}

	DuiSize::DuiSize(long _cx, long _cy)
	{
		cx = _cx;
		cy = _cy;
	}

    DuiSize::DuiSize(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) cx = cy = 0;
        LPTSTR pstr = NULL;
        cx = cy = _tcstol(pstrValue, &pstr, 10); ASSERT(pstr);
        cy = _tcstol(pstr + 1, &pstr, 10);       ASSERT(pstr);
    }

    String DuiSize::ToString()
    {
		TCHAR cSize[64] = {};
		::wsprintf(cSize, _T("%ld,%ld"), cx, cy);
		return String(cSize);
    }

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	DuiRect::DuiRect()
	{
		left = top = right = bottom = 0;
	}

	DuiRect::DuiRect(const RECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
	}

	DuiRect::DuiRect(long iLeft, long iTop, long iRight, long iBottom)
	{
		left = iLeft;
		top = iTop;
		right = iRight;
		bottom = iBottom;
	}

    DuiRect::DuiRect(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) left = top = right = bottom = 0;
        LPTSTR pstr = NULL;
        left = top = right = bottom = _tcstol(pstrValue, &pstr, 10); ASSERT(pstr);
        top = bottom = _tcstol(pstr + 1, &pstr, 10);                 ASSERT(pstr);
        right = _tcstol(pstr + 1, &pstr, 10);                        ASSERT(pstr);
        bottom = _tcstol(pstr + 1, &pstr, 10);                       ASSERT(pstr);
    }

    String DuiRect::ToString()
    {
		TCHAR cRect[64] = {};
		::wsprintf(cRect, _T("%ld,%ld,%ld,%ld"), left, top, right, bottom);
		return String(cRect);
    }

	int DuiRect::GetWidth() const
	{
		return right - left;
	}

	int DuiRect::GetHeight() const
	{
		return bottom - top;
	}

	void DuiRect::Empty()
	{
		left = top = right = bottom = 0;
	}

	bool DuiRect::IsNull() const
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0); 
	}

	void DuiRect::Join(const RECT& rc)
	{
		if( rc.left < left ) left = rc.left;
		if( rc.top < top ) top = rc.top;
		if( rc.right > right ) right = rc.right;
		if( rc.bottom > bottom ) bottom = rc.bottom;
	}

	void DuiRect::ResetOffset()
	{
		::OffsetRect(this, -left, -top);
	}

	void DuiRect::Normalize()
	{
		if( left > right ) { int iTemp = left; left = right; right = iTemp; }
		if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
	}

	void DuiRect::Offset(int cx, int cy)
	{
		::OffsetRect(this, cx, cy);
	}

	void DuiRect::Inflate(int cx, int cy)
	{
		::InflateRect(this, cx, cy);
	}

	void DuiRect::Deflate(int cx, int cy)
	{
		::InflateRect(this, -cx, -cy);
	}

	void DuiRect::Union(DuiRect& rc)
	{
		::UnionRect(this, this, &rc);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	PtrArray::PtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
	{
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
	}

	PtrArray::PtrArray(const PtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
	{
		for(int i=0; i<src.GetSize(); i++)
			Add(src.GetAt(i));
	}

	PtrArray::~PtrArray()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
	}

	void PtrArray::Empty()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
		m_ppVoid = NULL;
		m_nCount = m_nAllocated = 0;
	}

	void PtrArray::Resize(int iSize)
	{
		Empty();
		m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
		::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
		m_nAllocated = iSize;
		m_nCount = iSize;
	}

	bool PtrArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool PtrArray::Add(LPVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		m_ppVoid[m_nCount - 1] = pData;
		return true;
	}

	bool PtrArray::InsertAt(int iIndex, LPVOID pData)
	{
		if( iIndex == m_nCount ) return Add(pData);
		if( iIndex < 0 || iIndex > m_nCount ) return false;
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
		m_ppVoid[iIndex] = pData;
		return true;
	}

	bool PtrArray::SetAt(int iIndex, LPVOID pData)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		m_ppVoid[iIndex] = pData;
		return true;
	}

    bool PtrArray::Remove(int iIndex, int iCount)
    {
        if( iIndex < 0 || iCount <= 0 || iIndex + iCount > m_nCount ) return false;
        if (iIndex + iCount < m_nCount) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + iCount, (m_nCount - iIndex - iCount) * sizeof(LPVOID));
        m_nCount -= iCount;
        return true;
    }

	int PtrArray::Find(LPVOID pData) const
	{
		for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
		return -1;
	}

	int PtrArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID* PtrArray::GetData()
	{
		return m_ppVoid;
	}

	LPVOID PtrArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_ppVoid[iIndex];
	}

	LPVOID PtrArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_ppVoid[iIndex];
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	ValArray::ValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
	m_pVoid(NULL), 
		m_nCount(0), 
		m_iElementSize(iElementSize), 
		m_nAllocated(iPreallocSize)
	{
		ASSERT(iElementSize>0);
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
	}

	ValArray::~ValArray()
	{
		if( m_pVoid != NULL ) free(m_pVoid);
	}

	void ValArray::Empty()
	{   
		m_nCount = 0;  // NOTE: We keep the memory in place
	}

	bool ValArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool ValArray::Add(LPCVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
			if( pVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_pVoid = pVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
		return true;
	}

	bool ValArray::Remove(int iIndex, int iCount)
	{
        if( iIndex < 0 || iCount <= 0 || iIndex + iCount > m_nCount ) return false;
        if (iIndex + iCount < m_nCount) ::CopyMemory(m_pVoid + (iIndex * m_iElementSize), m_pVoid + (iIndex + iCount) * m_iElementSize, (m_nCount - iIndex - iCount) * m_iElementSize);
        m_nCount -= iCount;
		return true;
	}

	int ValArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID ValArray::GetData()
	{
		return static_cast<LPVOID>(m_pVoid);
	}

	LPVOID ValArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_pVoid + (iIndex * m_iElementSize);
	}

	LPVOID ValArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_pVoid + (iIndex * m_iElementSize);
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//
    struct TITEM
    {
        String Key;
        LPVOID Data;
        struct TITEM* pPrev;
        struct TITEM* pNext;
    };

	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		SIZE_T len = _tcslen(Key);
		while( len-- > 0 ) i = (i << 5) + i + Key[len];
		return i;
	}

	static UINT HashKey(const String& Key)
	{
		return HashKey((LPCTSTR)Key.c_str());
	};

	StringPtrMap::StringPtrMap(int nSize) : m_nCount(0)
	{
		if( nSize < 16 ) nSize = 16;
		m_nBuckets = nSize;
		m_aT = new TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(TITEM*));
	}

	StringPtrMap::~StringPtrMap()
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}
	}

	void StringPtrMap::RemoveAll()
	{
		this->Resize(m_nBuckets);
	}

	void StringPtrMap::Resize(int nSize)
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}

		if( nSize < 0 ) nSize = 0;
		if( nSize > 0 ) {
			m_aT = new TITEM*[nSize];
			memset(m_aT, 0, nSize * sizeof(TITEM*));
		} 
		m_nBuckets = nSize;
		m_nCount = 0;
	}

	LPVOID StringPtrMap::Find(LPCTSTR key, bool optimize) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		UINT slot = HashKey(key) % m_nBuckets;
		for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
			if( pItem->Key == key ) {
				if (optimize && pItem != m_aT[slot]) {
					if (pItem->pNext) {
						pItem->pNext->pPrev = pItem->pPrev;
					}
					pItem->pPrev->pNext = pItem->pNext;
					pItem->pPrev = NULL;
					pItem->pNext = m_aT[slot];
					pItem->pNext->pPrev = pItem;
					//将item移动至链条头部
					m_aT[slot] = pItem;
				}
				return pItem->Data;
			}        
		}

		return NULL;
	}

	bool StringPtrMap::Insert(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return false;
		if( Find(key) ) return false;

		// Add first in bucket
		UINT slot = HashKey(key) % m_nBuckets;
		TITEM* pItem = new TITEM;
		pItem->Key = key;
		pItem->Data = pData;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
			pItem->pNext->pPrev = pItem;
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	}

	LPVOID StringPtrMap::Set(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return pData;

		if (GetSize()>0) {
			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
				if( pItem->Key == key ) {
					LPVOID pOldData = pItem->Data;
					pItem->Data = pData;
					return pOldData;
				}
			}
		}

		Insert(key, pData);
		return NULL;
	}

	bool StringPtrMap::Remove(LPCTSTR key)
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		UINT slot = HashKey(key) % m_nBuckets;
		TITEM** ppItem = &m_aT[slot];
		while( *ppItem ) {
			if( (*ppItem)->Key == key ) {
				TITEM* pKill = *ppItem;
				*ppItem = (*ppItem)->pNext;
				if (*ppItem)
					(*ppItem)->pPrev = pKill->pPrev;
				delete pKill;
				m_nCount--;
				return true;
			}
			ppItem = &((*ppItem)->pNext);
		}

		return false;
	}

	int StringPtrMap::GetSize() const
	{
#if 0//def _DEBUG
		int nCount = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) nCount++;
		}
		ASSERT(m_nCount==nCount);
#endif
		return m_nCount;
	}

	LPCTSTR StringPtrMap::GetAt(int iIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == iIndex ) {
					return pItem->Key.c_str();
				}
			}
		}

		return NULL;
	}

	LPCTSTR StringPtrMap::operator[] (int nIndex) const
	{
		return GetAt(nIndex);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CWaitCursor::CWaitCursor()
	{
		m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	}

	CWaitCursor::~CWaitCursor()
	{
		::SetCursor(m_hOrigCursor);
	}

	

} // namespace dui