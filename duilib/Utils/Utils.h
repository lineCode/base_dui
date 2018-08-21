#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once

namespace dui
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString);
		STRINGorID(unsigned int nID);

		LPCTSTR m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class DUILIB_API DuiPoint : public tagPOINT
	{
	public:
		DuiPoint();
		DuiPoint(const POINT& src);
		DuiPoint(long x, long y);
		DuiPoint(LPARAM lParam);
        DuiPoint(LPCTSTR pstrValue);
        String ToString();
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API DuiSize : public tagSIZE
	{
	public:
		DuiSize();
		DuiSize(const SIZE& src);
		DuiSize(const RECT rc);
		DuiSize(long cx, long cy);
        DuiSize(LPCTSTR pstrValue);
        String ToString();
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API DuiRect : public tagRECT
	{
	public:
		DuiRect();
		DuiRect(const RECT& src);
		DuiRect(long iLeft, long iTop, long iRight, long iBottom);
        DuiRect(LPCTSTR pstrValue);
        String ToString();

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(DuiRect& rc);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API PtrArray
	{
	public:
		PtrArray(int iPreallocSize = 0);
		PtrArray(const PtrArray& src);
		~PtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex, int iCount = 1);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API ValArray
	{
	public:
		ValArray(int iElementSize, int iPreallocSize = 0);
		~ValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex,  int iCount = 1);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

    struct TITEM;
	class DUILIB_API StringPtrMap
	{
	public:
		StringPtrMap(int nSize = 83);
		~StringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
#if 0
	class Variant : public VARIANT
	{
	public:
		Variant() 
		{ 
			VariantInit(this); 
		}
		Variant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		Variant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		Variant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		Variant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~Variant() 
		{ 
			VariantClear(this); 
		}
	};
#endif


	bool CreateImageObject(void **ppv);
	String GetIconByFile(String file);
	bool FilePathExtension(const String &filepath_in, String &extension_out);
	
}// namespace dui

#endif // __UTILS_H__