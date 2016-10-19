#include "stdafx.h"
#include "CInfoCard.h"
#include "CText.h"
#include "Resource.h"
#include <windowsx.h>
#include <CommCtrl.h>

#define IDC_BTN_DONE			111

static CInfoCard* CInfoCard_gInfoCard;

LRESULT CALLBACK CInfoCard_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//WaitForSingleObject(CCard_hwndSemGCard, INFINITE);
	if (CInfoCard_gInfoCard != NULL)
	{
		//ReleaseSemaphore(CCard_hwndSemGCard, 1, NULL);
		return CInfoCard_gInfoCard->WndProc(hWnd, message, wParam, lParam);
	}
	//ReleaseSemaphore(CCard_hwndSemGCard, 1, NULL);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CInfoCard_MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CInfoCard_WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MANAGEMONEY));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = L"Information Card";
	wcex.lpszClassName = L"informationcard";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);
}

LRESULT CALLBACK CInfoCard::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CInfoCard::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDC_BTN_DONE:
	{
		size_t size;
		//Content
		wchar_t *szContent = CText::GetInfoFrom(m_hwndStaticContent, size);
		//Money
		wchar_t *szMoney = CText::GetInfoFrom(m_hwndStaticMoney, size);
		size = CText::CleanWString(szMoney);
		if (size == 0)
		{
			/*szMoney = wcsdup(L"0");*/
			MessageBox(0, L"Bạn chưa nhập số tiền chi tiêu", L"Chưa nhập tiền", 0);
			delete[]szContent; delete[]szMoney;
			break;
		}
		//Type
		wchar_t *szType = CText::GetInfoFrom(m_hwndStaticType, size);
		CExpenseType type;
		for (int i = 0; i < CExpense_NType; ++i)
		{
			if (wcscmp(szType, CExpense_Types[i]) == 0)
			{
				type = (CExpenseType)i;
			}
		}
		delete[]szType;

		if (m_expenseID == -1)
		{
			if (!CText::IsInvalidNumber(szMoney))
			{
				CText::Standardize(szMoney);

				CExpense expense;
				expense.SetInfo(type, szContent, _wtof(szMoney));
				delete[]szContent; delete[]szMoney;
				//Add expense to expenseManager
				m_expenseID = m_expenseManager->Add(expense);
				m_informComplete(m_expenseID);
				//Reset
				SetWindowText(m_hwndStaticType, L"");
				SetWindowText(m_hwndStaticContent, L"");
				SetWindowText(m_hwndStaticMoney, L"");
			}
			else
			{
				break;
			}
		}
		else
		{
			if (!CText::IsInvalidNumber(szMoney))
			{
				CText::Standardize(szMoney);

				CExpense expense = m_expenseManager->GetExpense(m_expenseID);
				expense.SetInfo(type, szContent, _wtof(szMoney));
				delete[]szContent; delete[]szMoney;
				m_expenseManager->Replace(m_expenseID, expense);
				m_informComplete(m_expenseID);
				//Reset
				SetWindowText(m_hwndStaticType, L"");
				SetWindowText(m_hwndStaticContent, L"");
				SetWindowText(m_hwndStaticMoney, L"");
			}
			else
			{
				break;
			}
		}

		SendMessage(hwnd, WM_CLOSE, 0, 0);

		break;
	} //case IDC_BTN_DONE
	}
}

bool CInfoCard::OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont20 = CreateFont(20, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial"));
	HFONT hFont32 = CreateFont(32, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial"));

	HWND wnd;
	if (m_expenseID == -1)
	{
		m_hwndTitle = CreateWindowEx(0, WC_STATIC, L"THÊM MỚi CHI TIÊU", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 10,
			600, 50, hWnd, NULL, m_hInst, NULL);
	}
	else
	{
		m_hwndTitle = CreateWindowEx(0, WC_STATIC, L"THÂY ĐỔI THÔNG TIN CHI TIÊU", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 10,
			600, 50, hWnd, NULL, m_hInst, NULL);
	}
	SendMessage(m_hwndTitle, WM_SETFONT, (WPARAM)hFont32, TRUE);
	//Type
	wnd = CreateWindowEx(0, WC_STATIC, L"LOẠI CHI TIÊU:", WS_CHILD | WS_VISIBLE | SS_LEFT, 50, 80,
		150, 50, hWnd, NULL, m_hInst, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont20, TRUE);
	m_hwndStaticType = CreateWindowEx(0, WC_COMBOBOX, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL,
		185, 78, 150, 50, hWnd, NULL, m_hInst, NULL);
	SendMessage(m_hwndStaticType, WM_SETFONT, (WPARAM)hFont20, TRUE);
	for (int i = 0; i < CExpense_NType; ++i)
	{
		SendMessage(m_hwndStaticType, CB_ADDSTRING, NULL, (LPARAM)CExpense_Types[i]);
	}
	SendMessage(m_hwndStaticType, CB_SETCURSEL, 0, 0);
	//Money
	wnd = CreateWindowEx(0, WC_STATIC, L"SỐ TIỀN:", WS_CHILD | WS_VISIBLE | SS_LEFT, 50, 127,
		150, 50, hWnd, NULL, m_hInst, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont20, TRUE);
	m_hwndStaticMoney = CreateWindowEx(0, WC_EDIT, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT, 185, 125,
		140, 25, hWnd, NULL, m_hInst, NULL);
	SendMessage(m_hwndStaticMoney, WM_SETFONT, (WPARAM)hFont20, TRUE);
	wnd = CreateWindowEx(0, WC_STATIC, L" VND", WS_CHILD | WS_VISIBLE | SS_LEFT, 328, 128,
		200, 50, hWnd, NULL, m_hInst, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont20, TRUE);
	//Content
	wnd = CreateWindowEx(0, WC_STATIC, L"NỘI DUNG (Ví dụ: Tiền ăn sáng)", WS_CHILD | WS_VISIBLE | SS_LEFT, 50, 170,
		350, 50, hWnd, NULL, m_hInst, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont20, TRUE);
	m_hwndStaticContent = CreateWindowEx(0, WC_EDIT, L"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER,
		50, 200, 400, 180, hWnd, NULL, m_hInst, NULL);
	SendMessage(m_hwndStaticContent, WM_SETFONT, (WPARAM)hFont20, TRUE);
	//Done button
	wnd = CreateWindowEx(0, WC_BUTTON, L"DONE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 400,
		600, 40, hWnd, (HMENU)IDC_BTN_DONE, m_hInst, NULL);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont20, TRUE);

	return TRUE;
}

void CInfoCard::OnClose(HWND hwnd)
{
	EnableWindow(m_hwndParent, TRUE);
	SetFocus(m_hwndParent);

	if (canQuit)
	{
		ReleaseSemaphore(m_semWaitToQuit, 1, NULL);
	}
	else
	{
		//Reset
		SetWindowText(m_hwndStaticType, L"");
		SetWindowText(m_hwndStaticContent, L"");
		SetWindowText(m_hwndStaticMoney, L"");
		ShowWindow(hwnd, SW_HIDE);
	}
}

CInfoCard::CInfoCard(HWND hwndParent, HINSTANCE hInst, CExpenseManager *expenseManager)
{
	canQuit = false;
	CInfoCard_gInfoCard = this;

	m_expenseManager = expenseManager;
	m_hwndParent = hwndParent;
	m_hInst = hInst;

	m_semWaitToQuit = CreateSemaphore(0, 0, 1, NULL);

	//Create screen
	CInfoCard_MyRegisterClass(m_hInst);
	m_hwndThis = CreateWindowW(L"informationcard", L"Information Card", WS_SYSMENU,
		CW_USEDEFAULT, 0, 600, 500, nullptr, nullptr, m_hInst, nullptr);
	if (m_hwndThis)
	{
		ShowWindow(m_hwndThis, SW_HIDE);
	}
}

void CInfoCard::SetInfo(int expenseID)
{
	CExpense expense;
	if (expenseID != -1) //adjust expense
	{
		expense = m_expenseManager->GetExpense(expenseID);
		m_expenseManager->Replace(expenseID, expense);
	}
	else //add new expense
	{
		//expense->SetInfo();
		m_expenseManager->Add(expense);
	}

}

void CInfoCard::Show(FuncComplete _informComplete, int expenseID)
{
	m_informComplete = _informComplete;
	m_expenseID = expenseID;

	if (expenseID != -1) //adjust info
	{
		SetWindowText(m_hwndTitle, L"THÂY ĐỔI THÔNG TIN CHI TIÊU");
		CExpense expense = m_expenseManager->GetExpense(expenseID);
		SendMessage(m_hwndStaticType, CB_SETCURSEL, expense.IGetType(), 0);
		SetWindowText(m_hwndStaticContent, expense.GetContent());
		SetWindowText(m_hwndStaticMoney, CText::NumberToWCS(expense.GetMoney(), 2));
	}
	else
	{
		SetWindowText(m_hwndTitle, L"THÊM MỚI CHI TIÊU");
	}

	EnableWindow(m_hwndParent, FALSE);
	ShowWindow(m_hwndThis, SW_SHOW);
}

void CInfoCard::Quit()
{
	canQuit = true;
	SendMessage(m_hwndThis, WM_CLOSE, 0, 0);

	WaitForSingleObject(m_semWaitToQuit, INFINITE);
}
