// SimpleActiveScriptHost.h

#pragma once

using namespace System;

namespace CitizenMatt {
	namespace SimpleActiveScriptHost {

		public ref class ScriptHost
		{
		public:
			ScriptHost(String ^language, String ^application)
			{
			}

			~ScriptHost()
			{
			}

			void Parse(String ^code, String ^name)
			{
			}
		};
	}
}
