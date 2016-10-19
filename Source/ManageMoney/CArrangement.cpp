#include "stdafx.h"
#include "CArrangement.h"

static int(*CArrangement_funcCompare)(const CExpense &value1, const CExpense &value2);

void CArrangement::Ascend(std::vector<CExpense*>& _vec)
{
	int size = _vec.size();

	for (int i = 0; i < size - 1; ++i)
	{
		for (int j = i + 1; j < size; ++j)
		{
			if (CArrangement_funcCompare(*_vec[i], *_vec[j]) == 1) //*_vec[i] > *_vec[j]
			{
				CExpense* pExpense = _vec[i];
				_vec[i] = _vec[j];
				_vec[j] = pExpense;
			}
		} //for (int j = i + 1; j < size; ++j)
	} //for (int i = 0; i < size - 1; ++i)
}

void CArrangement::Descend(std::vector<CExpense*>& _vec)
{
	int size = _vec.size();

	for (int i = 0; i < size - 1; ++i)
	{
		for (int j = i + 1; j < size; ++j)
		{
			if (CArrangement_funcCompare(*_vec[i], *_vec[j]) == -1) //*_vec[i] < *_vec[j]
			{
				CExpense* pExpense = _vec[i];
				_vec[i] = _vec[j];
				_vec[j] = pExpense;
			}
		} //for (int j = i + 1; j < size; ++j)
	} //for (int i = 0; i < size - 1; ++i)
}

void CArrangement::Sort(std::vector<CExpense*>& _vec, int(*_funcCompare)(const CExpense&, const CExpense&), CArrangementType sortingType)
{
	CArrangement_funcCompare = _funcCompare;

	if (CARRANGEMENT_ASCENDING == sortingType)
	{
		Ascend(_vec);
	}
	else if (CARRANGEMENT_DESCENDING == sortingType)
	{
		Descend(_vec);
	}
}