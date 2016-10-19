#ifndef CMESSAGE
#define CMESSAGE

struct MessageInteger
{
	int value;
	void operator()(int _value)
	{
		value = _value;
	}
};


#endif