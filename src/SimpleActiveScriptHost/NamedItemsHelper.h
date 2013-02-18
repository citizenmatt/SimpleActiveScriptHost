#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

ref class CNamedItemsHelper
{
public:
	CNamedItemsHelper()
	{
		namedItems = gcnew Dictionary<String ^, Object ^>();
	}

	~CNamedItemsHelper()
	{
		for each (Object ^item in namedItems->Values)
		{
			IDisposable ^disposable = dynamic_cast<IDisposable^>(item);
			if (disposable != nullptr)
				delete disposable;
		}
	}

	void AddNamedItem(String ^name, Func<Object ^> ^creator)
	{
		namedItems->Add(name, creator);
	}

	HRESULT GetItemInfo(LPCWSTR name, DWORD returnMask, IUnknown **pUnk, ITypeInfo **ppti)
	{
		String ^itemName = gcnew String(name);

		Object ^value;
		if (!namedItems->TryGetValue(itemName, value))
			return TYPE_E_ELEMENTNOTFOUND;

		Func<Object^> ^creator = dynamic_cast<Func<Object^>^>(value);
		if (creator != nullptr)
		{
			value = creator();
			namedItems[itemName] = value;
		}

		if ((returnMask & SCRIPTINFO_ITYPEINFO) != 0)
		{
			if (ppti == nullptr)
				return E_POINTER;
			*ppti = static_cast<ITypeInfo*>(Marshal::GetITypeInfoForType(value->GetType()).ToPointer());
		}

		if ((returnMask & SCRIPTINFO_IUNKNOWN) != 0)
		{
			if (pUnk == nullptr)
				return E_POINTER;
			*pUnk = static_cast<IUnknown*>(Marshal::GetIUnknownForObject(value).ToPointer());
		}

		return S_OK;
	}

private:
	IDictionary<String ^, Object ^> ^namedItems;
};