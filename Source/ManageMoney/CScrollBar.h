#include "CExpenseManager.h"
#include <Windows.h>

#ifndef CSCROLLBAR
#define CSCROLLBAR

class CScrollBar
{
public:
	static const int MAX_ITEM = 9;

private:
	SCROLLINFO siBackup;
	int posBackup;

	HWND m_hwndParent;
	CExpenseManager* m_expenseManager;
	HANDLE hwndSemBackup;

	//This function called by thread
	HANDLE hwndSemCheckShouldShow;
	HANDLE hwndSemDestructor; //use to destructor wait until CheckShouldShow function has done
	DWORD CheckShouldShow();
	static DWORD WINAPI StaticThreadStart(void* Param);
public:
	CScrollBar(HWND hwndParent, CExpenseManager* expenseManager);
	~CScrollBar();

	void ResetZero();
	void ResetScrollBar();
	void BackupScrollBar();
	void RestoreScrollBar();
};

#endif