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
    Control* Create(STRINGorID xml, LPCTSTR type = NULL, IDialogBuilderCallback* pCallback = NULL,
        CPaintManager* pManager = NULL, Control* pParent = NULL);
    Control* Create(IDialogBuilderCallback* pCallback = NULL, CPaintManager* pManager = NULL,
        Control* pParent = NULL);

    CMarkup* GetMarkup();

    void GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const;
    void GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const;
private:
    Control* _Parse(CMarkupNode* parent, Control* pParent = NULL, CPaintManager* pManager = NULL);

    CMarkup m_xml;
    IDialogBuilderCallback* m_pCallback;
    LPCTSTR m_pstrtype;
};

} // namespace dui

#endif // __UIDLGBUILDER_H__
