#include "stdafx.h"
#include "CExpense.h"
#include "CBitMap.h"
#include <Windows.h>


static CBitMap CExpense_BitMap(CExpense::MAX_NEXPENSE);

CExpense::CExpense()
{
	m_ID = CExpense_BitMap.Find(); //Set id for this object
	m_type = CExpenseType::CEXPENSE_NONE;
	m_content = 0;
	m_money = 0;
}

CExpense::CExpense(const CExpense& other)
{
	m_ID = CExpense_BitMap.Find(); //Set id for this object
	m_content = 0;
	*this = other;
}

CExpense::~CExpense()
{
	CExpense_BitMap.Clear(m_ID);
	delete[]m_content;
}

//Check if the object has created successfully
//Return true if it has created successfully, else false
bool CExpense::IsCreated()
{
	return m_ID != -1;
}

void CExpense::SetInfo(CExpenseType _type, wchar_t *_content, double _money)
{
	if (m_content)
	{
		delete[]m_content;
	}

	if (_content)
	{
		m_content = wcsdup(_content);
	}
	else
	{
		m_content = 0;
	}

	m_type = _type;

	m_money = _money;
}

void CExpense::operator= (const CExpense& other)
{
	SetInfo(other.m_type, other.m_content, other.m_money);
}

bool CExpense::operator== (const CExpense& other)
{
	return m_ID == other.m_ID;
}

int CExpense::GetID() const
{
	return m_ID;
}

int CExpense::IGetType() const
{
	return (int)m_type;
}

wchar_t *CExpense::GetType() const
{
	return CExpense_Types[m_type];
}

wchar_t* CExpense::GetContent() const
{
	return &m_content[0];
}

double CExpense::GetMoney() const
{
	return m_money;
}

wchar_t *CExpense::ExpenseTypeToWCHAR(CExpenseType _type)
{
	return CExpense_Types[_type];
}