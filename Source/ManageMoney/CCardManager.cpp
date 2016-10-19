#include "stdafx.h"
#include "CCardManager.h"
#include <thread>

CCardManager *CCardManager_this;


DWORD WINAPI CCardManager::StaticThreadStart(void* Param)
{
	CCardManager* This = (CCardManager*)Param;
	return This->WaitForId();
}

void CCardManager::SetReceiveIDAsNULL()
{
	WaitForSingleObject(m_hwndSemReceiveID, INFINITE);

	m_receiveID = NULL;

	ReleaseSemaphore(m_hwndSemReceiveID, 1, NULL);
}

//This function will be called by another function
void AdjustComplete(int expenseID)
{
	CExpense expense = CCardManager_this->m_expenseManager->GetExpense(expenseID);
	//card->GetExpenseID()
	for each (int posID in CCardManager_this->m_vecIDMap)
	{
		if (CCardManager_this->m_Cards[posID]->GetExpenseID() == expenseID)
		{
			CCardManager_this->m_Cards[posID]->DrawInfo(expenseID, (CExpenseType)expense.IGetType(),
				expense.GetContent(), expense.GetMoney());
		}
	}

	CCardManager_this->m_informAdjustComplete();
}

//This function called by thread
DWORD CCardManager::WaitForId()
{
	m_receiveID->value = CExpense::MAX_NEXPENSE;
	int id;
	while (1)
	{
		WaitForSingleObject(m_hwndSemReceiveID, INFINITE);
		if (m_receiveID == NULL)
		{
			break;
		}
		id = m_receiveID->value;
		if (id != CExpense::MAX_NEXPENSE)
		{
			m_receiveID->value = CExpense::MAX_NEXPENSE;
			ReleaseSemaphore(m_hwndSemReceiveID, 1, NULL);
			if (id < 0)
			{
				//check in OnCommand function of CCard.cpp to clarify
				++id;
				id *= -1;
				//Get expense has the id
				CCard *card = m_Cards[id];
				int expenseID = card->GetExpenseID();
				m_infoCardScreen->Show(AdjustComplete, expenseID);
			}
			else
			{
				Remove(id);
			}
		}
		else
		{
			ReleaseSemaphore(m_hwndSemReceiveID, 1, NULL);
		}
		Sleep(10);
	}
	ReleaseSemaphore(m_hwndSemReceiveID, 1, NULL);
	ReleaseSemaphore(m_hwndSemDestructor, 1, NULL);

	return 0;
}

CCardManager::CCardManager(HWND _hwndParent, HINSTANCE _hInst, CExpenseManager* _expenseManager,
	CScrollBar *_controlVScroll, CInfoCard *_infoCardScreen, FuncNotice _informRemoveComplete, FuncNotice _informAdjustComplete)
{
	CCardManager_this = this;

	m_informRemoveComplete = _informRemoveComplete;
	m_informAdjustComplete = _informAdjustComplete;
	m_controlVScroll = _controlVScroll;
	m_infoCardScreen = _infoCardScreen;
	m_expenseManager = _expenseManager;
	m_receiveID = NULL;

	m_hwndSemReceiveID = CreateSemaphore(NULL, 1, 1, NULL);
	m_hwndSemDestructor = CreateSemaphore(NULL, 0, 1, NULL);

	m_nCard = m_nCardOnARow = 0;
	m_hInst = _hInst;
	m_hwndParent = _hwndParent;

	for (int i = 0; i < _expenseManager->size(); ++i)
	{
		DrawCard(_expenseManager->GetExpenseID(i));
	}
}

CCardManager::~CCardManager()
{
	if (m_receiveID == NULL) //still not call thread WaitForId
	{
		ReleaseSemaphore(m_hwndSemDestructor, 1, NULL);
	}

	WaitForSingleObject(m_hwndSemReceiveID, INFINITE);
	delete m_receiveID;
	m_receiveID = NULL;
	ReleaseSemaphore(m_hwndSemReceiveID, 1, NULL);

	WaitForSingleObject(m_hwndSemDestructor, INFINITE);

	RemoveAll();
}

void CCardManager::AllocateForReceiveID()
{
	if (m_receiveID == NULL)
	{
		m_receiveID = new MessageInteger;

		DWORD threadID;
		CreateThread(NULL, 0, StaticThreadStart, (void*)this, 0, &threadID);

		while (m_receiveID->value != CExpense::MAX_NEXPENSE) //to make sure that m_receiveID->value has been set default value
		{
			Sleep(10);
		}
	}
}

void CCardManager::DrawCard(int _expenseID)
{
	if (m_nCard < MAX_NCARD)
	{
		CExpense expense = m_expenseManager->GetExpense(_expenseID);

		AllocateForReceiveID();

		m_controlVScroll->BackupScrollBar();
		m_controlVScroll->ResetScrollBar();
		//Init card
		int height = 180 + 10 * m_nCard; //number 150 is above distance, number 10 is y distance of each card
		int width = m_nCardOnARow * 10 + 10; //x distance of each card
		CCard* card = new CCard(m_hwndParent, m_hInst,
			m_nCardOnARow * CARD_MAX_WIDTH + width,
			m_nCard * CARD_MAX_HEIGHT + height, m_receiveID);
		card->InitCard((CExpenseType)expense.IGetType());
		m_controlVScroll->RestoreScrollBar();

		++m_nCardOnARow;
		if (m_nCardOnARow == MAX_CARD_ON_ROW)
		{
			m_nCardOnARow = 0;
			++m_nCard;
		}

		//Draw card
		card->DrawInfo(_expenseID, (CExpenseType)expense.IGetType(), expense.GetContent(), expense.GetMoney());
		//Save card in vector
		int cardId = card->GetId();
		m_vecIDMap.push_back(cardId);
		m_Cards[cardId] = card;
	}
}

//Identify where _cardID is in m_vecIDMap
int  CCardManager::IdentifyPosCardID(int _cardID)
{
	int posID = -1; //position of the id in m_vecIDMap

	int sizeRun = m_vecIDMap.size() - 1;
	if (sizeRun >= 0)
	{
		while (posID < sizeRun)
		{
			if (m_vecIDMap[++posID] == _cardID)
			{
				break;
			}
		}
	}

	return posID;
}

//Use this function when remove one card
//posStart = position of starting where we start move card
void CCardManager::MoveCard(int posStart)
{
	m_controlVScroll->BackupScrollBar();
	m_controlVScroll->ResetScrollBar();
	DWORD dwX, dwY;
	DWORD dwPrevX, dwPrevY;
	dwPrevX = m_Cards[m_vecIDMap[posStart]]->GetX();
	dwPrevY = m_Cards[m_vecIDMap[posStart]]->GetY();
	//Move cards
	int sizeRun = m_vecIDMap.size() - 1;
	for (int i = posStart + 1; i <= sizeRun; ++i)
	{
		dwX = m_Cards[m_vecIDMap[i]]->GetX();
		dwY = m_Cards[m_vecIDMap[i]]->GetY();

		m_Cards[m_vecIDMap[i]]->SetX(dwPrevX);
		m_Cards[m_vecIDMap[i]]->SetY(dwPrevY);

		dwPrevX = dwX;
		dwPrevY = dwY;
	}
	m_controlVScroll->RestoreScrollBar();

	if (m_expenseManager->size() == CScrollBar::MAX_ITEM)
	{
		m_controlVScroll->ResetZero();
	}

	//Reset
	if (m_nCardOnARow == 0)
	{
		m_nCardOnARow = MAX_CARD_ON_ROW - 1;
		--m_nCard;
	}
	else
	{
		--m_nCardOnARow;
	}
}

void CCardManager::Remove(int _cardID)
{
	//Identify where _cardID is in m_vecIDMap
	int posID = IdentifyPosCardID(_cardID);
	if (posID != -1) //can't find the id in ID map
	{
		//Move cards
		MoveCard(posID);
		//Remove card
		m_vecIDMap.erase(m_vecIDMap.begin() + posID);
		int expenseID; //store expense id
		int sizeRun = m_vecIDMap.size();
		if (sizeRun > 0)
		{
			expenseID = m_Cards[_cardID]->Remove(m_Cards[m_vecIDMap[sizeRun - 1]]);
		}
		else
		{
			SetReceiveIDAsNULL();
			expenseID = m_Cards[_cardID]->Remove(NULL);
		}
		delete m_Cards[_cardID];
		m_Cards[_cardID] = NULL;
		//Remove expense
		if (expenseID != -1)
		{
			m_expenseManager->Remove(expenseID);
		}
		m_informRemoveComplete();
	} //if (posID != -1)
}

void CCardManager::RemoveAll()
{
	if (m_expenseManager->size())
	{
		m_controlVScroll->ResetZero();
		for each (int posID in m_vecIDMap)
		{
			m_Cards[posID]->Remove(NULL);
			delete m_Cards[posID];
		}
		m_vecIDMap.clear();
		m_nCard = m_nCardOnARow = 0;
		m_expenseManager->RemoveAll();
	}
}
