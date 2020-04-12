// Copyright (C) 2015-17 Andrzej Jaszkiewicz


#ifndef __SOLUTION_H
#define __SOLUTION_H

#include "global.h"

class TProblem;
extern TProblem Problem;


/** Possible relations between points in multiple objective space */
enum TCompare {_Dominating, _Dominated, _Nondominated, _EqualSol};  

/** Point in objective space */
class TPoint {
public:
public:
	int index;

	static long long Comparisons;

	/** Vector of objective values */
	vector<float> ObjectiveValues;

	/** Constructor */
	TPoint () {
		if (NumberOfObjectives == 0) {
			cout << "TPoint::TPoint ()\n";
			cout << "NumberOfObjectives == 0\n";
			exit (1);
		}
		ObjectiveValues.resize (NumberOfObjectives);
	}

	/** Copy constructor */
	TPoint (TPoint& Point) {
		index = Point.index;
		ObjectiveValues.resize (NumberOfObjectives);
		int i; for (i = 0; i < NumberOfObjectives; i++)
			ObjectiveValues [i] = Point.ObjectiveValues [i];
	}

	/** Copy operator */
	TPoint& operator = (TPoint& Point);

	bool IsBetterOnObjective (TPoint& Point, int iObjective) {
		if (Objectives [iObjective].ObjectiveType == _Max) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (ObjectiveValues [iObjective] > Point.ObjectiveValues [iObjective])
				return true;
			else
				return false;
		} 
		else {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (ObjectiveValues [iObjective] < Point.ObjectiveValues [iObjective])
				return true;
			else
				return false;
		}
	}

	/** Compares two points in multiple objective space
	*
	*	Returns _Dominated if this is dominated
	*	Returns _Dominating if this is dominating
	*	It may also return _Nondominated or _EqualSol */
	TCompare Compare(TPoint& Point);

	/** Calculates Euclidean distance between two points in the
	*	normalized objective space.
	*
	*	The objectives are normalized with the ranges between the 
	*	given ideal and nadir points. */
	double Distance (TPoint& Point, TPoint& IdealPoint, TPoint& NadirPoint);

    void Display();

	double squareDistance(TPoint& Point, TPoint& IdealPoint, TPoint& NadirPoint);

	double manhatanDistance(TPoint& Point);

	/** Reads the point from the stream */
    virtual istream& Load(istream& Stream);

	/** Saves objective values to an open Stream
	*
	*	Values are separated by TAB character */
    virtual ostream& Save(ostream& Stream);             
};

#endif
