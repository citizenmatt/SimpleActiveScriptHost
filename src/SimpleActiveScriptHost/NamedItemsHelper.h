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
		// TODO: Dispose all stored objects that implement IDisposable
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
//			*ppti = dynamic_cast<ITypeInfo*>(Marshal::GetITypeInfoForType(value->GetType()));
		}

		IntPtr ip = Marshal::GetIUnknownForObject(value);
		*pUnk = (IUnknown*)ip.ToPointer();

		return S_OK;
	}

private:
	IDictionary<String ^, Object ^> ^namedItems;
};