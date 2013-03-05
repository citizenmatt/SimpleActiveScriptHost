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
				HRESULT hr = m_pSite->ParseScriptText(scriptText, scriptName);
				if (FAILED(hr))
					throw GetScriptException(hr);
			}

			Object^ CallMethod(String ^name, ... array<Object^>^ params)
			{
				pin_ptr<const WCHAR> methodName = PtrToStringChars(name);

				VARIANTARG *args = nullptr;
				if (params->Length > 0)
				{
					args = new VARIANTARG[params->Length];
					for (int i = 0; i < params->Length; i++)
					{
						IntPtr ip = (IntPtr)&args[params->Length - i - 1];
						Marshal::GetNativeVariantForObject(params[i], ip);
					}
				}

				_variant_t vt;
				vt.Clear();
				HRESULT hr = m_pSite->CallMethod(methodName, params->Length, args, &vt);

				if (args != nullptr)
				{
					for (int i = 0; i < params->Length; i ++)
						::VariantClear(&args[i]);
					delete [] args;
				}

				if (FAILED(hr))
					throw GetScriptException(hr);

				return Marshal::GetObjectForNativeVariant((IntPtr)&vt);
			}

			void AddNamedItem(String ^name, Func<Object^> ^creator)
			{
				m_pSite->namedItems->AddNamedItem(name, creator);

				pin_ptr<const WCHAR> itemName = PtrToStringChars(name);
				m_pSite->AddNamedItem(itemName);
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

			Exception ^GetScriptException(HRESULT hr)
			{
				ScriptErrorInfo &error = m_pSite->ScriptErrorInfo;

				String ^source = GetStringOrNull(error.ExcepInfo.bstrSource);
				String ^description = GetStringOrNull(error.ExcepInfo.bstrDescription);
				String ^lineText = GetStringOrNull(error.LineText);

				String ^message = String::Format(L"{0}. {1} at line {2}, column {3}", source, description, error.LineNumber.ToString(), error.CharacterPosition.ToString());
				if (!String::IsNullOrEmpty(lineText))
                    message += String::Format(L". Source text: '{0}'", lineText);

				ScriptException ^exception = gcnew ScriptException(message, Marshal::GetExceptionForHR(hr));
				exception->Source = source;
				exception->Description = description;
				exception->ScriptName = GetStringOrNull(error.ScriptName);
				exception->LineNumber = error.LineNumber;
				exception->CharacterPosition = error.CharacterPosition;
				exception->LineText = lineText;
				return exception;
			}

			String ^GetStringOrNull(BSTR bstr)
			{
				if (bstr == nullptr || ::SysStringLen(bstr) == 0)
					return nullptr;
				return gcnew String(bstr);
			}
		};
	}
}
