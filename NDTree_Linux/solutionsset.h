// Copyright (C) 2015-17 Andrzej Jaszkiewicz

#if !defined(AFX_TSOLUTIONSSET_H__25978BD1_92C8_11D3_8149_000000000000__INCLUDED_)
#define AFX_TSOLUTIONSSET_H__25978BD1_92C8_11D3_8149_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "global.h"
#include "solution.h"

/**	Set of solutions 
*
*	It is just a set of pointers */
class TSolutionsSet : public vector<TPoint*>
{
public:
	/** Update nadir and ideal point values
	*
	* Method updates nadir and ideal point approximation.
	* New point is taken into consideration.
	*
	* @param Solution new solution added to nondominated set
	**/
	void UpdateIdealNadir(TPoint& Solution);

	bool idealUpdated;
	bool nadirUpdated;

	/** Tells how many nondominated sulutions is on list */
	int				iSetSize;

	/** Aproximate ideal point 
	*
	*	The field is set by UpdateIdealNadir () method such that
	*	each component is equal to the best value of the objective 
	*	in the set of solutions */
	TPoint ApproximateIdealPoint;

	/** Aproximate nadir point 
	*
	*	The field is set by UpdateIdealNadir () method such that
	*	each component is equal to the worst value of the objective 
	*	in the set of solutions */
	TPoint ApproximateNadirPoint;

	/** Saves the set of solutions */
	virtual void Save(char* FileName);

	/** Loads the set of solutions */
	virtual void Load(char* FileName);

	/** Updates ApproximateIdealPoint and ApproximateNadirPoint */
	void UpdateIdealNadir ();

	/* Deletes all solutions */
	virtual void DeleteAll ();

};

#endif // !defined(AFX_TSOLUTIONSSET_H__25978BD1_92C8_11D3_8149_000000000000__INCLUDED_)
