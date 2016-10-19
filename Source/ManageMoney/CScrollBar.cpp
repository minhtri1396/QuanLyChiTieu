#include "stdafx.h"
#include "CScrollBar.h"
#include <limits>

DWORD WINAPI CScrollBar::StaticThreadStart(void* Param)
{
	CScrollBar* This = (CScrollBar*)Param;
	return This->CheckShouldShow();
}

//This function called by thread
DWORD CScrollBar::CheckShouldShow()
{
	LONG wndStyle;
	RECT mainRect;

	while (1)
	{
		WaitForSingleObject(hwndSemCheckShouldShow, INFINITE);
		if (m_expenseManager == NULL)
		{
			break;
		}
		wndStyle = GetWindowLong(m_hwndParent, GWL_STYLE);
		GetWindowRect(m_hwndParent, &mainRect);

		if (m_expenseManager->size() >= MAX_ITEM)
		{
			if (!(wndStyle & WS_VSCROLL)) //vertical scroll bar is invisible
			{
				ShowScrollBar(m_hwndParent, SB_VERT, TRUE);

				MoveWindow(m_hwndParent, mainRect.left, mainRect.top, 1205, mainRect.bottom - mainRect.top, TRUE);
			}
		}
		else
		{
			if (wndStyle & WS_VSCROLL) //vertical scroll bar is visible
			{
				ResetScrollBar();

				ShowScrollBar(m_hwndParent, SB_VERT, FALSE);

				MoveWindow(m_hwndParent, mainRect.left, mainRect.top, 1189, mainRect.bottom - mainRect.top, TRUE);
			}
		}
		ReleaseSemaphore(hwndSemCheckShouldShow, 1, NULL);

		Sleep(10);
	}
	ReleaseSemaphore(hwndSemDestructor, 1, NULL);

	return 0;
}

CScrollBar::CScrollBar(HWND hwndParent, CExpenseManager* expenseManager)
{
	posBackup = 0;
	m_hwndParent = hwndParent;
	m_expenseManager = expenseManager;

	hwndSemBackup = CreateSemaphore(0, 1, 1, NULL);
	hwndSemCheckShouldShow = CreateSemaphore(0, 1, 1, NULL);
	hwndSemDestructor = CreateSemaphore(0, 0, 1, NULL);

	DWORD threadID;
	CreateThread(NULL, 0, StaticThreadStart, (void*)this, 0, &threadID);
}

CScrollBar::~CScrollBar()
{
	WaitForSingleObject(hwndSemCheckShouldShow, INFINITE);
	m_expenseManager = NULL;
	ReleaseSemaphore(hwndSemCheckShouldShow, 1, NULL);

	WaitForSingleObject(hwndSemDestructor, INFINITE);
}

void CScrollBar::ResetZero()
{
	// Get all the vertial scroll bar information.
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(m_hwndParent, SB_VERT, &si);

	TEXTMETRIC tm;
	// Get the handle to the client area's device context. 
	HDC hdc = GetDC(m_hwndParent);
	// Extract font dimensions from the text metrics. 
	GetTextMetrics(hdc, &tm);
	// vertical scrolling unit 
	int yPos = tm.tmHeight + tm.tmExternalLeading;
	// Save the position for comparison later on.
	int curPos;
	if (m_expenseManager->size() >= MAX_ITEM)
	{
		//WaitForSingleObject(hwndSemCheckShouldShow, INFINITE);
		LONG wndStyle = GetWindowLong(m_hwndParent, GWL_STYLE);
		if (wndStyle & WS_VSCROLL) //vertical scroll bar is visible
		{
			curPos = yPos * si.nPos;
		}
		else
		{
			curPos = 0;
		}
		//ReleaseSemaphore(hwndSemCheckShouldShow, 1, NULL);
	}
	else
	{
		curPos = 0;
	}

	si.fMask = SIF_POS;
	si.nPos = 0;

	SetScrollInfo(m_hwndParent, SB_VERT, &si, TRUE);
	GetScrollInfo(m_hwndParent, SB_VERT, &si);

	ScrollWindow(m_hwndParent, 0, curPos, NULL, NULL);
	UpdateWindow(m_hwndParent);
}

void CScrollBar::ResetScrollBar()
{
	// Get all the vertial scroll bar information.
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(m_hwndParent, SB_VERT, &si);

	si.fMask = SIF_POS;
	si.nPos = 0;

	SetScrollInfo(m_hwndParent, SB_VERT, &si, TRUE);
	GetScrollInfo(m_hwndParent, SB_VERT, &si);

	ScrollWindow(m_hwndParent, 0, posBackup, NULL, NULL);
	UpdateWindow(m_hwndParent);
}

void CScrollBar::BackupScrollBar()
{
	WaitForSingleObject(hwndSemBackup, INFINITE);
	siBackup.cbSize = sizeof(siBackup);
	siBackup.fMask = SIF_ALL;
	GetScrollInfo(m_hwndParent, SB_VERT, &siBackup);

	TEXTMETRIC tm;
	// Get the handle to the client area's device context. 
	HDC hdc = GetDC(m_hwndParent);
	// Extract font dimensions from the text metrics. 
	GetTextMetrics(hdc, &tm);
	// vertical scrolling unit 
	int yPos = tm.tmHeight + tm.tmExternalLeading;
	// Save the position for comparison later on.
	if (m_expenseManager->size() >= MAX_ITEM)
	{
		//WaitForSingleObject(hwndSemCheckShouldShow, INFINITE);
		LONG wndStyle = GetWindowLong(m_hwndParent, GWL_STYLE);
		if (wndStyle & WS_VSCROLL) //vertical scroll bar is visible
		{
			posBackup = yPos * siBackup.nPos;
		}
		else
		{
			posBackup = 0;
		}
		//ReleaseSemaphore(hwndSemCheckShouldShow, 1, NULL);
	}
	else
	{
		posBackup = 0;
	}
}

void CScrollBar::RestoreScrollBar()
{
	siBackup.fMask = SIF_POS;
	SetScrollInfo(m_hwndParent, SB_VERT, &siBackup, TRUE);
	GetScrollInfo(m_hwndParent, SB_VERT, &siBackup);

	Sleep(50);

	ScrollWindow(m_hwndParent, 0, -posBackup, NULL, NULL);
	UpdateWindow(m_hwndParent);
	ReleaseSemaphore(hwndSemBackup, 1, NULL);
}
