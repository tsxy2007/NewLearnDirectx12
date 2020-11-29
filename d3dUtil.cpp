#include "d3dUtil.h"
#include <fstream>
#include <comdef.h>

using Microsoft::WRL::ComPtr;

DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber):
	ErrorCode(hr),
	FunctionName(functionName),
	FileName(filename),
	LineNumber(lineNumber)
{

}
