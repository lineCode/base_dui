// Copyright (c) 2010-2011, duilib develop team(www.duilib.com).
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted provided that the 
// following conditions are met.
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above 
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials 
// provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifdef UILIB_STATIC
#	define DUILIB_API 
#else
#	if defined(UILIB_EXPORTS)
#		if	defined(_MSC_VER)
#			define DUILIB_API __declspec(dllexport)
#		else
#			define DUILIB_API 
#		endif
#	else
#		if defined(_MSC_VER)
#			define DUILIB_API __declspec(dllimport)
#		else
#			define DUILIB_API 
#		endif
#	endif
#endif

#define UILIB_COMDAT __declspec(selectany)
#define DUI_VERSION	"1.01.01.0002"

#if defined _M_IX86
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <stdio.h>
#include <string>
#include <functional>
#include <map>

#ifdef _UNICODE
typedef std::wstring String;
#else
typedef std::string String;
#endif

#include "core/base.h"
#include "core/define.h"
#include "core/delegate.h"

#include "core/shadow.h"
#include "core/manager.h"
#include "core/window.h"
#include "core/control.h"
#include "core/markup.h"
#include "core/builder.h"
#include "core/render.h"
#include "core/win_impl_base.h"
#include "utils/dpi.h"

#include "control/label.h"
#include "control/text.h"
#include "control/edit.h"
#include "control/button.h"
#include "control/option.h"
#include "control/check.h"
#include "control/scroll.h"

#include "box/box.h"
#include "box/scrollbox.h"
#include "box/vbox.h"
#include "box/hbox.h"
#include "box/tilebox.h"
#include "box/tabbox.h"
#include "box/childbox.h"

#include "control/progress.h"
#include "control/slider.h"

#include "control/richedit.h"
#include "control/datetime.h"

#include "control/activeX.h"
#include "control/browser.h"
#include "control/gif.h"
#include "control/flash.h"

#include "control/list_item.h"
#include "control/list.h"
#include "control/listview.h"
#include "control/combo.h"
#include "control/tree.h"

#include "control/menu.h"			//djj[20170424] : add
#include "extend/labelbox.h"
#include "extend/buttonbox.h"
#include "extend/optionbox.h"

#ifdef _DEBUG
#include "log.h"
extern LogFile *g_plog;
#endif

//#include "Control/UIMenu.h"
