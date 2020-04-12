// Copyright (C) 2015-16 Andrzej Jaszkiewicz

#ifndef __NONDOMINATEDSET_H_
#define __NONDOMINATEDSET_H_

#include "global.h"
#include "solution.h"
#include "solutionsset.h"
#include "problem.h"

const int		PARAMETER_SPACE = 1;
const int		OBJECTIVE_SPACE = 2;

extern TProblem Problem;



/** This class represent set of nondominated solutions 
 * 
 * This class has methods usable to filter out nondominated solutions to set.
 * It also contains functions that reduce number of solutions in set using
 * clustering. Nondominated set is a TSolutionSet that contains only nondominated
 * solution. It also can have null values in vector storing solutions.  
 **/
class TNondominatedSet: public TSolutionsSet
{
private:
protected:

public:

    /** Update set using given solution
     * 
     * This function reduce nondominated set to given number of solutions.
     * Should be overloaded by offspring class.
     *
     * @param Solution possibly nondominated solution
     * @return true if solution is nondominated and set has been updated with this solution, false solution is dominated by solution in set
     **/
	virtual bool Update(TPoint& Solution) = 0;
/*    {
        return false;
    }*/

    /** Merge two nondominated sets and give final nondominated set.
     * 
     * This function add two sets and eliminate dominated solutions from both sets.
     * Should be overloaded by offspring class.
     *
     * @param NondominatedSet possibly nondominated solution
     * @return if true there weren't dominated solution, if false some solutions have been eliminated 
     **/
	virtual bool Update(TNondominatedSet& NondominatedSet)
    {
        return false;
    }

    /** This function choose random solution from set of solutions
     * 
     * Probability of choose for every solution should be equal. 
     * Should be overloaded by offspring class.
     *
     * @param pSolution reference to pointer where solution will be placed
     **/
	virtual void GetRandomSolution(TPoint* &pSolution)
    {
        pSolution = NULL;
    }

    /** Delete all solutions from set.
     * 
     * Every solution in set is released and vector is reallocated to size 0. 
     **/
    virtual void DeleteAll()
    {
    }

	TNondominatedSet() : TSolutionsSet()
	{ 
	}
};

#endif
