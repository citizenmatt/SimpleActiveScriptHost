#pragma once

#include "ScriptSite.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace CitizenMatt {
	namespace SimpleActiveScriptHost {

		public ref class ScriptHost
		{
		public:
			ScriptHost(String ^language, String ^application)
			{
				IActiveScriptPtr engine = CreateActiveScriptEngine(language);
				CreateSite(engine, application);
			}

			~ScriptHost()
			{
				if (m_pSite != nullptr)
				{
					m_pSite->Close();
					ULONG l = m_pSite->Release();
				}
			}

			void Parse(String ^code, String ^name)
			{
				pin_ptr<const WCHAR> scriptText = PtrToStringChars(code);
				pin_ptr<const WCHAR> scriptName = PtrToStringChars(name);
				m_pSite->ParseScriptText(scriptText, scriptName);
			}

			Object^ CallMethod(String ^name)
			{
				pin_ptr<const WCHAR> methodName = PtrToStringChars(name);

				_variant_t vt;
				vt.Clear();
				m_pSite->CallMethod(methodName, &vt);

				return Marshal::GetObjectForNativeVariant((IntPtr)&vt);
			}

			void AddNamedItem(String ^name, Func<Object^> ^creator)
			{
			}

		private:
			CScriptSite *m_pSite;

			IActiveScript *CreateActiveScriptEngine(String ^language)
			{
				pin_ptr<const OLECHAR> progid = PtrToStringChars(language);

				IActiveScriptPtr activeScript(progid);
				return activeScript.Detach();
			}

			void CreateSite(IActiveScript *engine, String ^application)
			{
				m_pSite = new CScriptSite();
				m_pSite->AddRef();

				pin_ptr<const WCHAR> applicationName = PtrToStringChars(application);
				m_pSite->CreateApplication(applicationName);

				IActiveScriptSitePtr pSite(m_pSite);
				engine->SetScriptSite(pSite);

				m_pSite->SetEngine(engine);
			}
		};

		public ref class ScriptException : public Exception
		{
		public:
			property String ^ScriptName;
			property Int32 LineNumber;
			property Int32 CharacterPosition;
			property String ^LineText;
			property String ^Description;
		};
	}
}
