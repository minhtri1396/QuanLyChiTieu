#include "stdafx.h"
#include "CChart.h"

CChart::CChart(CExpenseManager *_expenseManager)
{
	m_expenseManager = _expenseManager;
}

int* CChart::GetDivision()
{
	int countTypes[CExpense_NType] = { 0 };
	int* res = new int[CExpense_NType];

	int size = m_expenseManager->size();

	if (size == 0)
	{
		int sum = 0;
		for (int i = CExpense_NType - 2; i >= 0; --i)
		{
			res[i] = 14;
			sum += res[i];
		}
		res[CExpense_NType - 1] = 100 - sum;
	}
	else
	{
		CExpense expense;
		for (int i = 0; i < size; ++i)
		{
			expense = (*m_expenseManager)[i];
			++countTypes[expense.IGetType()];
		}

		int sum = 0;
		int countZero = 0;
		for (int i = CExpense_NType - 2; i >= 0; --i)
		{
			res[i] = floor((countTypes[i] * 100.0) / size);
			if (res[i] == 0)
			{
				++countZero;
			}

			sum += res[i];
		}

		if (countZero > 0)
		{
			sum = 0;
			countZero = countZero / (CExpense_NType - countZero);
			for (int i = CExpense_NType - 2; i >= 0; --i)
			{
				if (res[i] == 0)
				{
					res[i] = 1;
				}
				else
				{
					res[i] -= countZero;
				}

				sum += res[i];
			}
		}

		res[CExpense_NType - 1] = 100 - sum;
		if (res[CExpense_NType - 1] <= 0)
		{
			res[CExpense_NType - 1] = 1;
		}
	}

	return res;
}