#include "CExpense.h"
#include <vector>

#ifndef CARRANGEMENT
#define CARRANGEMENT

enum CArrangementType
{
	CARRANGEMENT_ASCENDING,
	CARRANGEMENT_DESCENDING
};

//TValue is type of all value in vector (which we want to sort)
class CArrangement
{
private:
	static void Ascend(std::vector<CExpense*>& _vec);
	static void Descend(std::vector<CExpense*>& _vec);

public:
	static void Sort(std::vector<CExpense*>& _vec, int (*_funcCompare)(const CExpense&, const CExpense&),
		CArrangementType sortingType = CARRANGEMENT_ASCENDING);
};

#endif // !CARRANGEMENT
