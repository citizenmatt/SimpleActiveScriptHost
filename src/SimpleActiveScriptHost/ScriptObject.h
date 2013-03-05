#pragma once

using namespace System;
using namespace System::Reflection;

namespace CitizenMatt {
	namespace SimpleActiveScriptHost {

		public ref class ScriptObject
		{
		public:
			ScriptObject(Object^ scriptObject)
			{
				this->scriptObject = static_cast<IReflect^>(scriptObject);
			}

			generic <class T>
			T GetValue(String ^name)
			{
				return static_cast<T>(Invoke(name, System::Reflection::BindingFlags::GetProperty));
			}

			property ScriptObject^ default[Object^]
			{
				ScriptObject^ get(Object ^key) { return gcnew ScriptObject(GetValue<Object^>(key->ToString())); }
			}

			ScriptObject^ GetObject(String ^name)
			{
				return gcnew ScriptObject(GetValue<Object^>(name));
			}

		private:
			Object^ Invoke(String ^name, System::Reflection::BindingFlags bindingFlags)
			{
				return scriptObject->InvokeMember(name, bindingFlags, nullptr, scriptObject, nullptr, nullptr, nullptr, nullptr);
			}

			IReflect^ scriptObject;
		};
	}
}
