#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

ref class CDebugDocumentsHelper
{
public:
	CDebugDocumentsHelper()
	{
		documents = gcnew Dictionary<UInt32, Data^>();
	}

	void Add(UInt32 sourceContext, LPCWSTR name, IDebugDocumentHelper32 *debugDocumentHelper)
	{
		documents->Add(sourceContext, gcnew Data(name, debugDocumentHelper));
	}

	BSTR GetDocumentName(UInt32 sourceContext)
	{
		Data ^data;
		if (documents->TryGetValue(sourceContext, data))
		{
			pin_ptr<const WCHAR> name = PtrToStringChars(data->Name);
			return ::SysAllocString(name);
		}
		return nullptr;
	}

	IDebugDocumentHelper32 *GetDocumentHelper(UInt32 sourceContext)
	{
		Data ^data;
		if (documents->TryGetValue(sourceContext, data))
		{
			return static_cast<IDebugDocumentHelper32*>(Marshal::GetIUnknownForObject(data->DebugDocumentHelper).ToPointer());
		}
		return nullptr;
	}

private:
	ref class Data
	{
	public:
		Data(LPCWSTR name, IDebugDocumentHelper32 *debugDocumentHelper)
		{
			Name = gcnew String(name);
			DebugDocumentHelper = Marshal::GetObjectForIUnknown((IntPtr)debugDocumentHelper);
		}

		String ^Name;
		Object ^DebugDocumentHelper;
	};

	IDictionary<UInt32, Data^> ^documents;
};