#include <Windows.h>
#include "CExpenseManager.h"
#include "CMessage.h"

#ifndef CCARD
#define CCARD

#define CARD_MAX_WIDTH 280 //480
#define CARD_MAX_HEIGHT 160 //200

typedef CExpenseType(*FuncGetType)(int);

class CCard
{
private:
	int m_id;
	int m_expenseID;
	CExpenseType m_expenseType;
	MessageInteger* m_sendID;

	DWORD m_dwX;
	DWORD m_dwY;

	HINSTANCE m_hInst;
	HWND m_hwndParent;

	HWND m_hwndCard;
	HWND m_hwndBack1, m_hwndBack2;
	HWND m_hwndStaticType;
	HWND m_hwndStaticContent;
	HWND m_hwndStaticMoney;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
	VOID OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
	HBRUSH CCard::OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type);

	friend LRESULT CALLBACK CCard_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CCard(HWND _hwndParent, HINSTANCE _hInst, DWORD _dwX, DWORD _dwY, MessageInteger* _sendID);
	~CCard();
	void InitCard(CExpenseType _expenseType);
	int GetId();
	HWND GetHWND();
	void SetX(DWORD _dwX);
	void SetY(DWORD _dwY);
	DWORD GetX();
	DWORD GetY();
	void DrawInfo(int _expenseID, CExpenseType _type, wchar_t *_content, double _money);
	//Return id of expense which this object is showing information
	int Remove(CCard* lastCard);
	//Return id of expense which this object is showing information
	int GetExpenseID();
};

#endif