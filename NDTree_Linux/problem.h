// Copyright (C) 2015-17 Andrzej Jaszkiewicz

#ifndef __PROBLEM_H
#define __PROBLEM_H

#include "global.h"
#include "solution.h"

class TPoint;

// Abstract class. Generalization of all problems
class TProblem 
{
public:
	TPoint* IdealPoint;
	TPoint* NadirPoint;
	void setArtificialProblem(int numberOfObjectives) {
		NumberOfObjectives = numberOfObjectives;
		Objectives.resize(NumberOfObjectives);
		int iobj;
		for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
			Objectives[iobj].ObjectiveType = _Min;
		}
		IdealPoint = new TPoint();
		NadirPoint = new TPoint();
		int j;
		for (j = 0; j < NumberOfObjectives; j++) {
			NadirPoint->ObjectiveValues[j] = 0;
			IdealPoint->ObjectiveValues[j] = 0;
		}
	}
};                      

#endif  //__PROBLEM_H
