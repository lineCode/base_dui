#include "stdafx.h"

namespace dui {

CDialogBuilder::CDialogBuilder() : m_pCallback(NULL), m_pstrtype(NULL)
{

}

Control* CDialogBuilder::Create(STRINGorID xml, LPCTSTR type, IDialogBuilderCallback* pCallback, 
	UIManager* pManager, Control* pParent, Box *pFilledContainer)
{
	//资源ID为0-65535，两个字节；字符串指针为4个字节
	//字符串以<开头认为是XML字符串，否则认为是XML文件

    if( HIWORD(xml.m_lpstr) != NULL ) {
        if( *(xml.m_lpstr) == _T('<') ) {
            if( !m_xml.Load(xml.m_lpstr) ) return NULL;
        }
        else {
			if (!m_xml.LoadFromFile(xml.m_lpstr, pManager)) return NULL;
        }
    }
    else {
        HRSRC hResource = ::FindResource(UIManager::GetResourceDll(), xml.m_lpstr, type);
        if( hResource == NULL ) return NULL;
        HGLOBAL hGlobal = ::LoadResource(UIManager::GetResourceDll(), hResource);
        if( hGlobal == NULL ) {
            FreeResource(hResource);
            return NULL;
        }

        m_pCallback = pCallback;
        if( !m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(UIManager::GetResourceDll(), hResource) )) return NULL;
        ::FreeResource(hResource);
        m_pstrtype = type;
    }

	return Create(pCallback, pManager, pParent, pFilledContainer);
}

Control* CDialogBuilder::Create(IDialogBuilderCallback* pCallback, UIManager* pManager, Control* pParent, Box *pFilledContainer)
{
	m_pCallback = pCallback;
	MarkupNode root = m_xml.GetRoot();
    if( !root.IsValid() ) return NULL;

    if( pManager ) {
        LPCTSTR pstrClass = NULL;
        int nAttributes = 0;
        LPCTSTR pstrName = NULL;
        LPCTSTR pstrValue = NULL;
        LPTSTR pstr = NULL;
        for( MarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) {
            pstrClass = node.GetName();
            if( _tcsicmp(pstrClass, _T("Image")) == 0 ) {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pImageName = NULL;
                LPCTSTR pImageResType = NULL;
                DWORD mask = 0;
				bool shared = false;
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    if( _tcsicmp(pstrName, _T("name")) == 0 ) {
                        pImageName = pstrValue;
                    }
                    else if( _tcsicmp(pstrName, _T("restype")) == 0 ) {
                        pImageResType = pstrValue;
                    }
                    else if( _tcsicmp(pstrName, _T("mask")) == 0 ) {
                        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
                        mask = _tcstoul(pstrValue, &pstr, 16);
                    }
					else if( _tcsicmp(pstrName, _T("shared")) == 0 ) {
						shared = (_tcsicmp(pstrValue, _T("true")) == 0);
					}
                }
                if( pImageName ) pManager->AddImage(pImageName, pImageResType, mask, shared);
            }
            else if( _tcsicmp(pstrClass, _T("Font")) == 0 ) {
                nAttributes = node.GetAttributeCount();
				//int id=-1;
				int id = pManager->GetCustomFontCount();		//djj:for load font.[20170421]
                LPCTSTR pFontName = NULL;
                int size = 12;
                bool bold = false;
                bool underline = false;
                bool italic = false;
                bool defaultfont = false;
				bool shared = false;
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
					if( _tcsicmp(pstrName, _T("id")) == 0 ) {
						id = _tcstol(pstrValue, &pstr, 10);
					}
                    else if( _tcsicmp(pstrName, _T("name")) == 0 ) {
                        pFontName = pstrValue;
                    }
                    else if( _tcsicmp(pstrName, _T("size")) == 0 ) {
                        size = _tcstol(pstrValue, &pstr, 10);
                    }
                    else if( _tcsicmp(pstrName, _T("bold")) == 0 ) {
                        bold = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                    else if( _tcsicmp(pstrName, _T("underline")) == 0 ) {
                        underline = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                    else if( _tcsicmp(pstrName, _T("italic")) == 0 ) {
                        italic = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                    else if( _tcsicmp(pstrName, _T("default")) == 0 ) {
                        defaultfont = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                }
                if( id >= 0 && pFontName ) {
                    pManager->AddFont(id, pFontName, size, bold, underline, italic, shared);
					if (defaultfont) pManager->SetDefaultFont(pFontName, pManager->GetDPIObj()->Scale(size), bold, underline, italic, shared);
                }
            }
            else if( _tcsicmp(pstrClass, _T("Default")) == 0 ) {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pControlName = NULL;
                LPCTSTR pControlValue = NULL;
				bool shared = false;
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    if( _tcsicmp(pstrName, _T("name")) == 0 ) {
                        pControlName = pstrValue;
                    }
                    else if( _tcsicmp(pstrName, _T("value")) == 0 ) {
                        pControlValue = pstrValue;
                    }
					else if( _tcsicmp(pstrName, _T("shared")) == 0 ) {
						shared = (_tcsicmp(pstrValue, _T("true")) == 0);
					}
                }
                if( pControlName ) {
                    pManager->AddDefaultAttributeList(pControlName, pControlValue, shared);
                }
            }
			else if( _tcsicmp(pstrClass, _T("MultiLanguage")) == 0 ) {
				nAttributes = node.GetAttributeCount();
				int id = -1;
				LPCTSTR pMultiLanguage = NULL;
				for( int i = 0; i < nAttributes; i++ ) {
					pstrName = node.GetAttributeName(i);
					pstrValue = node.GetAttributeValue(i);
					if( _tcsicmp(pstrName, _T("id")) == 0 ) {
						id = _tcstol(pstrValue, &pstr, 10);
					}
					else if( _tcsicmp(pstrName, _T("value")) == 0 ) {
						pMultiLanguage = pstrValue;
					}
				}
				if (id >= 0 && pMultiLanguage ) {
					pManager->AddMultiLanguageString(id, pMultiLanguage);
				}
			}
        }

        pstrClass = root.GetName();
        if( _tcsicmp(pstrClass, _T("Window")) == 0 ) {
            if( pManager->GetPaintWindow() ) {
                int nAttributes = root.GetAttributeCount();
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = root.GetAttributeName(i);
                    pstrValue = root.GetAttributeValue(i);
                    pManager->SetWindowAttribute(pstrName, pstrValue);
                }
            }
        }
    }
	return _Parse(&root, pParent, pManager, pFilledContainer);
}

Markup* CDialogBuilder::GetMarkup()
{
    return &m_xml;
}

void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
}

void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorLocation(pstrSource, cchMax);
}

Control* CDialogBuilder::_Parse(MarkupNode* pRoot, Control* pParent, UIManager* pManager, Box* pFilledContainer)
{
	Box* pContainer = NULL;
    Control* pReturn = NULL;
    for( MarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling() ) {
        LPCTSTR pstrClass = node.GetName();
        if( _tcsicmp(pstrClass, _T("Image")) == 0 || _tcsicmp(pstrClass, _T("Font")) == 0 \
            || _tcsicmp(pstrClass, _T("Default")) == 0 
			|| _tcsicmp(pstrClass, _T("MultiLanguage")) == 0 ) continue;

        Control* pControl = NULL;
        if( _tcsicmp(pstrClass, _T("Include")) == 0 ) {
            if( !node.HasAttributes() ) continue;
            int count = 1;
            LPTSTR pstr = NULL;
            TCHAR szValue[500] = { 0 };
            SIZE_T cchLen = lengthof(szValue) - 1;
            if ( node.GetAttributeValue(_T("count"), szValue, cchLen) )
                count = _tcstol(szValue, &pstr, 10);
            cchLen = lengthof(szValue) - 1;
            if ( !node.GetAttributeValue(_T("source"), szValue, cchLen) ) continue;
			CDialogBuilder builder;
			for ( int i = 0; i < count; i++ ) {
				if (!builder.GetMarkup()->IsValid())
				{
					if( m_pstrtype != NULL ) { // 使用资源dll，从资源中读取
						WORD id = (WORD)_tcstol(szValue, &pstr, 10); 
						pControl = builder.Create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
					}
					else 
						pControl = builder.Create((LPCTSTR)szValue, (UINT)0, m_pCallback, pManager, pParent);
				}
				else
					pControl = builder.Create(m_pCallback, pManager, pParent);
			}
            continue;
        }
		//树控件XML解析
		else if( _tcsicmp(pstrClass, _T("TreeNode")) == 0 ) {
			TreeNode* pParentNode	= static_cast<TreeNode*>(pParent->GetInterface(_T("TreeNode")));
			TreeNode* pNode			= new TreeNode();
			if(pParentNode){
				if (!pParentNode->AddChildNode(pNode)){
					delete pNode;
					continue;
				}
			}

			// 若有控件默认配置先初始化默认属性
			if( pManager ) {
				pNode->SetManager(pManager, NULL, false);
				LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
				if( pDefaultAttributes ) {
					pNode->SetAttributeList(pDefaultAttributes);
				}
			}

			// 解析所有属性并覆盖默认属性
			if( node.HasAttributes() ) {
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				// Set ordinary attributes
				int nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) {
					ASSERT(i == 0 || _tcscmp(node.GetAttributeName(i), _T("class")) != 0);
					pNode->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
				}
			}

			//检索子节点及附加控件
			if(node.HasChildren()){
				Control* pSubControl = _Parse(&node,pNode,pManager);
				if(pSubControl && _tcsicmp(pSubControl->GetClass(),_T("TreeNodeUI")) != 0)
				{
					// 					pSubControl->SetFixedWidth(30);
					// 					HBox* pHorz = pNode->GetTreeNodeHoriznotal();
					// 					pHorz->Add(new Edit());
					// 					continue;
				}
			}

			if(!pParentNode){
				Tree* pTree = static_cast<Tree*>(pParent->GetInterface(_T("TreeView")));
				ASSERT(pTree);
				if (pTree == NULL) return NULL;
				if (!pTree->AddChildNode(pNode)) {
					delete pNode;
					continue;
				}
			}
			continue;
		}
        else {
			if (pFilledContainer && pFilledContainer->GetInterface(pstrClass))
			{
				pControl = pFilledContainer;
			}
			else
			{
				DUITRACE(_T("Create Control: %s"), pstrClass);
				SIZE_T cchLen = _tcslen(pstrClass);
				switch (cchLen) {
				case 3:
					if (_tcsicmp(pstrClass, DUI_CTR_BOX) == 0)					pControl = new Box;
					break;
				case 4:
					if (_tcsicmp(pstrClass, DUI_CTR_HBOX) == 0)					pControl = new HBox;
					else if (_tcsicmp(pstrClass, DUI_CTR_VBOX) == 0)			pControl = new VBox;
					else if(_tcsicmp(pstrClass, DUI_CTR_EDIT) == 0)             pControl = new Edit;
					else if (_tcsicmp(pstrClass, DUI_CTR_LIST) == 0)             pControl = new List;
					else if (_tcsicmp(pstrClass, DUI_CTR_TEXT) == 0)             pControl = new CTextUI;
					else if (_tcsicmp(pstrClass, DUI_CTR_TREE) == 0)             pControl = new Tree;
					break;
				case 5:
					if (_tcsicmp(pstrClass, DUI_CTR_COMBO) == 0)                 pControl = new Combo;
					else if (_tcsicmp(pstrClass, DUI_CTR_LABEL) == 0)            pControl = new Label;
					//else if( _tcsicmp(pstrClass, DUI_CTR_FLASH) == 0 )           pControl = new Flash;
					break;
				case 6:
					if (_tcsicmp(pstrClass, DUI_CTR_BUTTON) == 0)                pControl = new Button;
					else if (_tcsicmp(pstrClass, DUI_CTR_OPTION) == 0)           pControl = new OptionBtn;
					else if (_tcsicmp(pstrClass, DUI_CTR_SLIDER) == 0)           pControl = new Slider;
					else if (_tcsicmp(pstrClass, DUI_CTR_TABBOX) == 0)			pControl = new TabBox;
					break;
				case 7:
					if (_tcsicmp(pstrClass, DUI_CTR_CONTROL) == 0)               pControl = new Control;
					else if (_tcsicmp(pstrClass, DUI_CTR_ACTIVEX) == 0)          pControl = new ActiveX;
					else if (_tcscmp(pstrClass, DUI_CTR_GIFANIM) == 0)            pControl = new GifAnim;
					else if (_tcsicmp(pstrClass, DUI_CTR_TILEBOX) == 0)			pControl = new TileBox;
					break;
				case 8:
					if (_tcsicmp(pstrClass, DUI_CTR_PROGRESS) == 0)              pControl = new Progress;
					else if (_tcsicmp(pstrClass, DUI_CTR_RICHEDIT) == 0)         pControl = new RichEdit;
					else if (_tcsicmp(pstrClass, DUI_CTR_CHECKBOX) == 0)		  pControl = new CheckBtn;
					else if (_tcsicmp(pstrClass, DUI_CTR_COMBOBOX) == 0)		  pControl = new Combo;
					else if (_tcsicmp(pstrClass, DUI_CTR_DATETIME) == 0)		  pControl = new DateTime;
					else if (_tcsicmp(pstrClass, DUI_CTR_LISTVIEW) == 0)          pControl = new ListView;
					else if (_tcsicmp(pstrClass, DUI_CTR_TREENODE) == 0)			pControl = new TreeNode;
					else if (_tcsicmp(pstrClass, DUI_CTR_CHILDBOX) == 0)			pControl = new ChildBox;
					else if (_tcsicmp(pstrClass, DUI_CTR_LABELBOX) == 0)			pControl = new LabelBox;
					break;
				case 9:
					if (_tcsicmp(pstrClass, DUI_CTR_SCROLLBAR) == 0)			pControl = new ScrollBar;
					else if (_tcsicmp(pstrClass, DUI_CTR_SCROLLBOX) == 0)		pControl = new ScrollBox;
					else if (_tcsicmp(pstrClass, DUI_CTR_BUTTONBOX) == 0)		pControl = new ButtonBox;
					else if (_tcsicmp(pstrClass, DUI_CTR_OPTIONBOX) == 0)		pControl = new OptionBox;
					break;
				case 10:
					if (_tcsicmp(pstrClass, DUI_CTR_LISTHEADER) == 0)            pControl = new ListHeader;
					else if (_tcsicmp(pstrClass, DUI_CTR_WEBBROWSER) == 0)       pControl = new WebBrowser;
					break;
				case 11:
					if (_tcsicmp(pstrClass, DUI_CTR_LISTELEMENT) == 0)		 pControl = new ListElement;
					break;
				case 14:
					if (_tcsicmp(pstrClass, DUI_CTR_LISTHEADERITEM) == 0)		pControl = new ListHeaderItem;
					break;
				case 20:
					if (_tcsicmp(pstrClass, DUI_CTR_LISTCONTAINERELEMENT) == 0)  pControl = new ListContainerElement;
					break;
				}
				// User-supplied control factory
				if (pControl == NULL) {
					PtrArray* pPlugins = UIManager::GetPlugins();
					LPCREATECONTROL lpCreateControl = NULL;
					for (int i = 0; i < pPlugins->GetSize(); ++i) {
						lpCreateControl = (LPCREATECONTROL)pPlugins->GetAt(i);
						if (lpCreateControl != NULL) {
							pControl = lpCreateControl(pstrClass);
							if (pControl != NULL) break;
						}
					}
				}
				if (pControl == NULL && m_pCallback != NULL) {
					pControl = m_pCallback->CreateControl(pstrClass);
				}
			}
			
        }

		if( pControl == NULL )
		{
			//DUITRACE(_T("Unknow Control:%s"),pstrClass);
			_tprintf(_T("Unknow Control:%s"), pstrClass);
			assert(0 && "CDialogBuilder::_Parse:Unknow Control");
			continue;
		}

        // Add children
        if( node.HasChildren() ) {
            _Parse(&node, pControl, pManager);
        }
        TCHAR szValue[256] = { 0 };
        int cchLen = lengthof(szValue) - 1;
        // Attach to parent
        // 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
		if( pParent != NULL ) {
            LPCTSTR lpValue = szValue;
            if( node.GetAttributeValue(_T("cover"), szValue, cchLen) && _tcscmp(lpValue, _T("true")) == 0 ) {
                pParent->SetCover(pControl);
            }
            else {
               /* TreeNode* pContainerNode = static_cast<TreeNode*>(pParent->GetInterface(DUI_CTR_TREENODE));
                if(pContainerNode)
                    pContainerNode->GetTreeNodeHoriznotal()->Add(pControl);
                else*/
                {
					if (pContainer == NULL) pContainer = static_cast<Box*>(pParent->GetInterface(DUI_CTR_BOX));
                    ASSERT(pContainer);
                    if( pContainer == NULL ) return NULL;
                    if( !pContainer->Add(pControl) ) {
                        pControl->Delete();
                        continue;
                    }
                }
            }
		}
        // Init default attributes
        if( pManager ) {
            pControl->SetManager(pManager, NULL, false);
            LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
            if( pDefaultAttributes ) {
                pControl->SetAttributeList(pDefaultAttributes);
            }
        }
        // Process attributes
        if( node.HasAttributes() ) {
            // Set ordinary attributes
            int nAttributes = node.GetAttributeCount();
            for( int i = 0; i < nAttributes; i++ ) {
				ASSERT(i == 0 || _tcscmp(node.GetAttributeName(i), _T("class")) != 0);
                pControl->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
            }
        }
        if( pManager ) {
            pControl->SetManager(NULL, NULL, false);
        }
        // Return first item
        if( pReturn == NULL ) pReturn = pControl;
    }
    return pReturn;
}

} // namespace dui
