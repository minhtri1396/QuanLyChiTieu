#include "stdafx.h"
#include "CText.h"


#define MSG_DOT_ERROR L"Bạn đã đặt sai dấu chấm '.'\
					\n  Một vài trường hợp sai:\n    + 1.2.\n    + 10.5.256\n\
					\n  Một vài trường hợp đúng:\n    + 5.\n    + .5\n    + 10.000.000", L"Lỗi dấu chấm"
#define MSG_CHARACTER_ERROR L"Bạn đã nhập các ký tự không hợp lệ. Đây là các ký tự hợp lệ:\
					\n    + Dot (.)\n\n    + Whitespace\n\n    + 0 -> 9", L"Lỗi ký tự"

LPWSTR CText::GetInfoFrom(_In_ HWND hWnd, _Out_ size_t& uiInfoLength)
{
	uiInfoLength = GetWindowTextLength(hWnd);
	if (uiInfoLength)
	{
		LPWSTR lpwStr = new WCHAR[uiInfoLength + 1];
		GetWindowText(hWnd, lpwStr, uiInfoLength + 1);
		return lpwStr;
	}
	return nullptr;
}

//Clear whitespaces
size_t CText::CleanWString(_Inout_opt_ LPWSTR& lpwStr)
{
	INT k = 0;
	if (lpwStr)
	{
		LPWSTR lpwNewStr = new WCHAR[wcslen(lpwStr) + 1];
		for (INT i = 0; lpwStr[i]; ++i)
		{
			if (lpwStr[i] != 32)
			{
				lpwNewStr[k++] = lpwStr[i];
			}
		}

		lpwNewStr[k] = 0;
		delete[]lpwStr;
		lpwStr = lpwNewStr;

		return wcslen(lpwStr);
	}

	return 0;
}

BOOL CText::IsNumber(_In_opt_ WCHAR* _lpwStr)
{
	__int8 k = 0;
	if (_lpwStr)
	{
		while (_lpwStr[k])
		{
			if (_lpwStr[k] < L'0' || _lpwStr[0] > L'9')
			{
				return FALSE;
			}
			++k;
		}
		return TRUE;
	}

	return FALSE;
}

int CText::CompareWStr(wchar_t* wszStr1, wchar_t* wszStr2)
{
	int resCompare = 0;
	wchar_t* pRun1 = wszStr1;
	wchar_t* pRun2 = wszStr2;

	//Loop until pRun1 or pRun2 meets NULL character ('\0')
	while (!resCompare && *pRun1 && *pRun2)
	{
		if (*pRun1 > *pRun2)
		{
			resCompare = 1;
		}

		if (*pRun1 < *pRun2)
		{
			resCompare = -1;
		}

		++pRun1; ++pRun2;
	}

	if (*pRun1) //string has length greater than the other then it greater
	{
		resCompare = 1;
	}

	if (*pRun2) //string wcStr1 has length less than the other then it less
	{
		resCompare = -1;
	}

	return resCompare;
}

wchar_t* CText::CExpenseToWString(const CExpense& _expense)
{
	wchar_t buffer[1001];
	wchar_t szType[2], *szMoney;
	//Convert type of expense to int
	_itow(_expense.IGetType(), szType, 10);
	//Set type of expense to buffer
	wcscpy(buffer, szType);
	wcscat(buffer, L"|");
	//Convert money of expense to wchar_t*
	szMoney = CText::NumberToWCS(_expense.GetMoney(), 2);
	//Set money of expense to buffer
	wcscat(buffer, szMoney);
	wcscat(buffer, L"|");
	delete[]szMoney;
	//Set content of expense to buffer
	wchar_t *szContent = _expense.GetContent();
	if (szContent)
	{
		int k = wcslen(buffer);
		while (*szContent)
		{
			buffer[k++] = *szContent;
			if (*szContent == '|')
			{
				buffer[k++] = '|';
			}

			++szContent;
		}
		buffer[k] = 0;
		wcscat(buffer, L"|");
	}
	else
	{
		wcscat(buffer, L"|");
	}

	return wcsdup(buffer);
}

CExpense CText::WStringToCExpense(wchar_t *wszStr)
{
	wchar_t *pRun = wszStr;
	wchar_t buffer[1001];
	int k = 0; //running variable
	bool nextValue;

	int valueType = 0; // 0 => Type, 1 => Content, 2 => Money of expense
	CExpenseType type;
	wchar_t *szContent = NULL;
	double money;

	while (*pRun)
	{
		nextValue = false;

		if (*pRun == '|')
		{
			if (valueType != 1 && *(pRun + 1) == '|') //valueType == 1 => Money
			{
				++pRun;
			}
			else
			{
				nextValue = true;
			}
		}

		if (nextValue) //have the next value when meet just one '|' character
		{
			buffer[k] = 0;
			if (valueType == 2) //Content
			{
				szContent = _wcsdup(buffer);
			}
			else if (valueType == 1) //Money
			{
				money = _wtof(buffer);
			}
			else //Type
			{
				type = (CExpenseType)_wtoi(buffer);
			}
			buffer[0] = 0;
			k = 0;
			++valueType;
		}
		else
		{
			buffer[k++] = *pRun;
		}
		++pRun;
	} //while

	if (!szContent)
	{
		szContent = wcsdup(L"");
	}

	CExpense e;
	e.SetInfo(type, szContent, money);

	delete[]szContent;

	return e;
}

int CountDigitOf(long value)
{
	int count = 0;
	while (value)
	{
		++count;
		value /= 10;
	}

	return count;
}

//Convert integer or real number to UNICODE (LPWSTR)
LPWSTR CText::NumberToWCS(_In_ double number, _In_ SHORT nDecimalPlaces)
{
	LPWSTR lpwTemp = new WCHAR[20];
	LPWSTR lpwStr = new WCHAR[20];
	lpwStr[0] = 0;
	if (number < 0) wcscat(lpwStr, L"-");
	//Get integer place (lpwTemp saves integer place of number now)
	double integer = floor(number);
	if (integer != 0) {
		INT numberOfDigit = (INT)log10(integer) + 1;
		lpwTemp[numberOfDigit] = 0;
		WCHAR wcDigit[2];
		while (numberOfDigit > 0)
		{
			INT digit = INT(integer - 10 * floor(integer / 10));
			_itow(digit, wcDigit, 10);
			lpwTemp[--numberOfDigit] = wcDigit[0];
			integer /= 10;
		}
	}
	else
	{
		lpwTemp[0] = '0', lpwTemp[1] = 0;
	}
	wcscat(lpwStr, lpwTemp);
	//Get decimal place (lpwTemp saves decimal place of number now)
	integer = floor(number);
	long decimal = (number - integer) * pow(10, nDecimalPlaces); //get n decimal places
	if (decimal != 0) //it has decimal place
	{
		wcscat(lpwStr, L"."); //decimal point
		int nDigits = CountDigitOf(decimal);
		if (nDigits != nDecimalPlaces)
		{
			for (int i = nDecimalPlaces - nDigits; i > 0; --i)
			{
				wcscat(lpwStr, L"0");
			}
		}
		wcscat(lpwStr, _itow(decimal, lpwTemp, 10));
	}
	delete[]lpwTemp;

	return lpwStr;
}

//Convert number series (char* type getted from EncryptWCHAR function) to wchar*
//Ex: _strCode = 65|66|67 => output: ABC
wchar_t* CText::DecyptToWCHAR(char *_szCode)
{
	wchar_t *wszResult = NULL;
	char szNum[11];

	__int32 lenWStr = strlen(_szCode);

	if (lenWStr > 0)
	{
		wszResult = new wchar_t[lenWStr + 1];
		lenWStr = 0;
		__int32 ncStrNum = 0;
		while (*_szCode)
		{
			if (*_szCode == '|')
			{
				szNum[ncStrNum] = 0;
				ncStrNum = 0;
				wszResult[lenWStr++] = (wchar_t)atoi(szNum);
			}
			else
			{
				szNum[ncStrNum++] = *_szCode;
			}
			++_szCode;
		} //for
		wszResult[lenWStr++] = (wchar_t)atoi(szNum);
		wszResult[lenWStr] = 0;
	} //if
	return wszResult;
}

//convert wchar* to number series (char* type)
//Ex: _wszStr = ABC => output: 65|66|67
char* CText::EncryptWCHAR(wchar_t *_wszStr)
{
	__int32 lenWStr = wcslen(_wszStr);
	char szNum[11];
	char buffer[1001];
	buffer[0] = 0;

	if (lenWStr)
	{
		__int16 iNum;
		while (*_wszStr)
		{
			iNum = (__int16)*_wszStr;
			itoa(iNum, szNum, 10);
			if (strlen(buffer))
			{
				strcat(buffer, "|");
				strcat(buffer, szNum);
			}
			else
			{
				strcpy(buffer, szNum);
			}
			++_wszStr;
		} //for
	} //if

	return strdup(buffer);
}

_Success_(return == FALSE)
BOOL CText::IsInvalidNumber(_In_opt_ LPWSTR lpwStr)
{
	if (lpwStr == nullptr)
	{
		return TRUE;
	}

	BOOL fValidSyntax = TRUE;
	SHORT countDigit = 0; //the number of digit after dot.
	SHORT countDot = 0;
	for (INT i = wcslen(lpwStr) - 1; i >= 0; --i)
	{
		if (lpwStr[i] == '.') //checking if it's real or integer number
		{
			++countDot;
			if (countDigit != 3)
				fValidSyntax = FALSE; //maybe this number is a real number, so the first wrong we ignore
			if (countDot > 1 && !fValidSyntax)
			{
				MessageBox(0, MSG_DOT_ERROR, 0);
				return TRUE;
			}
			countDigit = 0;
		}
		else if (lpwStr[i] != 32 && ((lpwStr[i] - 48) > 9 || (lpwStr[i] - 48) < 0)) //whether string contains invalid character
		{
			MessageBox(0, MSG_CHARACTER_ERROR, 0);
			return TRUE;
		}
		else if (lpwStr[i] != 32) ++countDigit;
	}

	return FALSE;
}

//Before running this function, you should run IsInvalid funtion
//Clear whitespaces and dots to calculate conveniently. Ex: 10.000.000 => 10000000
size_t CText::Standardize(_Inout_opt_ LPWSTR& lpwStr)
{
	SHORT countDot = 0, i = -1;
	INT oldLen = wcslen(lpwStr);
	LPWSTR lpwNewStr = new WCHAR[oldLen + 1];
	//Clear dots if it's not the decimal point
	while (lpwStr[++i])
	{
		if (lpwStr[i] == '.')
		{
			++countDot;
			if (countDot == 1) lpwNewStr[i] = lpwStr[i]; //suppose that this Dot is decimal point
		}
		else lpwNewStr[i - (countDot > 1 ? countDot - 1 : 0)] = lpwStr[i]; //countDot - 1 <=> eliminate decimal point
	}
	lpwNewStr[i - (countDot > 1 ? countDot - 1 : 0)] = 0;

	delete[]lpwStr;
	if (countDot > 1) //it's not a decimal point
	{
		lpwStr = new WCHAR[oldLen - countDot + 1];
		int j = 0;
		for (int i = 0; lpwNewStr[i]; ++i)
		{
			if (lpwNewStr[i] != '.') lpwStr[j++] = lpwNewStr[i];
		}
		lpwStr[j] = 0;
	}
	else lpwStr = _wcsdup(lpwNewStr);
	delete[]lpwNewStr;
	//Clear whitespaces
	CleanWString(lpwStr);

	return wcslen(lpwStr);
}

//Before running this function, you should run Standardize function
//Ex: 10000 => 10.000
size_t CText::AddDots(_Inout_opt_ LPWSTR& lpwStr)
{
	SHORT numberOfDots = wcslen(lpwStr) / 3; //the number of dot need to add in number
	if (numberOfDots * 3 == wcslen(lpwStr)) --numberOfDots; //Ex: 500 => numberOfDots = 0, 50 000 => numberOfDots = 1

	SHORT countDigit = 0;
	INT k = numberOfDots + wcslen(lpwStr) - 1;
	LPWSTR lpwNewStr = new WCHAR[k + 2];
	for (INT i = wcslen(lpwStr) - 1; i >= 0; --i)
	{
		if (lpwStr[i] == '.') //this number is a real number
		{
			delete[]lpwNewStr;
			return wcslen(lpwStr);
		}

		++countDigit;
		lpwNewStr[k--] = lpwStr[i];
		if (countDigit == 3 && i > 0)
		{
			countDigit = 0;
			lpwNewStr[k--] = ',';
		}
	}
	lpwNewStr[numberOfDots + wcslen(lpwStr)] = 0;

	delete[]lpwStr;
	lpwStr = lpwNewStr;

	return wcslen(lpwStr);
}

//Clean end white spaces
size_t CText::CleanEndWhiteSpaceWString(_Inout_opt_ LPWSTR& lpwStr)
{
	if (!lpwStr || wcslen(lpwStr) == 0)
	{
		return 0;
	}
	LPWSTR lpwEndStr = lpwStr + wcslen(lpwStr) - 1;
	while ((lpwEndStr != lpwStr) && (*lpwEndStr == 32))
	{
		--lpwEndStr;
	}
	*(lpwEndStr + 1) = '\0';

	return wcslen(lpwStr);
}