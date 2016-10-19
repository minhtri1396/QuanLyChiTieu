#include "stdafx.h"
#include "CFile.h"
#include "CText.h"
#include <Windows.h>

void CFile::SetByteToRead(_In_ HANDLE &_hFile, _In_ __int32 _nbytes)
{
	char pwNumBytes[10]; //the number of bytes
	_itoa(_nbytes, pwNumBytes, 10);

	//Write the number of digit //Ex: _nbytes = 15 => iNumDigit = 2
	__int8 iNumDigit = 0;
	while (_nbytes > 0)
	{
		_nbytes /= 10;
		++iNumDigit;
	}
	char pwNumDigit[2];
	_itoa(iNumDigit, pwNumDigit, 10);
	DWORD dwBytesWritten;
	WriteFile(
		_hFile,           // open file handle
		pwNumDigit,      // start of data to write
		1,				// number of bytes to write
		&dwBytesWritten, // number of bytes that were written
		NULL);            // no overlapped structure

						  //Write _nbytes
	WriteFile(
		_hFile,           // open file handle
		pwNumBytes,      // start of data to write
		iNumDigit,  // number of bytes to write
		&dwBytesWritten, // number of bytes that were written
		NULL);            // no overlapped structure
}

__int32 CFile::GetByteToRead(_In_ HANDLE &_hFile)
{
	DWORD dwBytesRead;
	char numOfDigit[2] = { 0 };
	char *lpwLen;

	ReadFile(_hFile, numOfDigit, 1, &dwBytesRead, NULL); //number of digit of lpwLen

	lpwLen = new char[atoi(numOfDigit) + 1]; //length of buffer need to create
	ReadFile(_hFile, lpwLen, atoi(numOfDigit), &dwBytesRead, NULL);
	lpwLen[atoi(numOfDigit)] = 0;

	__int32 dwBytesToRead = atoi(lpwLen);
	delete[]lpwLen;

	return dwBytesToRead;
}

void CFile::SaveToFile(std::vector<CExpense*> _vec, wchar_t *fileName)
{
	HANDLE hFile = CreateFile(fileName,                // name of the write
		FILE_GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_ALWAYS,             // create new file only
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (hFile != INVALID_HANDLE_VALUE)
	{
		wchar_t *wszEncoding;
		char *wcsStrSave;
		DWORD dwBytesToWrite;
		DWORD dwBytesWritten;
		BOOL bErrorFlag;
		for each (CExpense* expense in _vec)
		{
			wszEncoding = CText::CExpenseToWString(*expense);
			wcsStrSave = CText::EncryptWCHAR(wszEncoding);
			SetByteToRead(hFile, strlen(wcsStrSave));

			dwBytesToWrite = (DWORD)strlen(wcsStrSave);
			dwBytesWritten = 0;

			bErrorFlag = WriteFile(
				hFile,           // open file handle
				wcsStrSave,      // start of data to write
				dwBytesToWrite,  // number of bytes to write
				&dwBytesWritten, // number of bytes that were written
				NULL);            // no overlapped structure

			delete[]wszEncoding;
			delete[]wcsStrSave;

			if (FALSE == bErrorFlag)
			{
				return;
			}
		}
		CloseHandle(hFile);
	}
}

void CFile::LoadFromFile(std::vector<CExpense*> &_vec, wchar_t *fileName)
{
	HANDLE hFile;

	hFile = CreateFile(fileName,               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD  dwBytesToRead = 0;
		DWORD  dwBytesRead = 0;
		wchar_t *wszString;
		char *buffer;

		while (TRUE)
		{
			dwBytesToRead = GetByteToRead(hFile);
			if (dwBytesToRead == 0)
			{
				break;
			}

			buffer = new char[dwBytesToRead + 1];
			if (FALSE == ReadFile(hFile, buffer, dwBytesToRead, &dwBytesRead, NULL))
			{
				break;
			}

			buffer[dwBytesRead] = '\0'; // NULL character

			wszString = CText::DecyptToWCHAR(buffer);

			_vec.push_back(new CExpense(CText::WStringToCExpense(wszString)));

			delete[]buffer;
			delete[]wszString;
		} //while

		CloseHandle(hFile);
	} //if
}