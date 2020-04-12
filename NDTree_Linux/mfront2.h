// Copyright (C) 2015-17 Andrzej Jaszkiewicz
#pragma once

#include "global.h"
#include "solution.h"
#include <list>
#include <unordered_set>

class Element2;

class KDNode2;

class Element2 {
public:
	KDNode2* kdNode;
	TPoint* solution;
	bool empty = false;
	Element2() {
	}
	~Element2() {
		delete solution;
	}
};

class MList2 : public vector <Element2*> {
public:
	int find(Element2* ref, int obj) {
		int first = 0;
		int last = size() - 1;
#ifdef MFPC
		fpc++;
#endif // MFPC
		if ((*this)[first] == ref)
			return first;
#ifdef MFPC
		fpc++;
#endif // MFPC
		if ((*this)[last] == ref)
			return last;
		while (last - first > 1) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			int middle = (first + last) / 2;
#ifdef MFPC
			fpc++;
#endif // MFPC
			if ((*this)[middle] == ref)
				return middle;

#ifdef MFPC
			fpc++;
#endif // MFPC

			if ((ref)->solution->ObjectiveValues[obj] < (*(*this)[middle]).solution->ObjectiveValues[obj])
				last = middle;
			else {
#ifdef MFPC
				fpc++;
#endif // MFPC
				if ((ref)->solution->ObjectiveValues[obj] > (*(*this)[middle]).solution->ObjectiveValues[obj])
					first = middle;
				else {
					int m1 = middle;
					m1--;
					while (m1 >= 0 && (ref)->solution->ObjectiveValues[obj] == (*(*this)[m1]).solution->ObjectiveValues[obj]) {
#ifdef MFPC
						fpc++;
#endif // MFPC
#ifdef MFPC
						fpc++;
#endif // MFPC
						if ((*this)[m1] == ref)
							return m1;
						m1--;
					}
					m1 = middle;
					m1++;
					while ((ref)->solution->ObjectiveValues[obj] == (*(*this)[m1]).solution->ObjectiveValues[obj]) {
#ifdef MFPC
						fpc++;
						fpc++;
#endif // MFPC
						if ((*this)[m1] == ref)
							return m1;
						m1++;
					}
					cout << "Not found 2\n";
					exit(0);
				}
			}
		}
		cout << "Not found\n";
		exit(0);
	}

	void remove(Element2*sol, int obj) {
		int pos = find(sol, obj);
		erase(begin() + pos);
	}
};

class KDNode2 {
public:
	int depth;

	bool isLeaf;

	bool removed = false;
	
	Element2* solution;

	double splitValue;

	vector <double> min;
	vector <double> max;

	KDNode2* lower = NULL;
	KDNode2* greaterOrEqual = NULL;

	KDNode2* parent;

	KDNode2(Element2* newSolution, int parenthDepth, KDNode2* parentNode) {
		solution = newSolution;
		depth = parenthDepth + 1;
		isLeaf = true;
		parent = parentNode;
		(*solution).kdNode = this;
		min.resize(NumberOfObjectives, -1e30);
		max.resize(NumberOfObjectives, 1e30);
	}

	~KDNode2() {
		if (lower != NULL)
			delete lower;
		if (greaterOrEqual!= NULL)
			delete greaterOrEqual;
	}

	void remove() {
		if (parent != NULL) {
			parent->remove(this);
		}
		else {
			removed = true;
		}
	}

	void remove(KDNode2* son) {
		if (lower == son) {
			delete lower;
			lower = NULL;
			if (greaterOrEqual != NULL && greaterOrEqual->isLeaf) {
				isLeaf = true;
				solution = greaterOrEqual->solution;
				(*solution).kdNode = this;
				delete greaterOrEqual;
				greaterOrEqual = NULL;
			}
		}
		else if (greaterOrEqual == son) {
			delete greaterOrEqual;
			greaterOrEqual = NULL;
			if (lower != NULL && lower->isLeaf) {
				isLeaf = true;
				solution = lower->solution;
				(*solution).kdNode = this;
				delete lower;
				lower = NULL;
			}
		} 
		else {
			cout << "son not found\n";
			exit(0);
		}
		if (!isLeaf && lower == NULL && greaterOrEqual == NULL && parent != NULL)
			parent->remove(this);
	}

	void setbox(bool lower, int j, vector <double>& min, vector <double>& max, double splitValue) {
		this->min = min;
		this->max = max;
		if (lower) {
			this->max[j] = splitValue;
		}
		else {
			this->min[j] = splitValue;
		}
	}

	void add(Element2* newSolution) {
		int obj = depth % NumberOfObjectives;

		if (isLeaf) {
			isLeaf = false;
			splitValue = ((*newSolution).solution->ObjectiveValues[obj] +
				(*solution).solution->ObjectiveValues[obj]) / 2;

			// The solutions are not equal on obj
			if (((*newSolution).solution->ObjectiveValues[obj] !=
				(*solution).solution->ObjectiveValues[obj])) {
#ifdef MFPC
				fpc++;
#endif // MFPC
				if ((*newSolution).solution->ObjectiveValues[obj] >= splitValue) {
					greaterOrEqual = new KDNode2(newSolution, depth, this);
					lower = new KDNode2(solution, depth, this);
				}
				else {
					greaterOrEqual = new KDNode2(solution, depth, this);
					lower = new KDNode2(newSolution, depth, this);
				}
				greaterOrEqual->setbox(false, obj, min, max, splitValue);
				lower->setbox(true, obj, min, max, splitValue);
			}
			// The solutions are equal on obj
			else {
				greaterOrEqual = new KDNode2(solution, depth, this);
				greaterOrEqual->setbox(false, obj, min, max, splitValue);
				greaterOrEqual->add(newSolution);
			}

		}
		// if is not leaf
		else {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if ((*newSolution).solution->ObjectiveValues[obj] < splitValue) {
				if (lower != NULL)
					lower->add(newSolution);
				else {
					lower = new KDNode2(newSolution, depth, this);
					lower->setbox(true, obj, min, max, splitValue);
				}
			}
			else {
				if (greaterOrEqual != NULL)
					greaterOrEqual->add(newSolution);
				else {
					greaterOrEqual = new KDNode2(newSolution, depth, this);
					greaterOrEqual->setbox(false, obj, min, max, splitValue);
				}
			}
		}
	}

	double boxDistance(TPoint& Solution, double minDistance) {
		double distance = 0;
		int j;
		for (j = 0; (j < NumberOfObjectives) && (distance < minDistance); j++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			double Range = Problem.IdealPoint->ObjectiveValues[j] - Problem.NadirPoint->ObjectiveValues[j];
			if (Range == 0)
				Range = 1;
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (Solution.ObjectiveValues[j] < min[j]) {
				double d = (Solution.ObjectiveValues[j] - min[j]) / Range;
				distance += d * d;
			}
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (Solution.ObjectiveValues[j] > max[j]) {
				double d = (Solution.ObjectiveValues[j] - max[j]) / Range;
				distance += d * d;
			}
		}
		return distance;
	}

	void checkNearer(Element2*& nearest, double& minDistance, TPoint& Solution) {
		if (distanceChecks > 4)
			return;

		int obj = depth % NumberOfObjectives;

		if (isLeaf) {
			distanceChecks++;
			double distance = (*solution).solution->squareDistance(Solution, *(Problem.IdealPoint), *(Problem.NadirPoint));
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (distance < minDistance) {
				minDistance = distance;
				nearest = solution;
			}
		}
		// if is not leaf
		else {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (boxDistance(Solution, minDistance) < minDistance) {
#ifdef MFPC
				fpc++;
#endif // MFPC
				if (Solution.ObjectiveValues[obj] < splitValue) {
					if (lower != NULL) {
						lower->checkNearer(nearest, minDistance, Solution);
						if (greaterOrEqual != NULL)
							greaterOrEqual->checkNearer(nearest, minDistance, Solution);
					}
				}
				else {
					if (greaterOrEqual != NULL) {
						greaterOrEqual->checkNearer(nearest, minDistance, Solution);
						if (lower != NULL)
							lower->checkNearer(nearest, minDistance, Solution);
					}
				}
			}
		}
	}

	Element2* nearest(double& minDistance, TPoint& Solution) {
		int obj = depth % NumberOfObjectives;

		if (isLeaf) {
			minDistance = (*solution).solution->squareDistance(Solution, *(Problem.IdealPoint), *(Problem.NadirPoint));
			return solution;
		}
		// if is not leaf
		else {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (Solution.ObjectiveValues[obj] < splitValue) {
				if (lower != NULL) {
					Element2* n = lower->nearest(minDistance, Solution);
					if (greaterOrEqual != NULL) {
						greaterOrEqual->checkNearer(n, minDistance, Solution);
					}
					return n;
				}
				else {
					return greaterOrEqual->nearest(minDistance, Solution);
				}
			}
			else {
				if (greaterOrEqual != NULL) {
					Element2* n = greaterOrEqual->nearest(minDistance, Solution);
					if (lower != NULL) {
						lower->checkNearer(n, minDistance, Solution);
					}
					return n;
				}
				else {
					return lower->nearest(minDistance, Solution);
				}
			}
		}
	}
};

class KDTree2 
{
public:
	KDNode2* root = NULL;

public:
	void add(Element2* newSolution) {
		if (root == NULL) {
			root = new KDNode2(newSolution, -1, NULL);
		}
		else if (root->removed) {
			delete root;
			root = new KDNode2(newSolution, -1, NULL);
		}
		else {
			distanceChecks = 0;
			root->add(newSolution);
//			root->check();
		}
	}

	~KDTree2() {
		delete root;
	}
};

template <class TProblemSolution> class MFront2
{
public:
	clock_t kdTreeRunningTime = 0;

	vector<Element2*> solutions;

protected:
	KDTree2* kdTree;

	vector <MList2*> mlists;

	Element2* newSol;

	Element2* ref;

	Element2* findCloseSolution(TPoint& Solution) {
		vector<Element2*>::iterator it;
		vector<Element2*>::iterator minPosition = solutions.begin();
		double minDistance = (*(solutions.begin()))->solution->squareDistance(Solution, *(Problem.IdealPoint), *(Problem.NadirPoint));
		for (it = solutions.begin(); it != solutions.end(); it++) {
//			if (rand() % 10 != 0)
//				continue;
			double distance = (*it)->solution->squareDistance(Solution, *(Problem.IdealPoint), *(Problem.NadirPoint));
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (minDistance > distance) {
				minPosition = it;
				minDistance = distance;
			}
		}
		return (*minPosition);
	}

public:
	MFront2() {
		kdTree = new KDTree2();
	}
	~MFront2() {
		int k;
		for (k = 0; k < solutions.size(); k++)
			delete solutions[k];
		solutions.clear();
		for (k = 0; k < mlists.size(); k++)
			delete mlists[k];
		mlists.clear();
		delete kdTree;
	}

	void findNear(TPoint& Solution) {
		double minDistance = 1e30;
		ref = kdTree->root->nearest(minDistance, Solution);
	}

	bool update2(TPoint& Solution) {
		TCompare result = (ref)->solution->Compare(Solution);
		if (result == _Dominating || result == _EqualSol)
			return false;

		Element2* element = new Element2;
		element->solution = new TProblemSolution(Solution);
		solutions.push_back(element);
		newSol = element;

		bool dominated = false;
		bool dominating = false;

		bool refDominated = false;

		vector <bool> objVisited;
		objVisited.resize(NumberOfObjectives, false);
		int j;
		for (j = 0; j < NumberOfObjectives && !dominated; j++) {
			int curSol = mlists[j]->find(ref, j);
#ifdef MFPC
			fpc++;
#endif // MFPC
			if ((newSol)->solution->ObjectiveValues[j] >(ref)->solution->ObjectiveValues[j]) {
				objVisited[j] = true;
				MList2* curList = mlists[j];
				// decrement it to the last position where f(it)==f(ref)
				while ((curSol != 0) && (((*curList)[curSol]))->solution->ObjectiveValues[j] == (ref)->solution->ObjectiveValues[j]) {
#ifdef MFPC
					fpc++;
#endif // MFPC
					curSol--;
				}
#ifdef MFPC
				fpc++;
#endif // MFPC
				if ((((*curList)[curSol]))->solution->ObjectiveValues[j] != (ref)->solution->ObjectiveValues[j])
					curSol++;
				double newSolValue = (newSol)->solution->ObjectiveValues[j];
				int size = curList->size();
				TPoint* curSolution = (((*curList)[curSol]))->solution;
				while ((curSol != size) && curSolution->ObjectiveValues[j] <= newSolValue) {
#ifdef MFPC
					fpc++;
#endif // MFPC
					if (!dominated && !dominating) {
						TCompare result = curSolution->Compare(Solution);
						if (result == _Dominating || result == _EqualSol) {
							dominated = true;
							int j1;
							for (j1 = 0; j1 < j; j1++) {
								if (objVisited[j1])
									mlists[j1]->remove(newSol, j1);
							}
							delete element;
							solutions.pop_back();
							return false;
						}
					}
					curSol++;
					if (curSol != size)
						curSolution = ((*curList)[curSol])->solution;
				}
				mlists[j]->insert(mlists[j]->begin() + curSol, newSol);
				//				check(j);
			}
		}
		for (j = 0; j < NumberOfObjectives && !dominated; j++) {
			int curSol = mlists[j]->find(ref, j);
			MList2* curList = mlists[j];
#ifdef MFPC
			fpc++;
#endif // MFPC
			if ((newSol)->solution->ObjectiveValues[j] == (ref)->solution->ObjectiveValues[j]) {
				mlists[j]->insert(mlists[j]->begin() + curSol, newSol);
			}
			else {
#ifdef MFPC
				fpc++;
#endif // MFPC
				if ((newSol)->solution->ObjectiveValues[j] < (ref)->solution->ObjectiveValues[j]) {
					// increment it to the last position where f(it)==f(ref)
					curSol++;
					while ((curSol != (mlists[j])->size()) && ((((*curList)[curSol]))->solution->ObjectiveValues[j] == (ref)->solution->ObjectiveValues[j])) {
#ifdef MFPC
						fpc++;
#endif // MFPC
						curSol++;
					}
					curSol--;
					while ((((*curList)[curSol]))->solution->ObjectiveValues[j] >= (newSol)->solution->ObjectiveValues[j]) {
#ifdef MFPC
						fpc++;
#endif // MFPC
						TCompare result = (((*curList)[curSol]))->solution->Compare(Solution);
						bool finish = false;
						int curSola = curSol;
						if (curSol == 0) {
							finish = true;
						}
						else
							curSol--;
						if (result == _Dominated) {
							dominating = true;
							Element2* sol = (*curList)[curSola];
							if (sol != ref) {
								int j1;
								for (j1 = 0; j1 < NumberOfObjectives; j1++) {
									mlists[j1]->remove(sol, j1);
								}
								(*sol).kdNode->remove();
								sol->empty = true;
							}
							else
								refDominated = true;
						}
						if (finish) {
							break;
						}
					}
#ifdef MFPC
					fpc++;
#endif // MFPC
					if ((((*curList)[curSol]))->solution->ObjectiveValues[j] < (newSol)->solution->ObjectiveValues[j])
						curSol++;
					mlists[j]->insert(mlists[j]->begin() + curSol, newSol);
				}
			}
		}

		if (refDominated) {
			int j1;
			for (j1 = 0; j1 < NumberOfObjectives; j1++) {
				mlists[j1]->remove(ref, j1);
			}
			(*ref).kdNode->remove();
			ref->empty = true;
		}

		if (dominated) {
			int j1;
			for (j1 = 0; j1 < NumberOfObjectives; j1++) {
				if (objVisited[j1])
					mlists[j1]->remove(newSol, j1);
			}
			delete element;
			solutions.pop_back();
			return false;
		}

		clock_t t0 = clock();

		kdTree->add(newSol);

		kdTreeRunningTime += clock() - t0;
	}

	virtual bool Update(TPoint& Solution) {
		if (solutions.size() == 0) {
			Element2* element = new Element2;
			element->solution = new TProblemSolution(Solution);
			solutions.push_back(element);
			kdTree->add(solutions[0]);
			int j;
			for (j = 0; j < NumberOfObjectives; j++) {
				MList2* mlist = new MList2;
				mlist->push_back(solutions[0]);
				mlists.push_back(mlist);
			}
			return true;
		}

		clock_t t0 = clock();

		findNear(Solution);

		kdTreeRunningTime += clock() - t0;

		return update2(Solution);
	}

	long numberOfSolutions() {
		int size = 0;
		int i;
		for (i = 0; i < solutions.size(); i++) {
			if (!solutions[i]->empty)
				size++;
		}
		return size;
	}

};

