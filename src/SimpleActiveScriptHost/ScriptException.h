#pragma once

using namespace System;

namespace CitizenMatt {
	namespace SimpleActiveScriptHost {
		
		public ref class ScriptException : public Exception
		{
		public:
			ScriptException(String ^message, Exception ^innerException)
				: Exception(message, innerException)
			{
			}

			property String ^ScriptName;
			property Int32 LineNumber;
			property Int32 CharacterPosition;
			property String ^LineText;
			property String ^Description;
		};

		public ref class UnknownScriptLanguageException : public Exception
		{
		public:
			UnknownScriptLanguageException(String ^message, Exception ^innerException)
				: Exception(message, innerException)
			{
			}
		};
	}
}