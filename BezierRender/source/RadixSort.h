//////////////////////////////////////////////////////////////////////
// RadixSort.h
// Description:		Linear-time sorting 
//
// By:				Ben Woodhouse 
// Date:			12/2003
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIXSORT_H__83FEAB60_11EF_4C72_9813_3C3DE3B2B6F5__INCLUDED_)
#define AFX_RADIXSORT_H__83FEAB60_11EF_4C72_9813_3C3DE3B2B6F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RadixSort
{
public:
	RadixSort(int maxNumVals);
	virtual ~RadixSort();
	void sort(	float * input, 
				int numvals, 
				int * outputOrder);

protected:
	unsigned int counter[256];
	unsigned int offset[256];

	float * valueSwap;
	int * orderSwap;
	int maxNumVals;
};

#endif // !defined(AFX_RADIXSORT_H__83FEAB60_11EF_4C72_9813_3C3DE3B2B6F5__INCLUDED_)
