#include "CExpense.h"
#include <vector>

#ifndef CFILE
#define CFILE

static wchar_t FileNameSystem[] = L"info.txt";

class CFile
{
private:
	static __int32 GetByteToRead(_In_ HANDLE &_hFile);
	static void SetByteToRead(_In_ HANDLE &_hFile, _In_ __int32 _nbytes);
public:
	static void SaveToFile(std::vector<CExpense*> _vec, wchar_t *fileName);
	static void LoadFromFile(std::vector<CExpense*> &_vec, wchar_t *fileName);
};

#endif // !CFILE
