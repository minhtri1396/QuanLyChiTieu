// ManageMoney.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Main.h"

#include "CExpenseManager.h"
#include "CCard.h"
#include "CCardManager.h"
#include "CInfoCard.h"
#include "CChart.h"
#include "CText.h"

#include <windowsx.h>
#include <commctrl.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hwndThis;
HWND hwndBtnAdd;
HWND hwndSum;
HWND hwndBtnDelAll;
HWND hwndCharts[7];
CScrollBar *controlVScroll;
CExpenseManager *expenseManager;
CCardManager *cardManager;
CInfoCard *adjustScreen;
CChart *controlChart;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
VOID OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
VOID OnPaint(HWND hWnd);
VOID OnDestroy(HWND hWnd);
HBRUSH OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type);
void OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
void ReceiveNoticeAdd(int expenseID);
void ReceiveNoticeRemove();
void ReceiveNoticeAdjust();
void DrawChart(HWND hWnd);
void SetSum();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MANAGEMONEY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MANAGEMONEY));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MANAGEMONEY));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MANAGEMONEY);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hwndThis = CreateWindowW(szWindowClass, szTitle, WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0, 1189, 700, nullptr, nullptr, hInstance, nullptr);

	if (!hwndThis)
	{
		return FALSE;
	}

	//Create expense manager
	expenseManager = new CExpenseManager();

	//Get expenses before
	expenseManager->Load();

	if (expenseManager->size() == 0)
	{
		EnableWindow(hwndBtnDelAll, FALSE);
	}

	//Create control chart
	controlChart = new CChart(expenseManager);

	//Create scrollbar controlling
	controlVScroll = new CScrollBar(hwndThis, expenseManager);

	adjustScreen = new CInfoCard(hwndThis, hInst, expenseManager);

	//Create card management
	cardManager = new CCardManager(hwndThis, hInst, expenseManager, controlVScroll, adjustScreen,
		ReceiveNoticeRemove, ReceiveNoticeAdjust);

	DrawChart(hwndThis);
	SetSum();

	ShowWindow(hwndThis, nCmdShow);
	UpdateWindow(hwndThis);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_VSCROLL, OnVScroll);
		HANDLE_MSG(hWnd, WM_CTLCOLORSTATIC, OnCtlColor);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	// Get system font
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	HFONT hFont40 = CreateFont(40, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial Bold"));
	HFONT hFont32 = CreateFont(32, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial Bold"));
	HFONT hFont25 = CreateFont(25, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial Bold"));
	HFONT hFont20 = CreateFont(20, lf.lfWidth,
		lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
		lf.lfPitchAndFamily, TEXT("Arial"));

	HWND wnd = CreateWindowEx(0, WC_STATIC, L"", WS_CHILD | WS_VISIBLE, 0, 10,
		1189, 50, hWnd, (HMENU)IDC_BACK_TITLE, hInst, 0);
	wnd = CreateWindowEx(0, WC_STATIC, L"QUẢN LÝ CHI TIÊU", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 25,
		1189, 50, hWnd, (HMENU)IDC_BACK_TITLE, hInst, 0);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont40, TRUE);

	hwndBtnAdd = CreateWindowEx(0, WC_BUTTON, L"Thêm chi tiêu", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 850, 85,
		301, 50, hWnd, (HMENU)IDC_BTN_ADD, hInst, 0);
	SendMessage(hwndBtnAdd, WM_SETFONT, (WPARAM)hFont20, TRUE);

	hwndBtnDelAll = CreateWindowEx(0, WC_BUTTON, L"Xóa tất cả", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 750, 85,
		90, 50, hWnd, (HMENU)IDC_BTN_DELALL, hInst, 0);
	SendMessage(hwndBtnDelAll, WM_SETFONT, (WPARAM)hFont20, TRUE);

	wnd = CreateWindowEx(0, WC_STATIC, L"TỔNG:", WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 135,
		500, 25, hWnd, (HMENU)IDC_BACK_SUM, hInst, 0);
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont25, TRUE);

	hwndSum = CreateWindowEx(0, WC_STATIC, L"50000", WS_CHILD | WS_VISIBLE | SS_LEFT, 95, 130,
		500, 25, hWnd, (HMENU)IDC_BACK_SUM, hInst, 0);
	SendMessage(hwndSum, WM_SETFONT, (WPARAM)hFont32, TRUE);

	//Draw chart
	int division[CExpense_NType] = { 14, 14, 14, 14, 14, 14, 16 };

	int sum = 0;
	for (int i = CExpense_NType - 2; i >= 0; --i)
	{
		division[i] = 14;
		sum += division[i];
	}
	division[CExpense_NType - 1] = 100 - sum;

	int posX = 0;
	for (int i = 0; i < 7; ++i)
	{
		hwndCharts[i] = CreateWindowEx(0, WC_STATIC, L"", WS_CHILD | WS_VISIBLE, posX, 85,
			(700 * division[i]) / 100, 25, hWnd, (HMENU)(IDC_BACK_EATING + i), hInst, 0);

		posX += (700 * division[i]) / 100;
	}

	return TRUE;
}

VOID OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	// Parse the menu selections:
	switch (id)
	{
	case IDC_BTN_ADD:
	{
		adjustScreen->Show(ReceiveNoticeAdd);
		break;
	}

	case IDC_BTN_DELALL:
	{
		LRESULT result = MessageBox(0, L"Bạn có chắc chắn muốn xóa tất cả thẻ?", L"Xóa tất cả thẻ", MB_YESNO);
		if (result == (IDYES))
		{
			EnableWindow(hwndBtnAdd, TRUE);
			EnableWindow(hwndBtnDelAll, FALSE);
			cardManager->RemoveAll();
			DrawChart(hwndThis);
			SetSum();
		}
		break;
	}

	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;

	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	}
}

VOID OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	// TODO: Add any drawing code that uses hdc here...
	EndPaint(hWnd, &ps);
}

VOID OnDestroy(HWND hWnd)
{
	expenseManager->Save();
	delete controlChart;
	delete controlVScroll;
	delete cardManager;
	delete expenseManager;
	adjustScreen->Quit();
	delete adjustScreen;
	PostQuitMessage(0);
}

HBRUSH OnCtlColor(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	DWORD CtrlID = GetDlgCtrlID(hwndChild); //Window Control ID

	switch (CtrlID)
	{
	case IDC_BACK_TITLE:
	{
		HBRUSH hBrush = CreateSolidBrush(RGB(153, 209, 211));
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(153, 209, 211));
		return hBrush;
	}
	case IDC_BACK_EATING:
	{
		HBRUSH hBrush = CreateSolidBrush(backEating);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, backEating);
		return hBrush;
	}
	case IDC_BACK_TRANSPORT:
	{
		HBRUSH hBrush = CreateSolidBrush(backTransport);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, backTransport);
		return hBrush;
	}
	case IDC_BACK_HOME:
	{
		HBRUSH hBrush = CreateSolidBrush(backHome);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, backHome);
		return hBrush;
	}
	case IDC_BACK_VEHICLE:
	{
		HBRUSH hBrush = CreateSolidBrush(backVehicle);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, backVehicle);
		return hBrush;
	}
	case IDC_BACK_NECESSARY:
	{
		HBRUSH hBrush = CreateSolidBrush(backNecessary);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, backNecessary);
		return hBrush;
	}
	case IDC_BACK_SERVICE:
	{
		HBRUSH hBrush = CreateSolidBrush(backService);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, backService);
		return hBrush;
	}
	case IDC_BACK_NONE:
	{
		HBRUSH hBrush = CreateSolidBrush(backNone);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, backNone);
		return hBrush;
	}
	case IDC_BACK_SUM:
	{
		HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
		SetTextColor(hdc, RGB(130, 130, 130));
		SetBkColor(hdc, RGB(240, 240, 240));
		return hBrush;
	}
	default:
		return CreateSolidBrush(COLOR_BTNFACE);
	} //switch
}

void OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	// Get all the vertial scroll bar information.
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(hwnd, SB_VERT, &si);

	TEXTMETRIC tm;
	// Get the handle to the client area's device context. 
	HDC hdc = GetDC(hwnd);
	// Extract font dimensions from the text metrics. 
	GetTextMetrics(hdc, &tm);
	// vertical scrolling unit 
	int yChar = tm.tmHeight + tm.tmExternalLeading;
	// Save the position for comparison later on.
	int yPos = si.nPos;

	switch (code)
	{
		// User clicked the HOME keyboard key.
	case SB_TOP:
		si.nPos = si.nMin;
		break;

		// User clicked the END keyboard key.
	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;

		// User clicked the top arrow.
	case SB_LINEUP:
		si.nPos -= 1;
		break;

		// User clicked the bottom arrow.
	case SB_LINEDOWN:
		si.nPos += 1;
		break;

		// User clicked the scroll bar shaft above the scroll box.
	case SB_PAGEUP:
		si.nPos -= si.nPage;
		break;

		// User clicked the scroll bar shaft below the scroll box.
	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		break;

		// User dragged the scroll box.
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;

	default:
		break;
	}

	// Set the position and then retrieve it.  Due to adjustments
	// by Windows it may not be the same as the value set.
	si.fMask = SIF_POS | SIF_RANGE;

	si.nMax = 250;

	/*if (expenseManager->size() % 2)
	{
		si.nMax = ((expenseManager->size() + 1) / 5) * ((expenseManager->size() + 1) * 2.5);
	}
	else
	{
		si.nMax = (expenseManager->size() / 5) * (expenseManager->size() * 2.5);
	}*/

	SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

	GetScrollInfo(hwnd, SB_VERT, &si);

	// If the position has changed, scroll window and update it.
	if (si.nPos != yPos)
	{
		ScrollWindow(hwnd, 0, yChar * (yPos - si.nPos), NULL, NULL);
		UpdateWindow(hwnd);
	}
}

void ReceiveNoticeAdd(int expenseID)
{
	EnableWindow(hwndBtnDelAll, TRUE);
	DrawChart(hwndThis);
	cardManager->DrawCard(expenseID);
	if (expenseManager->size() == CExpense::MAX_NEXPENSE)
	{
		EnableWindow(hwndBtnAdd, FALSE);
	}
	SetSum();
}

void ReceiveNoticeRemove()
{
	if (expenseManager->size() == 0)
	{
		EnableWindow(hwndBtnDelAll, FALSE);
	}
	DrawChart(hwndThis);
	EnableWindow(hwndBtnAdd, TRUE);
	SetSum();
}

void ReceiveNoticeAdjust()
{
	SetSum();
	DrawChart(hwndThis);
}


void DrawChart(HWND hWnd)
{
	int *division;

	division = controlChart->GetDivision();

	controlVScroll->BackupScrollBar();
	controlVScroll->ResetScrollBar();
	int posX = 0;
	for (int i = 0; i < 7; ++i)
	{
		MoveWindow(hwndCharts[i], posX, 85, (700 * division[i]) / 100, 25, TRUE);

		posX += (700 * division[i]) / 100;
	}
	controlVScroll->RestoreScrollBar();

	delete[]division;
}

void SetSum()
{
	wchar_t szSum[101];

	double sum = expenseManager->GetSum();

	wchar_t *szTmp = CText::NumberToWCS(sum, 2);

	CText::AddDots(szTmp);

	wcscpy(szSum, szTmp);
	delete[]szTmp;

	wcscat(szSum, L" VND");

	SetWindowText(hwndSum, szSum);
}