// Copyright (C) 2015-16 Andrzej Jaszkiewicz


// MOMHSolution.h: interface for the TMOMHSolution class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOMHSolution_H__EE158131_9605_11D3_814C_000000000000__INCLUDED_)
#define AFX_MOMHSolution_H__EE158131_9605_11D3_814C_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "solution.h"
#include "solutionsset.h"
#include "tlistset.h"

class TNondominatedSet;

/** Abstract class - base for classes corresponding to solutions
*	of particular problems 
*
*	Note that this is not an abstract class in C++ sence (it has
*	no abstract methods) but you should not construct objects of 
*	this class.
*/
class TMOMHSolution : public TSolution  
{
public:
	/** Constructs new initial solution 
	*
	*	The solution should be constructed by randomized algorithm.
	*	You may use a randomized heuristic that would produce solutions 
	*	close to the nondominated set.
	*	The method should be overritten in specialization of TMOMHSolution for 
	*	your problem.
	*/
	TMOMHSolution () : TSolution () {
	}

	/** Constructs new solution by recombination of the parents.
	*
	*	The new solution should preserve important common feateures of the parents.
	*	You may use a randomized heuristic that would produce solutions 
	*	close to the nondominated set.
	*	The method should be overritten in specialization of TMOMHSolution for 
	*	your problem.
	*/
	TMOMHSolution (TMOMHSolution& Parent1, TMOMHSolution& Parent2): TSolution () {
	}

	/** Find next local move to be performed in local search type
	*	algorithms
	*
	*	There is no need to really perform the move (in most cases
	*	this is not efficient approach). The method may just find
	*	description of the next local move and update 
	*	ObjectiveValues field */
	virtual void FindLocalMove () {
	}

	/**	Accepts the local move found by FindLocalMove () method 
	*
	*	The recommended approach is to really perform the local
	*	move (i.e. to change the solution's description) in this 
	*	method. In this case FindLocalMove () just finds 
	*	description of the local move but does not perform it.
	*
	*	Another approach is to perform the local move FindLocalMove () 
	*	method (i.e. to change the solution's description). In 
	*	this case AcceptLocalMove () needs not to be overriten */
	virtual void AcceptLocalMove () {
	}

	/**	Rejects the local move found by FindLocalMove () method 
	*
	*	In the recommended approach FindLocalMove () just finds 
	*	description of the local move but does not perform it.
	*	In this case RejectLocalMove () should just restore
	*	original ObjectiveValues. You can use SaveObjectiveValues ()
	*	and RestoreObjectiveValues () to this end.
	*
	*	Another approach is to perform the local move FindLocalMove () 
	*	method (i.e. to change the solution's description). In 
	*	this case RejectLocalMove () should return to the original
	*	solution's description */
	virtual void RejectLocalMove () {
	}

	/** Mutates the solution */
	virtual void Mutate () {}

    /** Compute distance to given solution in parameter space.
	*  
    *   This measure should reflecs similarity of two solutions.
    *   Must be overloaded in derivered class because this measure 
    *   is solution dependent.
    *
    *   @param oSolution solution to that the distance will be measured
    *   @return computed distance
	*/
	virtual double ParameterValuesDistance(TMOMHSolution& oSolution) 
	{
		return 0.0;
	}

    /** Compute distance to given solution in objective space.
	*  
    * This measure reflects how close are 2 solutions in objective space.
    * In this case Euclidean distance is returned. Function can be overloaded
    * and own measure applied.
    *
    * @param oSolution solution to that the distance will be measured
    * @param bScalarize true - values of objectives should will be scalarized using ideal and nadir points
    * @param dIdeal ideal point or approximation
    * @param dNadir nadir point or approximation
    * @return computed distance
	*/
	virtual double ObjectiveValuesDistance(TMOMHSolution& oSolution, bool bScalarize, vector<double> &dIdeal, vector<double> &dNadir) 
	{
	
	int			i = 0;
	double		dSum = 0.0;
	double		dDifference = 0.0;

		for(i = 0; i < NumberOfObjectives; i++) 
		{
				dDifference = ObjectiveValues[i] - oSolution.ObjectiveValues[i];	
				if (bScalarize == true)
				{
					if (dIdeal[i] != dNadir[i]) 
					{
						dDifference = dDifference / (dIdeal[i] - dNadir[i]);
						dSum += dDifference * dDifference;
					}
					else
					{
						dSum += 0.0;
					}
				}
				else
				{
					dSum += dDifference * dDifference;
				}
		}

		dSum = sqrt(dSum);

		return dSum;
	}

	virtual void GenerateNondominatedNeighbors (TNondominatedSet* pNondominatedNeighbors) {
	}

	virtual void GenerateNondominatedNeighborsNew(TNondominatedSet* pNondominatedNeighbors, TNondominatedSet* pDominatingNeighbors) {
	}

	virtual void Perturb(int Steps) {
	}
};

#endif // !defined(AFX_MOMHSolution_H__EE158131_9605_11D3_814C_000000000000__INCLUDED_)
