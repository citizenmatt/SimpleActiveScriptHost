#pragma once

class CScriptSite : public IActiveScriptSite,
					public IActiveScriptSiteDebug32
{
public:
	CScriptSite()
	{
		m_dwRef = 0;
		m_applicationCookie = 0;
	}

	~CScriptSite()
	{
		if (m_applicationCookie != 0)
			m_processDebugManager->RemoveApplication(m_applicationCookie);
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
		return parser->InitNew();
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

		EXCEPINFO ei;
		hr = parser->ParseScriptText(scriptText, nullptr, nullptr, nullptr, dwSourceContextCookie, 0, SCRIPTTEXT_ISVISIBLE | SCRIPTTEXT_HOSTMANAGESSOURCE, nullptr, &ei);

		getchar();

		hr = m_scriptEngine->SetScriptState(SCRIPTSTATE_CONNECTED);
		return hr;
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
		return E_FAIL;
	}

    STDMETHODIMP GetDocVersionString(BSTR *pbstrVersion)
	{
		*pbstrVersion = nullptr;
		return S_OK;
	}

    STDMETHODIMP OnScriptTerminate(const VARIANT *pvarResult, const EXCEPINFO *pexcepinfo)
	{
		return S_OK;
	}

    STDMETHODIMP OnStateChange(SCRIPTSTATE ssScriptState)
	{
		return S_OK;
	}

    STDMETHODIMP OnScriptError(IActiveScriptError *pscripterror)
	{
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
		return S_OK;
	}

private:
	DWORD m_dwRef;
	DWORD m_applicationCookie;

	IActiveScriptPtr m_scriptEngine;
	IProcessDebugManager32Ptr m_processDebugManager;
	IDebugApplication32Ptr m_debugApplication;

	IDebugDocumentHelper32Ptr debugDocumentHelper;
};