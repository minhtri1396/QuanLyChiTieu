#include "CExpense.h"
#include <vector>

#ifndef CTEXT
#define CTEXT

class CText
{
public:
	static LPWSTR GetInfoFrom(_In_ HWND hWnd, _Out_ size_t& uiInfoLength);
	//Clean all white space
	static size_t CleanWString(_Inout_opt_ LPWSTR& lpwStr);
	//Clean end white spaces
	static size_t CleanEndWhiteSpaceWString(_Inout_opt_ LPWSTR& lpwStr);
	static BOOL IsNumber(_In_opt_ WCHAR* _lpwStr);
	static LPWSTR NumberToWCS(_In_ double number, _In_ SHORT nDecimalPlaces);
	static int CompareWStr(wchar_t* wszStr1, wchar_t* wszStr2);
	static wchar_t* CExpenseToWString(const CExpense& _expense);
	static CExpense WStringToCExpense(wchar_t *wszStr);

	//Convert number series (char* type getted from EncryptWCHAR function) to wchar*
	//Ex: _strCode = 65|66|67 => output: ABC
	static wchar_t* DecyptToWCHAR(char *_szCode);
	//convert wchar* to number series (char* type)
	//Ex: _wszStr = ABC => output: 65|66|67
	static char *EncryptWCHAR(wchar_t *_szStr);

	_Success_(return == FALSE)
		static BOOL IsInvalidNumber(_In_opt_ LPWSTR lpwStr);
	//Before running this function, you should run IsInvalid funtion
	//Clear whitespaces and dots to calculate conveniently. Ex: 10.000.000 => 10000000
	static size_t Standardize(_Inout_opt_ LPWSTR& lpwStr);
	//Before running this function, you should run Standardize function
	//Ex: 10000 => 10.000
	static size_t AddDots(_Inout_opt_ LPWSTR& lpwStr);
};

#endif // !CTEXT
