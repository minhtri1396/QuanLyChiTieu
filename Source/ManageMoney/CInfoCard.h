#include "CExpenseManager.h"
#include <Windows.h>

#ifndef CINFOCARD
#define CINFOCARD

typedef void(*FuncComplete)(int);

//Just declare one CInfoCard object in the time cause CInfoCard_gInfoCard variable in CInfoCard.cpp
class CInfoCard
{
private:
	bool canQuit;
	HWND m_hwndThis;

	HINSTANCE m_hInst;
	HWND m_hwndParent;
	int m_expenseID;

	HWND m_hwndTitle;
	HWND m_hwndStaticType;
	HWND m_hwndStaticContent;
	HWND m_hwndStaticMoney;

	HANDLE m_semWaitToQuit;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	bool OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
	void OnClose(HWND hwnd);

	friend LRESULT CALLBACK CInfoCard_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	CExpenseManager *m_expenseManager;

	FuncComplete m_informComplete;
	void SetInfo(int expenseID);

public:
	CInfoCard(HWND hwndParent, HINSTANCE hInst, CExpenseManager *expenseManager);
	//Return new expense ID
	void Show(FuncComplete _informComplete, int expenseID = -1);
	void Quit();
};

#endif // !CINFOCARD
