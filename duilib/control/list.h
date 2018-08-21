#ifndef __UILIST_H__
#define __UILIST_H__
#pragma once

namespace dui {

	typedef int (CALLBACK *PULVCompareFunc)(UINT_PTR, UINT_PTR, UINT_PTR);

	class IList
	{
	public:
		virtual int GetCurSel() const = 0;
		virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true) = 0;
		virtual void DoEvent(Event& event) = 0;
	};

	class DUILIB_API List : public ScrollBox, public IList
	{
	public:
		List();
		virtual ~List(){
			printf("~List\n");
		};
		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool GetScrollSelect();
		void SetScrollSelect(bool bScrollSelect);
		int GetCurSel() const;
		virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true) override;

		bool SetItemIndex(Control* pControl, int iIndex);
		bool SetMultiItemIndex(Control* pStartControl, int iCount, int iNewStartIndex);
		bool Add(Control* pControl);
		bool AddAt(Control* pControl, int iIndex);
		bool Remove(Control* pControl, bool bDoNotDestroy = false);
		bool RemoveAt(int iIndex, bool bDoNotDestroy = false);
		void RemoveAll();

		void EnsureVisible(int iIndex);
		void Scroll(int dx, int dy);

		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void DoEvent(Event& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

	protected:
		bool m_bScrollSelect;
		int m_iCurSel;
		/* IListCallback* m_pCallback;*/
	};
} // namespace dui

#endif // __UILIST_H__
