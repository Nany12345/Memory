// Copyright (C) 2015-17 Andrzej Jaszkiewicz

#ifndef __TLISTSET_H_
#define __TLISTSET_H_

#include "global.h"
#include "solution.h"
#include "solutionsset.h"

/** Implementation of nondominated set based on list
 *
 * This is simple but not the most efficient solution of nondominated set.
 **/
template <class TProblemSolution> class TListSet: public TSolutionsSet
{
protected:
	
    /** Update nadir point values
     * 
     * Method updates nadir point approximation. 
     * Nadir point is approximated on current nondominated set.  
     *
     * @param iRemovedSolution index of solution to be removed
     **/
	void UpdateNadir (int iRemovedSolution);

	virtual bool UpdateBiObjective(TPoint& Solution);

public:
	bool wasEqual;
	bool wasDominated;
	bool wasDominating;

	bool useSortedList = false;

	TSolutionsSet allSolutions;

	long updates = 0;

	long dominating = 0;
	long dominated = 0;
	long nondominated = 0;
	long equal = 0;

	static bool before(TPoint* solution1, TPoint* solution2);
	void sortLex();
	bool equalSets(TListSet <TProblemSolution>& otherSet);

	virtual void Load(char* FileName);

	void RemoveDominating (TProblemSolution& Solution);

    /** Update set using given solution
     * 
     * This function reduce nondominated set to given number of solutions.
     *
     * @param Solution possibly nondominated solution
     * @return if true solution is nondominated and set has been updated with this solution, false solution is dominated by solution in set
     **/
	virtual bool Update(TPoint& Solution);

	virtual void Add(TPoint& Solution);

	virtual bool checkUpdate(TPoint& Solution);

    /** This function choose random solution from set of solutions
     * 
     * Probability of choose for every solution should be equal. 
     *
     * @param pSolution reference to pointer where solution will be placed
     **/
	virtual void GetRandomSolution(TPoint* &pSolution);

    /** Delete all solutions from set.
     * 
     * Every solution in set is released and vector is reallocated to size 0. 
     **/
	virtual void DeleteAll();

	/** Destruct object 
     *
     **/
	TListSet();
	
	~TListSet () {
		DeleteAll();
	};
};

#include "tlistset.cpp"

#endif // _TLISTSET_H_
