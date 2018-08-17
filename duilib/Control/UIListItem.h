#ifndef __UILIST_ITEM_H__
#define __UILIST_ITEM_H__
#pragma once

namespace dui {

	class IList;
	class IListItem
	{
	public:
		virtual int GetIndex() const = 0;
		virtual void SetIndex(int iIndex) = 0;
		virtual IList* GetOwner() = 0;
		virtual void SetOwner(Control* pOwner) = 0;
		virtual bool IsSelected() const = 0;
		virtual bool Select(bool bSelect = true, bool bTriggerEvent = true) = 0;
		//------------------add by djj----------------------
		virtual void SetItemText(String text) = 0;
		virtual String GetItemText() = 0;
	};

	class DUILIB_API ListElement : public Label, public IListItem
	{
	public:
		ListElement();
		virtual ~ListElement(){ printf("~ListElement()\n"); };

		//vitrual funcs of class Control
		virtual LPCTSTR GetClass() const;
		virtual UINT GetControlFlags() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void SetEnabled(bool bEnable = true);
		virtual void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
		virtual bool Activate();
		virtual void DoEvent(TEvent& event);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void DrawItemBk(HDC hDC, const RECT& rcItem);
		virtual void DrawItemText(HDC hDC, const RECT& rcItem){};

		//vitrual funcs of class IListItem
		virtual int GetIndex() const;
		virtual void SetIndex(int iIndex);
		virtual IList* GetOwner() override;
		virtual void SetOwner(Control* pOwner) override;
		void SetVisible(bool bVisible = true);
		virtual bool IsSelected() const;
		virtual bool Select(bool bSelect = true, bool bTriggerEvent = true);
		virtual void SetItemText(String text){ SetText(text.c_str()); };
		virtual String GetItemText(){ return GetText(); };

	protected:
		int m_iIndex;
		bool m_bSelected;
		UINT m_uButtonState;
		IList* m_pOwner;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class DUILIB_API ListContainerElement : public Container, public IListItem
	{
	public:
		ListContainerElement();

		//vitrual funcs of class Control
		virtual LPCTSTR GetClass() const;
		virtual UINT GetControlFlags() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void Invalidate();		// 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
		virtual bool Activate();
		virtual void DoEvent(TEvent& event);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual bool DoPaint(HDC hDC, const RECT& rcPaint, Control* pStopControl);
		virtual void DrawItemText(HDC hDC, const RECT& rcItem);
		virtual void DrawItemBk(HDC hDC, const RECT& rcItem);
		void AttachItemClick(const EventCallback& callback){
			event_map[UIEVENT_ITEMCLICK] += callback;
		};

		//vitrual funcs of class IListItem
		virtual int GetIndex() const;
		virtual void SetIndex(int iIndex);
		virtual IList* GetOwner();
		virtual void SetOwner(Control* pOwner);
		virtual void SetVisible(bool bVisible = true);
		virtual void SetEnabled(bool bEnable = true);
		virtual bool IsSelected() const;
		virtual bool Select(bool bSelect = true, bool bTriggerEvent = true);
		virtual void SetItemText(String text){ return; };
		virtual String GetItemText(){ return _T(""); };

	protected:
		int m_iIndex;
		bool m_bSelected;
		UINT m_uButtonState;
		IList* m_pOwner;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class DUILIB_API ListHeaderItem : public Control
	{
	public:
		ListHeaderItem();
		virtual ~ListHeaderItem(){
			printf("~ListHeaderItem\n");
		};
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);

		bool IsDragable() const;
		void SetDragable(bool bDragable);
		DWORD GetSepWidth() const;
		void SetSepWidth(int iWidth);
		DWORD GetTextStyle() const;
		void SetTextStyle(UINT uStyle);
		DWORD GetTextColor() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetSepColor() const;
		void SetSepColor(DWORD dwSepColor);
		void SetTextPadding(RECT rc);
		RECT GetTextPadding() const;
		void SetFont(int index);
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);
		LPCTSTR GetNormalImage() const;
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage() const;
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage() const;
		void SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage() const;
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetSepImage() const;
		void SetSepImage(LPCTSTR pStrImage);

		void DoEvent(TEvent& event);
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		RECT GetThumbRect() const;

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
		POINT ptLastMouse;
		bool m_bDragable;
		UINT m_uButtonState;
		int m_iSepWidth;
		DWORD m_dwTextColor;
		DWORD m_dwSepColor;
		int m_iFont;
		UINT m_uTextStyle;
		bool m_bShowHtml;
		RECT m_rcTextPadding;
		TDrawInfo m_diNormal;
		TDrawInfo m_diHot;
		TDrawInfo m_diPushed;
		TDrawInfo m_diFocused;
		TDrawInfo m_diSep;
	};
}

#endif // __UILIST_ITEM_H__
