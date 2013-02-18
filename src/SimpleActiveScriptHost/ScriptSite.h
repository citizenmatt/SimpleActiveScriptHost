#pragma once

#include "NamedItemsHelper.h"
#include "ScriptErrorInfo.h"

class CScriptSite : public IActiveScriptSite,
					public IActiveScriptSiteDebug32
{
public:
	CScriptSite()
	{
		m_dwRef = 0;
		m_applicationCookie = 0;

		namedItems = gcnew CNamedItemsHelper();
	}

	~CScriptSite()
	{
		if (m_applicationCookie != 0)
			m_processDebugManager->RemoveApplication(m_applicationCookie);
		delete ((CNamedItemsHelper^)namedItems);
	}

	// IUnknown members
public:
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppv)
	{
		if (!ppv)
			return E_POINTER;

		*ppv = nullptr;

		if (riid == IID_IUnknown)
			*ppv = static_cast<IUnknown *>(static_cast<IActiveScriptSite *>(this));
		else if (riid == IID_IActiveScriptSite)
			*ppv = static_cast<IActiveScriptSite *>(this);
		else if (riid == IID_IActiveScriptSiteDebug32)
			*ppv = static_cast<IActiveScriptSiteDebug32 *>(this);

		if (*ppv)
			AddRef();

		return *ppv ? S_OK : E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		::InterlockedIncrement((LONG *)&m_dwRef);
		return m_dwRef;
	}

	STDMETHODIMP_(ULONG) Release()
	{
		ULONG l = ::InterlockedDecrement((LONG *)&m_dwRef);
		if (l == 0)
			delete this;
		return l;
	}

	// Helper methods
public:
	HRESULT SetEngine(IActiveScript *engine)
	{
		m_scriptEngine = engine;

		IActiveScriptParse32Ptr parser(m_scriptEngine);
		HRESULT hr = parser->InitNew();

		hr = m_scriptEngine->SetScriptState(SCRIPTSTATE_CONNECTED);
		return hr;
	}

	HRESULT Close()
	{
		return m_scriptEngine->Close();
	}

	HRESULT CreateApplication(LPCWSTR applicationName)
	{
		HRESULT hr = m_processDebugManager.CreateInstance(CLSID_ProcessDebugManager);

		hr = m_processDebugManager->CreateApplication(&m_debugApplication);
		hr = m_debugApplication->SetName(applicationName);
		hr = m_processDebugManager->AddApplication(m_debugApplication, &m_applicationCookie);
		return hr;
	}

	HRESULT ParseScriptText(LPCWSTR scriptText, LPCWSTR scriptName)
	{
		DWORD dwSourceContextCookie = 0;

		HRESULT hr = m_processDebugManager->CreateDebugDocumentHelper(nullptr, &debugDocumentHelper);
		hr = debugDocumentHelper->Init(m_debugApplication, scriptName, scriptName, TEXT_DOC_ATTR_READONLY);
		hr = debugDocumentHelper->Attach(nullptr);
		hr = debugDocumentHelper->AddUnicodeText(scriptText);
		ULONG len = lstrlenW(scriptText);
		hr = debugDocumentHelper->DefineScriptBlock(0, len, m_scriptEngine, FALSE, &dwSourceContextCookie);

		IActiveScriptParse32Ptr parser(m_scriptEngine);

		hr = parser->ParseScriptText(scriptText, nullptr, nullptr, nullptr, dwSourceContextCookie, 0,
			SCRIPTTEXT_ISVISIBLE | SCRIPTTEXT_HOSTMANAGESSOURCE, nullptr, &ScriptErrorInfo.ExcepInfo);
		return hr;
	}

	HRESULT GetScriptDispatch(IDispatch **ppdisp)
	{
		return m_scriptEngine->GetScriptDispatch(nullptr, ppdisp);
	}

	HRESULT CallMethod(LPCOLESTR methodName, VARIANT *vtResult)
	{
		IDispatchPtr disp;
		HRESULT hr = m_scriptEngine->GetScriptDispatch(nullptr, &disp);

		DISPID rgDispId;
		hr = disp->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&methodName), 1, 0, &rgDispId);

		::DISPPARAMS params = { 0 };
		UINT err = 0;
		hr = disp->Invoke(rgDispId, IID_NULL, 0, DISPATCH_METHOD, &params, vtResult, &ScriptErrorInfo.ExcepInfo, &err);
		return hr;
	}

	HRESULT AddNamedItem(LPCOLESTR name)
	{
		return m_scriptEngine->AddNamedItem(name, SCRIPTITEM_ISVISIBLE | SCRIPTITEM_ISSOURCE);
	}

	// IActiveScriptSite members
public:
	STDMETHODIMP GetLCID(LCID *plcid)
	{
		*plcid = 0;
		return E_NOTIMPL;
	}

    STDMETHODIMP GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppiunkItem, ITypeInfo **ppti)
	{
		return namedItems->GetItemInfo(pstrName, dwReturnMask, ppiunkItem, ppti);
	}

    STDMETHODIMP GetDocVersionString(BSTR *pbstrVersion)
	{
		*pbstrVersion = nullptr;
		return S_OK;
	}

    STDMETHODIMP OnScriptTerminate(const VARIANT *pvarResult, const ::EXCEPINFO *pexcepinfo)
	{
		return S_OK;
	}

    STDMETHODIMP OnStateChange(SCRIPTSTATE ssScriptState)
	{
		return S_OK;
	}

    STDMETHODIMP OnScriptError(IActiveScriptError *pScriptError)
	{
		BSTR bstr = nullptr;
		HRESULT hr = pScriptError->GetSourceLineText(&bstr);
		if (SUCCEEDED(hr))
			ScriptErrorInfo.LineText.Attach(bstr);

		DWORD sourceContext;
		hr = pScriptError->GetSourcePosition(&sourceContext, &ScriptErrorInfo.LineNumber, &ScriptErrorInfo.CharacterPosition);
		ScriptErrorInfo.LineNumber++;
		ScriptErrorInfo.CharacterPosition++;

		hr = pScriptError->GetExceptionInfo(&ScriptErrorInfo.ExcepInfo);
		return S_OK;
	}

	STDMETHODIMP OnEnterScript(void) { return S_OK; }
	STDMETHODIMP OnLeaveScript(void) { return S_OK; }

	// IActiveScriptSiteDebug32 members
public:
	STDMETHODIMP GetDocumentContextFromPosition(DWORD dwSourceContext, ULONG uCharacterOffset, ULONG uNumChars, IDebugDocumentContext **ppsc)
	{
		ULONG ulStartOffset = 0, ulLength = 0;
		HRESULT hr = debugDocumentHelper->GetScriptBlockInfo(dwSourceContext, nullptr, &ulStartOffset, &ulLength);
		return debugDocumentHelper->CreateDebugDocumentContext(ulStartOffset + uCharacterOffset, uNumChars, ppsc);
	}

	STDMETHODIMP GetApplication(IDebugApplication32 **ppda)
	{
		if (!ppda) return E_POINTER;
		*ppda = nullptr;
		if (!m_debugApplication) return E_UNEXPECTED;

		*ppda = m_debugApplication.GetInterfacePtr();
		return S_OK;
	}

	STDMETHODIMP GetRootApplicationNode(IDebugApplicationNode **ppdanRoot)
	{
		if (!ppdanRoot) return E_POINTER;
		if (!m_debugApplication) return E_UNEXPECTED;
		return m_debugApplication->GetRootNode(ppdanRoot);
	}

	STDMETHODIMP OnScriptErrorDebug(IActiveScriptErrorDebug *pErrorDebug, BOOL *pfEnterDebugger, BOOL *pfCallOnScriptErrorWhenContinuing)
	{
		*pfEnterDebugger = FALSE;
		*pfCallOnScriptErrorWhenContinuing = FALSE;

		IActiveScriptErrorPtr pError(pErrorDebug);
		OnScriptError(pError);

		return S_OK;
	}

private:
	DWORD m_dwRef;
	DWORD m_applicationCookie;

	IActiveScriptPtr m_scriptEngine;
	IProcessDebugManager32Ptr m_processDebugManager;
	IDebugApplication32Ptr m_debugApplication;

	IDebugDocumentHelper32Ptr debugDocumentHelper;

public:
	gcroot<CNamedItemsHelper^> namedItems;
	ScriptErrorInfo ScriptErrorInfo;
};