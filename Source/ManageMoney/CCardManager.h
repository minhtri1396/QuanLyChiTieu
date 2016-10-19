#include "CExpenseManager.h"
#include "CCard.h"
#include "CScrollBar.h"
#include "CInfoCard.h"
#include "Resource.h"
#include <vector>

#ifndef CCARDMANAGER
#define CCARDMANAGER

typedef void(*FuncNotice)();

//Just declare one CCardManager object in the time cause CCardManager_gCardManager variable in CCardManager.cpp
class CCardManager
{
public:
	static const int MAX_NCARD = CExpense::MAX_NEXPENSE; //the number of card id we can have
	static const int MAX_CARD_ON_ROW = 4;

private:
	FuncNotice m_informAdjustComplete;
	FuncNotice m_informRemoveComplete;
	CScrollBar *m_controlVScroll;
	int m_nCard, m_nCardOnARow;
	CInfoCard *m_infoCardScreen;

	HINSTANCE m_hInst;
	HWND m_hwndParent;
	CCard *m_Cards[MAX_NCARD];
	std::vector<int> m_vecIDMap;
	CExpenseManager* m_expenseManager;

	MessageInteger* m_receiveID;
	void AllocateForReceiveID();

	//Identify where _cardID is in m_vecIDMap
	int IdentifyPosCardID(int _cardID);

	//Use this function when remove one card
	//posStart = position of starting where we start move card
	void MoveCard(int posStart);
	void Remove(int _cardID);

	HANDLE m_hwndSemReceiveID; //use when check or set m_receiveID = NULL
	HANDLE m_hwndSemDestructor; //use to destructor wait until WaitForId function has done
	void SetReceiveIDAsNULL();

	friend void AdjustComplete(int expenseID);

	//This function called by thread
	DWORD WaitForId();
	static DWORD WINAPI StaticThreadStart(void* Param);
	
public:
	CCardManager(HWND _hwndParent, HINSTANCE _hInst, CExpenseManager* _expenseManager,
		CScrollBar *_controlVScroll, CInfoCard *_infoCardScreen, FuncNotice _informRemoveComplete, FuncNotice _informAdjustComplete);
	~CCardManager();
	void DrawCard(int _expenseID);
	void RemoveAll();
};

#endif