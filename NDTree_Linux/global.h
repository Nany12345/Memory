// Copyright (C) 2015-17 Andrzej Jaszkiewicz


#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <string>

using namespace std;

#include <assert.h>

extern long long fpc; // FloatingPointCOmparisons

//#define MFPC // Measure fpc

extern int distanceChecks;

extern int NumberOfObjectives;

/** Possible types of objectives minized or maximized */
enum TObjectiveType {_Min, _Max};

/** Description of an objective */
class TObjective {
public:
	/** Type of the objective minized or maximized */
	TObjectiveType ObjectiveType;

};

extern vector <TObjective> Objectives;

extern unsigned int NumberOfConstraints;

/** Possible types of constraints*/
enum TConstraintType {_EqualTo, _LowerThan, _GreaterThan};

/** Description of a constraint */
class TConstraint {
public:
	TConstraint () {
		EqualityPrecision = 0;
	}

	/** Type of the objective minized or maximized */
	TConstraintType ConstraintType;

	/** Level to which the constraint should be equal, lower than or 
	*	greater than depending on its type */
	double Threshold;

	/** Precision of equalities
	*
	*	Applies only to constraints of type _EqualTo
	*	Such constraint is feasible if its value is within the range Threshold +/- EqualityPrecision */
	double EqualityPrecision;
};

extern vector <TConstraint> Constraints;

/** Type - vector of integers **/
typedef	vector<int>					IntVector;

/** Type - vector of longs **/
typedef	vector<long>					LongVector;

/** Type - vector of double **/
typedef	vector<double>					DoubleVector;

/** Type - vector of bool **/
typedef	vector<bool>					BoolVector;

/** Type - vector of bytes **/
typedef	vector<unsigned char>			ByteVector;

/** Type - vector of characters **/
typedef	vector<char>					CharVector;

#endif //__GLOBAL_H
