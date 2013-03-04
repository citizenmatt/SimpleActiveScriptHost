#pragma once

using namespace System;
using namespace System::Reflection;

namespace CitizenMatt {
	namespace SimpleActiveScriptHost {

		public ref class JsObject
		{
		public:
			JsObject(Object^ jsObject)
			{
				this->jsObject = static_cast<IReflect^>(jsObject);
			}

			generic <class T>
			T GetValue(String ^name)
			{
				return static_cast<T>(Invoke(name, BindingFlags::GetProperty));
			}

		private:
			Object^ Invoke(String ^name, BindingFlags bindingFlags)
			{
				return jsObject->InvokeMember(name, bindingFlags, nullptr, jsObject, nullptr, nullptr, nullptr, nullptr);
			}

			IReflect^ jsObject;
		};
	}
}
