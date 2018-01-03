#ifndef __UIRICHEDIT_H__
#define __UIRICHEDIT_H__

#pragma once

#include <richole.h>
#include <textserv.h>

namespace dui {
	class RichEdit;
	class CTxtWinHost : public ITextHost
	{
	public:
		CTxtWinHost();
		BOOL Init(RichEdit *re, const CREATESTRUCT *pcs);
		virtual ~CTxtWinHost();

		ITextServices* GetTextServices(void) { return pserv; }
		void SetClientRect(RECT *prc);
		RECT* GetClientRect() { return &rcClient; }
		BOOL IsWordWrap(void) { return fWordWrap; }
		void SetWordWrap(BOOL fWordWrap);
		BOOL IsReadOnly();
		void SetReadOnly(BOOL fReadOnly);

		void SetFont(HFONT hFont);
		void SetColor(DWORD dwColor);
		SIZEL* GetExtent();
		void SetExtent(SIZEL *psizelExtent);
		void LimitText(LONG nChars);
		BOOL IsCaptured();
		BOOL IsShowCaret();
		void NeedFreshCaret();
		INT GetCaretWidth();
		INT GetCaretHeight();

		BOOL GetAllowBeep();
		void SetAllowBeep(BOOL fAllowBeep);
		WORD GetDefaultAlign();
		void SetDefaultAlign(WORD wNewAlign);
		BOOL GetRichTextFlag();
		void SetRichTextFlag(BOOL fNew);
		LONG GetDefaultLeftIndent();
		void SetDefaultLeftIndent(LONG lNewIndent);
		BOOL SetSaveSelection(BOOL fSaveSelection);
		HRESULT OnTxInPlaceDeactivate();
		HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
		BOOL GetActiveState(void) { return fInplaceActive; }
		BOOL DoSetCursor(RECT *prc, POINT *pt);
		void SetTransparent(BOOL fTransparent);
		void GetControlRect(LPRECT prc);
		LONG SetAccelPos(LONG laccelpos);
		WCHAR SetPasswordChar(WCHAR chPasswordChar);
		void SetDisabled(BOOL fOn);
		LONG SetSelBarWidth(LONG lSelBarWidth);
		BOOL GetTimerState();

		void SetCharFormat(CHARFORMAT2W &c);
		void SetParaFormat(PARAFORMAT2 &p);

		// -----------------------------
		//	IUnknown interface
		// -----------------------------
		virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
		virtual ULONG _stdcall AddRef(void);
		virtual ULONG _stdcall Release(void);

		// -----------------------------
		//	ITextHost interface
		// -----------------------------
		virtual HDC TxGetDC();
		virtual INT TxReleaseDC(HDC hdc);
		virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
		virtual BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags);
		virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
		virtual BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw);
		virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
		virtual void TxViewChange(BOOL fUpdate);
		virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
		virtual BOOL TxShowCaret(BOOL fShow);
		virtual BOOL TxSetCaretPos(INT x, INT y);
		virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
		virtual void TxKillTimer(UINT idTimer);
		virtual void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
		virtual void TxSetCapture(BOOL fCapture);
		virtual void TxSetFocus();
		virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
		virtual BOOL TxScreenToClient(LPPOINT lppt);
		virtual BOOL TxClientToScreen(LPPOINT lppt);
		virtual HRESULT TxActivate(LONG * plOldState);
		virtual HRESULT TxDeactivate(LONG lNewState);
		virtual HRESULT TxGetClientRect(LPRECT prc);
		virtual HRESULT TxGetViewInset(LPRECT prc);
		virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF);
		virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
		virtual COLORREF TxGetSysColor(int nIndex);
		virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
		virtual HRESULT TxGetMaxLength(DWORD *plength);
		virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
		virtual HRESULT TxGetPasswordChar(TCHAR *pch);
		virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
		virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
		virtual HRESULT OnTxCharFormatChange(const CHARFORMATW * pcf);
		virtual HRESULT OnTxParaFormatChange(const PARAFORMAT * ppf);
		virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
		virtual HRESULT TxNotify(DWORD iNotify, void *pv);
		virtual HIMC TxImmGetContext(void);
		virtual void TxImmReleaseContext(HIMC himc);
		virtual HRESULT TxGetSelectionBarWidth(LONG *lSelBarWidth);

	private:
		RichEdit *m_re;
		ULONG	cRefs;					// Reference Count
		ITextServices	*pserv;		    // pointer to Text Services object
		// Properties

		DWORD		dwStyle;				// style bits

		unsigned	fEnableAutoWordSel : 1;	// enable Word style auto word selection?
		unsigned	fWordWrap : 1;	// Whether control should word wrap
		unsigned	fAllowBeep : 1;	// Whether beep is allowed
		unsigned	fRich : 1;	// Whether control is rich text
		unsigned	fSaveSelection : 1;	// Whether to save the selection when inactive
		unsigned	fInplaceActive : 1; // Whether control is inplace active
		unsigned	fTransparent : 1; // Whether control is transparent
		unsigned	fTimer : 1;	// A timer is set
		unsigned    fCaptured : 1;
		unsigned    fShowCaret : 1;
		unsigned    fNeedFreshCaret : 1; // 修正改变大小后点击其他位置原来光标不能消除的问题

		INT         iCaretWidth;
		INT         iCaretHeight;
		INT         iCaretLastWidth;
		INT         iCaretLastHeight;
		LONG		lSelBarWidth;			// Width of the selection bar
		LONG  		cchTextMost;			// maximum text size
		DWORD		dwEventMask;			// DoEvent mask to pass on to parent window
		LONG		icf;
		LONG		ipf;
		RECT		rcClient;				// Client Rect for this control
		SIZEL		sizelExtent;			// Extent array
		CHARFORMAT2W cf;					// Default character format
		PARAFORMAT2	pf;					    // Default paragraph format
		LONG		laccelpos;				// Accelerator position
		WCHAR		chPasswordChar;		    // Password character
	};

	//-----------------------------------------------------------------------------
	//----------------------------------IRichEditOleCallbackEx-----------------------------------
	//-----------------------------------------------------------------------------
	interface IRichEditOleCallbackEx : public IRichEditOleCallback
	{
	public:
		IRichEditOleCallbackEx(RichEdit *re, std::function<void()> callback);
		virtual ~IRichEditOleCallbackEx();
		void SetCustomMode(bool custom = false);

		virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE* lplpstg);
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
		virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
			LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
		virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
		virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
		virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
		virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
			DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
		virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj);
		virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
		virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
			HMENU FAR *lphmenu);

	private:
		HRESULT SaveStr2DataObject(LPDATAOBJECT pDataSource, std::wstring str, CLIPFORMAT cfFormat);

	private:
		int m_iNumStorages;
		IStorage* pStorage;
		DWORD m_dwRef;
		UINT cf_html_format_;
		UINT cf_nim_format_;

		std::function<void()> callback_;
		bool custom_mode_;

		RichEdit *richedit_ = nullptr;
	};
	//-----------------------------------------------------------------------------
	//----------------------------------RichEdit-----------------------------------
	//-----------------------------------------------------------------------------
#define MAX_CUSTOM_ITEM_NUM	9

#define MAX_CUSTOM_ITEM_W	150
#define MAX_CUSTOM_ITEM_H	150

#define MAX_CUSTOM_FILE_W	200

typedef std::function<void()> InsertCustomItemErrorCallback;

class DUILIB_API RichEdit : public ScrollContainer, public IMessageFilter
{
public:
    RichEdit();
    ~RichEdit();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    bool IsWantTab();
    void SetWantTab(bool bWantTab = true);
    bool IsWantReturn();
    void SetWantReturn(bool bWantReturn = true);
    bool IsWantCtrlReturn();
    void SetWantCtrlReturn(bool bWantCtrlReturn = true);
    bool IsTransparent();
    void SetTransparent(bool bTransparent = true);
    bool IsRich();
    void SetRich(bool bRich = true);
    bool IsReadOnly();
    void SetReadOnly(bool bReadOnly = true);
    bool IsWordWrap();
    void SetWordWrap(bool bWordWrap = true);
    int GetFont();
    void SetFont(int index);
    void SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    LONG GetWinStyle();
    void SetWinStyle(LONG lStyle);
    DWORD GetTextColor();
    void SetTextColor(DWORD dwTextColor);
    int GetLimitText();
    void SetLimitText(int iChars);
    long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
    String GetText() const;
    void SetText(LPCTSTR pstrText);
    bool IsModify() const;
    void SetModify(bool bModified = true) const;
    void GetSel(CHARRANGE &cr) const;
    void GetSel(long& nStartChar, long& nEndChar) const;
    int SetSel(CHARRANGE &cr);
    int SetSel(long nStartChar, long nEndChar);
    void ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo);
    void ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
    String GetSelText() const;
    int SetSelAll();
    int SetSelNone();
    WORD GetSelectionType() const;
    bool GetZoom(int& nNum, int& nDen) const;
    bool SetZoom(int nNum, int nDen);
    bool SetZoomOff();
    bool GetAutoURLDetect() const;
    bool SetAutoURLDetect(bool bAutoDetect = true);
    DWORD GetEventMask() const;
    DWORD SetEventMask(DWORD dwEventMask);
    String GetTextRange(long nStartChar, long nEndChar) const;
    void HideSelection(bool bHide = true, bool bChangeStyle = false);
    void ScrollCaret();
    int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo = false);
    int AppendText(LPCTSTR lpstrText, bool bCanUndo = false);
    DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
    bool SetDefaultCharFormat(CHARFORMAT2 &cf);
    DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
    bool SetSelectionCharFormat(CHARFORMAT2 &cf);
    bool SetWordCharFormat(CHARFORMAT2 &cf);
    DWORD GetParaFormat(PARAFORMAT2 &pf) const;
    bool SetParaFormat(PARAFORMAT2 &pf);
    bool Redo();
    bool Undo();
    void Clear();
    void Copy();
    void Cut();
    void Paste();
    int GetLineCount() const;
    String GetLine(int nIndex, int nMaxLength) const;
    int LineIndex(int nLine = -1) const;
    int LineLength(int nLine = -1) const;
    bool LineScroll(int nLines, int nChars = 0);
	CDuiPoint GetCharPos(long lChar) const;
    long LineFromChar(long nIndex) const;
    CDuiPoint PosFromChar(UINT nChar) const;
    int CharFromPos(CDuiPoint pt) const;
    void EmptyUndoBuffer();
    UINT SetUndoLimit(UINT nLimit);
    long StreamIn(int nFormat, EDITSTREAM &es);
    long StreamOut(int nFormat, EDITSTREAM &es);

	RECT GetTextPadding() const;
	void SetTextPadding(RECT rc);

    void DoInit();
	bool SetDropAcceptFile(bool bAccept);
	// 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
	// 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
    virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
    IDropTarget* GetTxDropTarget();
    virtual bool OnTxViewChanged();
    virtual void OnTxNotify(DWORD iNotify, void *pv);

    void SetScrollPos(SIZE szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();

    SIZE EstimateSize(SIZE szAvailable);
	void SetPos(RECT rc, bool bNeedInvalidate = true);
	void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoEvent(TEvent& event);
    bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	//----------------------------add begin----------------------------
public:
	enum RE_OLE_TYPE
	{
		RE_OLE_TYPE_TEXT = 0,
		RE_OLE_TYPE_FACE,
		RE_OLE_TYPE_CUSTOM,
		RE_OLE_TYPE_IMAGE = RE_OLE_TYPE_CUSTOM,
		RE_OLE_TYPE_IMAGELOADING,//正在加载的图片
		RE_OLE_TYPE_FILE,
		RE_OLE_TYPE_ERROR,
		RE_OLE_TYPE_DESCRIPTION,
	};

public:
	bool InsertEmoji(const std::wstring &file_name, const std::wstring &tag, int size);
	bool InsertImage(InsertCustomItemErrorCallback callback, bool total_count_limit, const std::wstring& file, const std::wstring& file_tag = L"", bool loading = false, LONG cp = REO_CP_SELECTION);
	bool InsertFile(InsertCustomItemErrorCallback callback, const std::wstring& file);
	void InsertHBitmap(InsertCustomItemErrorCallback callback, HBITMAP hbitmap, bool custom);
protected:
	IRichEditOle* GetOleInterface();
	int GetCustomImageOleCount();
	/**
	* 查找RichEditt中是否已经插入了某个文件ole对象
	* @param[in] text_service RichEdit控件内部ITextServices*指针
	* @param[in] file 文件名
	* @param[in] check_sel 是否在选中的范围内查找
	* @return bool true 存在，false 不存在
	*/
	bool Re_FindFile(const std::wstring& file, bool check_sel = false);
	/**
	* 向RichEdit插入一个ImageOle对象
	* @param[in] text_service RichEdit控件内部ITextServices*指针
	* @param[in] callback 插入错误的回调通知函数
	* @param[in] file 图片文件路径
	* @param[in] face_tag 图片对应的标签
	* @param[in] ole_type 插入ole对象的类型
	* @param[in] face_id 图片id
	* @param[in] scale 是否保持缩放后的比例
	* @param[in] scale_width 缩放后的宽度
	* @param[in] scale_height 缩放后的高度
	* @param[in] cp 图片在RichEdit中插入的位置
	* @return bool true 成功，false 失败
	*/
	bool InsertCustomItem(InsertCustomItemErrorCallback callback, bool total_count_limit, const std::wstring& file, const std::wstring& face_tag, RE_OLE_TYPE ole_type
		, int face_id, bool scale, int scale_width = 0, int scale_height = 0, LONG cp = REO_CP_SELECTION);

	bool InsertDescriptionItem(InsertCustomItemErrorCallback callback, bool total_count_limit, const std::wstring& description
		, const std::wstring& face_tag, RE_OLE_TYPE ole_type, COLORREF clrBg = RGB(255, 255, 255), COLORREF clrfont = RGB(35, 142, 250), int fontSize = 20, LONG cp = REO_CP_SELECTION);
	//------------------------------add end----------------------------
protected:
	enum { 
		DEFAULT_TIMERID = 20,
	};

    CTxtWinHost* m_pTwh;
	IRichEditOleCallbackEx *m_pOleCallback = nullptr;

    bool m_bVScrollBarFixing;
    bool m_bWantTab;
    bool m_bWantReturn;
    bool m_bWantCtrlReturn;
    bool m_bTransparent;
    bool m_bRich;
    bool m_bReadOnly;
    bool m_bWordWrap;
    DWORD m_dwTextColor;
    int m_iFont;
    int m_iLimitText;
    LONG m_lTwhStyle;
	bool m_bDrawCaret;
	bool m_bInited;
	RECT	m_rcTextPadding;
};

} // namespace dui

#endif // __UIRICHEDIT_H__
