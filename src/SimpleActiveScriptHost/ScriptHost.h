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

			Object^ CallMethod(String ^name)
			{
				return nullptr;
			}

			void AddNamedItem(String ^name, Func<Object^> ^creator)
			{
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
