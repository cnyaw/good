// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlmisc.h>
#include <atlcom.h>
#include <atlhost.h>
#include <atlwin.h>
#include <atlctl.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlfind.h>
#include <atlscrl.h>
#include <atlcrack.h>
#include <atlddx.h>
#include <atlsplit.h>

#include <cmath>

#include <algorithm>
#include <map>
#include <sstream>
#include <vector>

#include "PropertyList.h"
