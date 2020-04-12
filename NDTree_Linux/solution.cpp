// Copyright (C) 2015-17 Andrzej Jaszkiewicz


#include "solution.h"
//#include "problem.h"

long long TPoint::Comparisons;

ostream& TPoint::Save(ostream& Stream)  
{
int i;
  for (i = 0; i < NumberOfObjectives; i++) {
	 Stream << ObjectiveValues [i];
	 Stream << '\x09';
  }
  Stream << '\x09';
  return Stream;
}

TPoint& TPoint::operator = (TPoint& Point) {
	index = Point.index;
	ObjectiveValues.resize (NumberOfObjectives);
	int i; for (i = 0; i < NumberOfObjectives; i++) {
		ObjectiveValues[i] = Point.ObjectiveValues[i];
	}
	return *this;
}

istream& TPoint::Load(istream& Stream) {
	int i; for (i = 0; i < NumberOfObjectives; i++) {
		Stream >> ObjectiveValues [i];

		ObjectiveValues[i] = ObjectiveValues[i];
	}
	return Stream;
}

double TPoint::Distance (TPoint& Point, TPoint& IdealPoint, TPoint& NadirPoint) {
	double s = 0;
	int iobj; for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
#ifdef MFPC
//		fpc++;
#endif // MFPC
		double Range = IdealPoint.ObjectiveValues [iobj] - NadirPoint.ObjectiveValues [iobj];
		if (Range == 0)
			Range = 1;
		double s1 = (ObjectiveValues [iobj] - Point.ObjectiveValues [iobj]) / Range;
		s += s1 * s1;
	}
	return sqrt (s);
}

void TPoint::Display(){
    for(int i = 0;i < NumberOfObjectives; i++)
        cout<<ObjectiveValues[i]<<" ";
    cout<<endl;
}

double TPoint::squareDistance(TPoint& Point, TPoint& IdealPoint, TPoint& NadirPoint) {
	double s = 0;
	int iobj; for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
#ifdef MFPC
//		fpc++;
#endif // MFPC
		double Range = IdealPoint.ObjectiveValues[iobj] - NadirPoint.ObjectiveValues[iobj];
		if (Range == 0)
			Range = 1;
		double s1 = (ObjectiveValues[iobj] - Point.ObjectiveValues[iobj]) / Range;
		s += s1 * s1;
	}
	return s;
}

double TPoint::manhatanDistance(TPoint& Point) {
	double s = 0;
	int iobj; for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
#ifdef MFPC
//		fpc++;
#endif // MFPC
		double s1 = (ObjectiveValues[iobj] - Point.ObjectiveValues[iobj]);
		if (s1 < 0)
			s1 = -s1;
		s += s1;
	}
	return s;
}

TCompare TPoint::Compare(TPoint& Point) {
	Comparisons++;

	bool bBetter = false;
	bool bWorse = false;

	int i = 0;
	do {
#ifdef MFPC
		fpc++;
		fpc++;
#endif // MFPC
		if (ObjectiveValues[i] < Point.ObjectiveValues[i])
			bBetter = true;
		if (Point.ObjectiveValues[i] < ObjectiveValues[i])
			bWorse = true;
		i++;
	} while (!(bWorse && bBetter) && (i < NumberOfObjectives));
	if (bWorse) {
		if (bBetter) {
			return _Nondominated;
		}
		else {
			return _Dominated;
		}
	}
	else {
		if (bBetter) {
			return _Dominating;
		}
		else {
			return _EqualSol;
		}
	}
}

