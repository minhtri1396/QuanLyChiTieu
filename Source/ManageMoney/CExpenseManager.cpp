#include "stdafx.h"
#include "CExpenseManager.h"
#include "CArrangement.h"
#include "CText.h"
#include "CFile.h"


CExpenseManager::~CExpenseManager()
{
	RemoveAll();
}

int CExpenseManager::Add(const CExpense& _expense)
{
	CExpense *newExpense = new CExpense(_expense);

	m_vecExpenses.push_back(newExpense);

	return newExpense->GetID();
}

void CExpenseManager::RemoveAll()
{
	for each (CExpense *expense in m_vecExpenses)
	{
		delete expense;
	}

	m_vecExpenses.clear();
}

void CExpenseManager::Remove(int _expenseID)
{
	int index = 0;
	for each (CExpense *expense in m_vecExpenses)
	{
		if (expense->GetID() == _expenseID)
		{
			delete expense;
			m_vecExpenses.erase(m_vecExpenses.begin() + index);

			break;
		}

		++index;
	} //for each
}

void CExpenseManager::Remove(const CExpense& _expense)
{
	int index = 0;
	for each (CExpense *expense in m_vecExpenses)
	{
		if (*expense == _expense)
		{
			delete expense;
			m_vecExpenses.erase(m_vecExpenses.begin() + index);

			break;
		}

		++index;
	} //for
}

void CExpenseManager::Replace(int _oldExpenseID, const CExpense& _newExpense)
{
	for each (CExpense *expense in m_vecExpenses)
	{
		if (expense->GetID() == _oldExpenseID)
		{
			expense->SetInfo((CExpenseType)_newExpense.IGetType(), _newExpense.GetContent(), _newExpense.GetMoney());

			break;
		}
	} //for
}

int CExpenseManager::size()
{
	return m_vecExpenses.size();
}

CExpense CExpenseManager::GetExpense(int _expenseID)
{
	for each (CExpense *expense in m_vecExpenses)
	{
		if (expense->GetID() == _expenseID)
		{
			return *expense;
		}
	} //for each
}

CExpense CExpenseManager::operator[](int _index)
{
	if (_index < 0 || _index >= size())
	{
		throw;
	}

	return *m_vecExpenses[_index];
}

CArrangement machineSort;

//Compare between type of expense1 and type of expense2 to sort
int CmpExpenseType(const CExpense &_expense1, const CExpense &_expense2)
{
	return CText::CompareWStr(_expense1.GetType(), _expense2.GetType());
}

//Compare between content of expense1 and content of expense2 to sort
int CmpExpenseContent(const CExpense &_expense1, const CExpense &_expense2)
{
	return CText::CompareWStr(_expense1.GetContent(), _expense2.GetContent());
}

//Compare between money of expense1 and money of expense2 to sort
int CmpExpenseMoney(const CExpense &_expense1, const CExpense &_expense2)
{
	int resCmp = 0;

	if (_expense1.GetMoney() > _expense2.GetMoney())
	{
		resCmp = 1;
	}
	else if (_expense1.GetMoney() < _expense2.GetMoney())
	{
		resCmp = -1;
	}

	return resCmp;
}

//Sort list of expenses ascending
void CExpenseManager::SortAscWith(CSortBy _kind)
{
	switch (_kind)
	{
	case CSortBy::CLISTEXPENSES_TYPE:
		CArrangement::Sort(m_vecExpenses, CmpExpenseType,CArrangementType::CARRANGEMENT_ASCENDING);
		break;
	case CSortBy::CLISTEXPENSES_CONTENT:
		CArrangement::Sort(m_vecExpenses, CmpExpenseContent, CArrangementType::CARRANGEMENT_ASCENDING);
		break;
	case CSortBy::CLISTEXPENSES_MONEY:
		CArrangement::Sort(m_vecExpenses, CmpExpenseMoney, CArrangementType::CARRANGEMENT_ASCENDING);
		break;
	}
}

//Sort list of expenses descending
void CExpenseManager::SortDescWith(CSortBy _kind)
{
	switch (_kind)
	{
	case CSortBy::CLISTEXPENSES_TYPE:
		CArrangement::Sort(m_vecExpenses, CmpExpenseType, CArrangementType::CARRANGEMENT_DESCENDING);
		break;
	case CSortBy::CLISTEXPENSES_CONTENT:
		CArrangement::Sort(m_vecExpenses, CmpExpenseContent, CArrangementType::CARRANGEMENT_DESCENDING);
		break;
	case CSortBy::CLISTEXPENSES_MONEY:
		CArrangement::Sort(m_vecExpenses, CmpExpenseMoney, CArrangementType::CARRANGEMENT_DESCENDING);
		break;
	}
}

int CExpenseManager::GetExpenseID(int _index)
{
	if (_index < 0 || _index >= size())
	{
		return -1;
	}
	return m_vecExpenses[_index]->GetID();
}

void CExpenseManager::Save()
{
	CFile::SaveToFile(m_vecExpenses, FileNameSystem);
}

void CExpenseManager::Load()
{
	RemoveAll();
	CFile::LoadFromFile(m_vecExpenses, FileNameSystem);
}

double CExpenseManager::GetSum()
{
	double sum = 0;
	for each (CExpense *expense in m_vecExpenses)
	{
		sum += expense->GetMoney();
	}

	return sum;
}
