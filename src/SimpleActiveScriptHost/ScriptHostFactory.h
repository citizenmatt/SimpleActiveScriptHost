#pragma once

namespace CitizenMatt {
	namespace SimpleActiveScriptHost {

		public ref class ScriptHostFactory
		{
		public:
			static ScriptHost ^CreateJavaScriptHost(String ^application)
			{
				try
				{
					return gcnew ScriptHost(ChakraClsid, application);
				}
				catch(UnknownScriptLanguageException^ exception)
				{
					return gcnew ScriptHost("javascript", application);
				}
			}

			static ScriptHost ^CreateVBScriptHost(String ^application)
			{
				return gcnew ScriptHost("vbscript", application);
			}

			static String^ ChakraClsid = "{16d51579-a30b-4c8b-a276-0ff4dc41e755}";
		};
	}
}