#include <windef.h>

#ifndef  CEXPENSE
#define CEXPENSE

static const int CExpense_NType = 7;

//Background color
static const COLORREF backEating = RGB(127, 82, 93);
static const COLORREF backTransport = RGB(132, 132, 130);
static const COLORREF backHome = RGB(188, 198, 204);
static const COLORREF backVehicle = RGB(255, 248, 198);
static const COLORREF backNecessary = RGB(0, 128, 128);
static const COLORREF backService = RGB(135, 175, 199);
static const COLORREF backNone = RGB(109, 123, 141);


//Some expense types
static wchar_t CExpense_Types[][10] = //cause max length of strings is 9, TRANSPORT, and plus 1 character '\0' => 10
{
	L"EATING",
	L"TRANSPORT",
	L"HOME",
	L"VEHICLE",
	L"NECESSARY",
	L"SERVICE",
	L"NONE",
};

enum CExpenseType
{
	CEXPENSE_EATING,
	CEXPENSE_TRANSPORT,
	CEXPENSE_HOME,
	CEXPENSE_VEHICLE,
	CEXPENSE_NECESSARY,
	CEXPENSE_SERVICE,
	CEXPENSE_NONE
};

class CExpense
{
public:
	static const int MAX_NEXPENSE = 1000;

private:
	int m_ID; //id of this expense used to distinguish this id with the others
	CExpenseType m_type; //expense type which will be one of values in enum CExpenseType
	wchar_t *m_content; //content (description) about expense
	double m_money; //money was used for the expense
public:
	CExpense();
	CExpense(const CExpense& other);
	~CExpense();
	//Check if the object has created successfully
	//Return true if it has created successfully, else false
	bool IsCreated();
	void SetInfo(CExpenseType _type, wchar_t *_content, double _money);
	void operator= (const CExpense& other);
	bool operator== (const CExpense& other);

	int GetID() const;
	int IGetType() const;
	wchar_t *GetType() const;
	wchar_t *GetContent() const;
	double GetMoney() const;

	static wchar_t *ExpenseTypeToWCHAR(CExpenseType _type);
};

#endif // ! CEXPENSE
