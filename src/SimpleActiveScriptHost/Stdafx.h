// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define __RPC__inout_xcount(x)
#define __RPC__in_xcount(x)

#include <vcclr.h>

#include <Windows.h>

#include <ActivScp.h>
#include "activdbg.h"

#include <comdef.h>
#include <comdefsp.h>

#if defined(__IActiveScriptParse32_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IActiveScriptParse32, __uuidof(IActiveScriptParse32));
#endif// #if defined(__IActiveScriptParse32_INTERFACE_DEFINED__)

_COM_SMARTPTR_TYPEDEF(IProcessDebugManager32, __uuidof(IProcessDebugManager32));
_COM_SMARTPTR_TYPEDEF(IDebugApplication32, __uuidof(IDebugApplication32));
_COM_SMARTPTR_TYPEDEF(IDebugDocumentHelper32, __uuidof(IDebugDocumentHelper32));

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
