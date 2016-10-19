#include "CExpense.h"
#include <vector>

#ifndef CEXPENSEMANAGER
#define CEXPENSEMANAGER

enum CSortBy
{
	CLISTEXPENSES_TYPE,
	CLISTEXPENSES_CONTENT,
	CLISTEXPENSES_MONEY
};

class CExpenseManager
{
private:
	std::vector<CExpense *> m_vecExpenses; //use this vector to manage expense we have

public:
	~CExpenseManager();
	int Add(const CExpense& _expense);
	void RemoveAll();
	void Remove(int _expenseID);
	void Remove(const CExpense& _expense);
	void Replace(int _oldExpenseID, const CExpense& _newExpense);
	//Sort list of expenses ascending
	void SortAscWith(CSortBy _kind);
	//Sort list of expenses descending
	void SortDescWith(CSortBy _kind);

	void Save();
	void Load();

	int size();
	int GetExpenseID(int _index);
	//Don't use expense which GetExpense return to get id of expense
	//Cause GetExpense return a clone expense, so ID will be different
	CExpense GetExpense(int _expenseID);
	//Don't use expense which operator[] return to get id of expense
	//Cause operator[] return a clone expense, so ID will be different
	CExpense operator[](int _index);

	double GetSum();
};

#endif // !CLISTEXPENSES
