#include "CExpenseManager.h"

#ifndef CCHART
#define CCHART

class CChart
{
private:
	CExpenseManager *m_expenseManager;
public:
	CChart(CExpenseManager *_expenseManager);
	int* GetDivision();
};

#endif // !CCHART
