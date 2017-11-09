#ifndef __UIDLGBUILDER_H__
#define __UIDLGBUILDER_H__

#pragma once

namespace dui {

class IDialogBuilderCallback
{
public:
    virtual Control* CreateControl(LPCTSTR pstrClass) = 0;
};


class DUILIB_API CDialogBuilder
{
public:
    CDialogBuilder();

	/*ps: func 'Create' and '_Parse'
	when call CPaintManager::FillBox(ScrollContainer* pFilledContainer, ...), pass pFilledContainer to last param of this func;
	when pFilledContainer is null, the same as you known;*/
    Control* Create(STRINGorID xml, LPCTSTR type = NULL, IDialogBuilderCallback* pCallback = NULL,
        CPaintManager* pManager = NULL, Control* pParent = NULL, ScrollContainer *pFilledContainer = NULL);
    Control* Create(IDialogBuilderCallback* pCallback = NULL, CPaintManager* pManager = NULL,
		Control* pParent = NULL, ScrollContainer *pFilledContainer = NULL);

    CMarkup* GetMarkup();

    void GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const;
    void GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const;
private:
	Control* _Parse(CMarkupNode* parent, Control* pParent = NULL, CPaintManager* pManager = NULL, ScrollContainer *pFilledContainer = NULL);

    CMarkup m_xml;
    IDialogBuilderCallback* m_pCallback;
    LPCTSTR m_pstrtype;
};

} // namespace dui

#endif // __UIDLGBUILDER_H__
