#pragma once

class ScriptErrorInfo
{
public:
	ScriptErrorInfo()
	{
		LineNumber = 0;
		CharacterPosition = 0;
		memset(&ExcepInfo, 0, sizeof(::EXCEPINFO));
	}

	_bstr_t ScriptName;
	ULONG LineNumber;
	LONG CharacterPosition;
	_bstr_t LineText;
	::EXCEPINFO ExcepInfo;
};