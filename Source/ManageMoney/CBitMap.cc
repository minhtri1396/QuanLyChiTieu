// bitmap.c 
//	Routines to manage a bitmap -- an array of bits each of which
//	can be either on or off.  Represented as an array of integers.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "stdafx.h"
#include "CBitMap.h"
#include <math.h>

#define divRoundUp(numBits, BitsInWord) ceil((numBits * 1.0) / BitsInWord)

//----------------------------------------------------------------------
// BitMap::BitMap
// 	Initialize a bitmap with "nitems" bits, so that every bit is clear.
//	it can be added somewhere on a list.
//
//	"nitems" is the number of bits in the bitmap.
//----------------------------------------------------------------------

CBitMap::CBitMap(int nitems)
{
	numBits = nitems;
	numWords = divRoundUp(numBits, BitsInWord);
	map = new unsigned int[numWords];
	for (int i = 0; i < numBits; i++)
	{
		Clear(i);
	}
}

//----------------------------------------------------------------------
// BitMap::~BitMap
// 	De-allocate a bitmap.
//----------------------------------------------------------------------

CBitMap::~CBitMap()
{
	delete map;
}

//----------------------------------------------------------------------
// BitMap::Set
// 	Set the "nth" bit in a bitmap.
//
//	"which" is the number of the bit to be set.
//----------------------------------------------------------------------

void
CBitMap::Mark(int which)
{
	if (which >= 0 && which < numBits)
	{
		map[which / BitsInWord] |= 1 << (which % BitsInWord);
	}
}

//----------------------------------------------------------------------
// BitMap::Clear
// 	Clear the "nth" bit in a bitmap.
//
//	"which" is the number of the bit to be cleared.
//----------------------------------------------------------------------

void
CBitMap::Clear(int which)
{
	if (which >= 0 && which < numBits)
	{
		map[which / BitsInWord] &= ~(1 << (which % BitsInWord));
	}
}

//----------------------------------------------------------------------
// BitMap::Test
// 	Return TRUE if the "nth" bit is set.
//
//	"which" is the number of the bit to be tested.
//----------------------------------------------------------------------

bool
CBitMap::Test(int which)
{
	if (which >= 0 && which < numBits)
	{
		if (map[which / BitsInWord] & (1 << (which % BitsInWord)))
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------
// BitMap::Find
// 	Return the number of the first bit which is clear.
//	As a side effect, set the bit (mark it as in use).
//	(In other words, find and allocate a bit.)
//
//	If no bits are clear, return -1.
//----------------------------------------------------------------------

int
CBitMap::Find()
{
	for (int i = 0; i < numBits; i++)
	{
		if (!Test(i)) {
			Mark(i);
			return i;
		}
	}
	return -1;
}

//----------------------------------------------------------------------
// BitMap::NumClear
// 	Return the number of clear bits in the bitmap.
//	(In other words, how many bits are unallocated?)
//----------------------------------------------------------------------

int
CBitMap::NumClear()
{
	int count = 0;

	for (int i = 0; i < numBits; i++)
	{
		if (!Test(i))
		{
			count++;
		}
	}
	return count;
}
