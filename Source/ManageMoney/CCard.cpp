#include "stdafx.h"
#include "CCard.h"
#include "CBitMap.h"
#include "CText.h"
#include "CCardManager.h"

#include <thread>
#include <windowsx.h>

#include <commctrl.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")

#define IDC_BACK_CARD			0
#define IDC_BTN_DEL				IDC_BACK_CARD + CExpense::MAX_NEXPENSE //should be MAX value

typedef void (CCardManager:: *NotifyRemoveFunc)(int);

static CCard* CCard_gCard; //try to keep this variable being always point to the last card in CCardManager
static const HWND CCard_hwndSemGCard = (const HWND)CreateSemaphore(NULL, 1, 1, NULL); //semaphore of CCard_gCard
static CBitMap CCard_gBitMap(CCardManager::MAX_NCARD);

CExpenseType CCard_gExpenseIDs[CCardManager::MAX_NCARD];

LRESULT CALLBACK CCard_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WaitForSingleObject(CCard_hwndSemGCard, INFINITE);
	if (CCard_gCard != NULL)
	{
		ReleaseSemaphore(CCard_hwndSemGCard, 1, NULL);
		return CCard_gCard->WndProc(hWnd, message, wParam, lParam);
	}
	ReleaseSemaphore(CCard_hwndSemGCard, 1, NULL);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CCard_MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CCard_WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MANAGEMONEY));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = L"Control Card";
	wcex.lpszClassName = L"card";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);
}

LRESULT CALLBACK CCard::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//MessageBox(0, 0, 0, 0);
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_CTLCOLORSTATIC, OnCtlColor);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

VOID CCard::OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	default:
		if (id >= CExpense::MAX_NEXPENSE && id < 2 * CExpense::MAX_NEXPENSE) //IDC_BTN_DEL
		{
			LRESULT result = MessageBox(0, L"Bạn có chắc chắn muốn xóa thẻ này?", L"Xóa thẻ", MB_YESNO);
			if (result == (IDYES))
			{
				id -= IDC_BTN_DEL; //cause this id has been assign IDC_BTN_DEL + CCard_gBitMap.Find()
				(*m_sendID)(id);
			}
		}
		else if (id >= IDC_BACK_CARD && id < CExpense::MAX_NEXPENSE) //IDC_BACK_CARD
		{
			id -= IDC_BACK_CARD; //cause this id has been assign IDC_BTN_DEL + CCard_gBitMap.Find()
			id *= -1;
			--id;
			(*m_sendID)(id);
		}
	} //default
}

BOOL CCard::OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	// Get system font
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont20 = CreateFont(20, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial"));
	HFONT hFont28 = CreateFont(28, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial"));
	HFONT hFont30 = CreateFont(30, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial"));

	//SetCursor(LoadCursor(m_hInst, MAKEINTRESOURCE(IDC_WAIT)));

	//Create backgroud for card
	m_hwndBack1 = CreateWindowEx(0, WC_STATIC, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, 30, 0,
		CARD_MAX_WIDTH - 25, CARD_MAX_HEIGHT, hWnd, (HMENU)(IDC_BACK_CARD + m_id), m_hInst, 0);
	m_hwndBack2 = CreateWindowEx(0, WC_STATIC, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, 0, 28,
		CARD_MAX_WIDTH, CARD_MAX_HEIGHT - 25, hWnd, (HMENU)(IDC_BACK_CARD + m_id), m_hInst, 0);

	m_hwndStaticType = CreateWindowEx(0, WC_STATIC, L"TYPE", WS_CHILD | WS_VISIBLE, 50, 5,
		CARD_MAX_WIDTH, CARD_MAX_HEIGHT, hWnd, (HMENU)(IDC_BACK_CARD + m_id), m_hInst, 0);
	SendMessage(m_hwndStaticType, WM_SETFONT, (WPARAM)hFont20, TRUE);

	m_hwndStaticContent = CreateWindowEx(0, WC_STATIC, L"CONTENT", WS_CHILD | WS_VISIBLE, 10, 35,
		CARD_MAX_WIDTH, CARD_MAX_HEIGHT, hWnd, (HMENU)(IDC_BACK_CARD + m_id), m_hInst, 0);
	SendMessage(m_hwndStaticContent, WM_SETFONT, (WPARAM)hFont28, TRUE);

	m_hwndStaticMoney = CreateWindowEx(0, WC_STATIC, L"4.000.000.000", WS_CHILD | WS_VISIBLE | SS_RIGHT, -10, 125,
		280, 45, hWnd, (HMENU)(IDC_BACK_CARD + m_id), m_hInst, 0);
	SendMessage(m_hwndStaticMoney, WM_SETFONT, (WPARAM)hFont30, TRUE);

	//Create X button
	CreateWindowEx(0, WC_STATIC, L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTER, 0, 0,
		30, 28, hWnd, (HMENU)(IDC_BTN_DEL + m_id), m_hInst, nullptr);
	HWND hwndXButton = CreateWindowEx(0, WC_STATIC, L"X", WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTER, 2, 4,
		25, 22, hWnd, (HMENU)(IDC_BTN_DEL + m_id), m_hInst, nullptr);
	SendMessage(hwndXButton, WM_SETFONT, (WPARAM)hFont20, TRUE);

	return TRUE;
}

HBRUSH CCard::OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	HBRUSH hBrush;
	DWORD CtrlID = GetDlgCtrlID(hwndChild); //Window Control ID

	switch (CtrlID)
	{
	default:
	{
		if (CtrlID >= CExpense::MAX_NEXPENSE && CtrlID < 2 * CExpense::MAX_NEXPENSE) //IDC_BTN_DEL
		{
			hBrush = CreateSolidBrush(RGB(0, 0, 0));
			SetTextColor(hdc, RGB(255, 255, 255));
			SetBkColor(hdc, RGB(0, 0, 0));
			return hBrush;
		}
		else if (CtrlID >= IDC_BACK_CARD && CtrlID < CExpense::MAX_NEXPENSE) //IDC_BACK_CARD
		{
			switch (CCard_gExpenseIDs[CtrlID - IDC_BACK_CARD])
			{
			case CExpenseType::CEXPENSE_EATING:
				hBrush = CreateSolidBrush(backEating);
				SetTextColor(hdc, RGB(255, 255, 255));
				SetBkColor(hdc, backEating);
				return hBrush;
			case CExpenseType::CEXPENSE_TRANSPORT:
				hBrush = CreateSolidBrush(backTransport);
				SetTextColor(hdc, RGB(255, 255, 255));
				SetBkColor(hdc, backTransport);
				return hBrush;
			case CExpenseType::CEXPENSE_HOME:
				hBrush = CreateSolidBrush(backHome);
				SetTextColor(hdc, RGB(79, 79, 79));
				SetBkColor(hdc, backHome);
				return hBrush;
			case CExpenseType::CEXPENSE_VEHICLE:
				hBrush = CreateSolidBrush(backVehicle);
				SetTextColor(hdc, RGB(130, 130, 130));
				SetBkColor(hdc, backVehicle);
				return hBrush;
			case CExpenseType::CEXPENSE_NECESSARY:
				hBrush = CreateSolidBrush(backNecessary);
				SetTextColor(hdc, RGB(255, 255, 255));
				SetBkColor(hdc, backNecessary);
				return hBrush;
			case CExpenseType::CEXPENSE_SERVICE:
				hBrush = CreateSolidBrush(backService);
				SetTextColor(hdc, RGB(255, 255, 255));
				SetBkColor(hdc, backService);
				return hBrush;
			case CExpenseType::CEXPENSE_NONE:
				hBrush = CreateSolidBrush(backNone);
				SetTextColor(hdc, RGB(255, 255, 255));
				SetBkColor(hdc, backNone);
				return hBrush;
			} //switch
		} //if else

		return CreateSolidBrush(COLOR_BTNFACE);
	} //default
	} //switch
}

CCard::CCard(HWND _hwndParent, HINSTANCE _hInst, DWORD _dwX, DWORD _dwY, MessageInteger* _sendID)
{
	m_expenseID = -1;
	m_sendID = _sendID;

	WaitForSingleObject(CCard_hwndSemGCard, INFINITE);
	CCard_gCard = this;
	ReleaseSemaphore(CCard_hwndSemGCard, 1, NULL);

	CCard_MyRegisterClass(_hInst);
	m_hInst = _hInst;
	m_hwndParent = _hwndParent;
	m_dwX = _dwX;
	m_dwY = _dwY;
}

CCard::~CCard()
{
	CCard_gBitMap.Clear(m_id);
}

int CCard::GetId()
{
	return m_id;
}

HWND CCard::GetHWND()
{
	return m_hwndCard;
}

void CCard::SetX(DWORD _dwX)
{
	m_dwX = _dwX;
	MoveWindow(m_hwndCard, m_dwX, m_dwY, CARD_MAX_WIDTH, CARD_MAX_HEIGHT, TRUE);
}

void CCard::SetY(DWORD _dwY)
{
	m_dwY = _dwY;
	MoveWindow(m_hwndCard, m_dwX, m_dwY, CARD_MAX_WIDTH, CARD_MAX_HEIGHT, TRUE);
}

DWORD CCard::GetX()
{
	return m_dwX;
}

DWORD CCard::GetY()
{
	return m_dwY;
}

void CCard::InitCard(CExpenseType _expenseType)
{
	m_expenseType = _expenseType;
	m_id = CCard_gBitMap.Find();
	if (m_id != -1)
	{
		m_hwndCard = CreateWindowW(L"card", L"Control Card", WS_CHILD, m_dwX, m_dwY,
			CARD_MAX_WIDTH, CARD_MAX_HEIGHT, m_hwndParent, nullptr, m_hInst, nullptr);

		ShowWindow(m_hwndCard, SW_SHOW);
	}
}

wchar_t * SetUpWSTR(wchar_t *_wszStr)
{
	wchar_t *wszResult = NULL;

	if (CText::CleanEndWhiteSpaceWString(_wszStr))
	{
		if (wcslen(_wszStr) <= 20)
		{
			int k = 0;
			int countLine = 0;
			int len = wcslen(_wszStr);
			wszResult = new wchar_t[len + 1];
			for (; k < len; ++k)
			{
				if (_wszStr[k] == '\r')
				{
					++countLine;
					if (countLine == 2)
					{
						wszResult[k] = 0;
						wcscat(wszResult, L"\n...");
						break;
					}
				}
				wszResult[k] = _wszStr[k];
			}
			if (k == len)
			{
				wszResult[k] = 0;
			}
		}
		else
		{
			wszResult = new wchar_t[20];
			wszResult[15] = 0;

			for (int i = 0; i < 15; ++i)
			{
				wszResult[i] = _wszStr[i];
			}
			
			wcscat(wszResult, L"....");
		} //if (wcslen(_wszStr) <= 20)
	}
	else
	{
		wszResult = wcsdup(L"");
	} //if (CText::CleanEndWhiteSpaceWString(_wszStr))

	return wszResult;
}

//Not done
void CCard::DrawInfo(int _expenseID, CExpenseType _type, wchar_t *_content, double _money)
{
	CCard_gExpenseIDs[m_id] = _type;
	m_expenseID = _expenseID;

	SetWindowText(m_hwndBack1, L"");

	SetWindowText(m_hwndBack2, L"");

	SetWindowText(m_hwndStaticType, CExpense::ExpenseTypeToWCHAR(_type));

	wchar_t *wszContent = SetUpWSTR(_content);
	SetWindowText(m_hwndStaticContent, wszContent);
	delete[]wszContent;

	wchar_t* szMoney = CText::NumberToWCS(_money, 2);
	CText::AddDots(szMoney);
	SetWindowText(m_hwndStaticMoney, szMoney);
	delete[]szMoney;
}

//Return id of expense which this object is showing information
int CCard::Remove(CCard* lastCard)
{
	SendMessage(m_hwndCard, WM_CLOSE, 0, 0);

	WaitForSingleObject(CCard_hwndSemGCard, INFINITE);
	CCard_gCard = lastCard;
	ReleaseSemaphore(CCard_hwndSemGCard, 1, NULL);

	return m_expenseID;
}

//Return id of expense which this object is showing information
int CCard::GetExpenseID()
{
	return m_expenseID;
}
