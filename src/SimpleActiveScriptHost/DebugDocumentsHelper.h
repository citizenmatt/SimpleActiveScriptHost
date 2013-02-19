#pragma once

#define ALLOCATE_STEP 30

class CDebugDocumentsHelper
{
public:
	CDebugDocumentsHelper()
	{
		documentCount = 0;
		documentAllocated = ALLOCATE_STEP;
		documents = new Data*[documentAllocated];
	}

	~CDebugDocumentsHelper()
	{
		for (int i = 0; i < documentCount; i++)
			delete documents[i];
		delete [] documents;
	}


	void Add(DWORD sourceContext, LPCWSTR name, IDebugDocumentHelper32 *debugDocumentHelper)
	{
		if (documentCount >= documentAllocated)
		{
			documentAllocated += ALLOCATE_STEP;
			Data **docs = documents;
			documents = new Data*[documentAllocated];
			for (int i = 0; i < documentCount; i++)
				documents[i] = docs[i];
			delete[] docs;
		}
		documents[documentCount] = new Data(sourceContext, name, debugDocumentHelper);
		documentCount++;
	}

	// Doesn't add reference!
	BSTR GetDocumentName(DWORD sourceContext)
	{
		for (int i = 0; i < documentCount; i++)
		{
			if (sourceContext == documents[i]->SourceContextCookie)
				return documents[i]->Name;
		}
		return nullptr;
	}

	// Doesn't add reference!
	IDebugDocumentHelper32 *GetDocumentHelper(DWORD sourceContext)
	{
		for (int i = 0; i < documentCount; i++)
		{
			if (sourceContext == documents[i]->SourceContextCookie)
				return documents[i]->DebugDocumentHelper.GetInterfacePtr();
		}
		return nullptr;
	}

private:
	class Data
	{
	public:
		Data(DWORD sourceContextCookie, LPCWSTR name, IDebugDocumentHelper32 *debugDocumentHelper)
		{
			SourceContextCookie = sourceContextCookie;
			Name = name;
			DebugDocumentHelper = debugDocumentHelper;
		}

		DWORD SourceContextCookie;
		_bstr_t Name;
		IDebugDocumentHelper32Ptr DebugDocumentHelper;
	};

	Data **documents;
	int documentCount;
	int documentAllocated;
};