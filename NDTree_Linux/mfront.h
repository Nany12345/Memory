// Copyright (C) 2015-17 Andrzej Jaszkiewicz
#pragma once

#include "global.h"
#include "solution.h"
#include "problem.h"
#include <list>
#include <unordered_map>

class Element;

class KDNode;

class MList : public list <list<Element*>::iterator> {
};

class Iterators : public vector <MList::iterator> {
public:
	Iterators() {
		resize(NumberOfObjectives);
	}
};

class Element {
public:
	KDNode* kdNode;
	TPoint* solution;
	bool empty = false;
//	vector <MList::iterator> iterators;
	Element() {
//		iterators.resize(NumberOfObjectives);
	}
	~Element() {
		delete solution;
	}
};

class KDNode {
public:
	int depth;

	bool isLeaf;

	bool removed = false;

	list<Element*>::iterator solution;

	double splitValue;

	vector <double> min;
	vector <double> max;

	KDNode* lower = NULL;
	KDNode* greaterOrEqual = NULL;

	KDNode* parent;

	KDNode(list<Element*>::iterator newSolution, int parenthDepth, KDNode* parentNode) {
		solution = newSolution;
		depth = parenthDepth + 1;
		isLeaf = true;
		parent = parentNode;
		(**solution).kdNode = this;
		min.resize(NumberOfObjectives, -1e30);
		max.resize(NumberOfObjectives, 1e30);
	}

	~KDNode() {
		if (lower != NULL)
			delete lower;
		if (greaterOrEqual != NULL)
			delete greaterOrEqual;
	}

	void check() {
		assert(isLeaf || lower != NULL || greaterOrEqual != NULL);
		if (parent != NULL) {
			int j;
			for (j = 0; j < NumberOfObjectives; j++) {
				assert(min[j] >= parent->min[j]);
				assert(max[j] <= parent->max[j]);
			}
		}
		if (!isLeaf) {
			if (lower != NULL) {
				assert(lower->parent == this);
				lower->check();
			}
			if (greaterOrEqual != NULL) {
				assert(greaterOrEqual->parent == this);
				greaterOrEqual->check();
			}
		}
		if (isLeaf) {
			assert((**(this->solution)).kdNode == this);
			int j;
			for (j = 0; j < NumberOfObjectives; j++) {
				assert((**solution).solution->ObjectiveValues[j] >= min[j]);
				assert((**solution).solution->ObjectiveValues[j] <= max[j]);
			}
		}
	}

	void remove() {
		if (parent != NULL) {
			parent->remove(this);
		}
		else {
			removed = true;
		}
	}

	void remove(KDNode* son) {
		if (lower == son) {
			delete lower;
			lower = NULL;
			if (greaterOrEqual != NULL && greaterOrEqual->isLeaf) {
				isLeaf = true;
				solution = greaterOrEqual->solution;
				(**solution).kdNode = this;
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
				(**solution).kdNode = this;
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

	void add(list<Element*>::iterator newSolution) {
		int obj = depth % NumberOfObjectives;

		if (isLeaf) {
			isLeaf = false;
			splitValue = ((**newSolution).solution->ObjectiveValues[obj] +
				(**solution).solution->ObjectiveValues[obj]) / 2;

			// The solutions are not equal on obj
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (((**newSolution).solution->ObjectiveValues[obj] !=
				(**solution).solution->ObjectiveValues[obj])) {
#ifdef MFPC
				fpc++;
#endif // MFPC
				if ((**newSolution).solution->ObjectiveValues[obj] >= splitValue) {
					greaterOrEqual = new KDNode(newSolution, depth, this);
					lower = new KDNode(solution, depth, this);
				}
				else {
					greaterOrEqual = new KDNode(solution, depth, this);
					lower = new KDNode(newSolution, depth, this);
				}
				greaterOrEqual->setbox(false, obj, min, max, splitValue);
				lower->setbox(true, obj, min, max, splitValue);
			}
			// The solutions are equal on obj
			else {
				greaterOrEqual = new KDNode(solution, depth, this);
				greaterOrEqual->setbox(false, obj, min, max, splitValue);
				greaterOrEqual->add(newSolution);
			}

		}
		// if is not leaf
		else {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if ((**newSolution).solution->ObjectiveValues[obj] < splitValue) {
				if (lower != NULL)
					lower->add(newSolution);
				else {
					lower = new KDNode(newSolution, depth, this);
					lower->setbox(true, obj, min, max, splitValue);
				}
			}
			else {
				if (greaterOrEqual != NULL)
					greaterOrEqual->add(newSolution);
				else {
					greaterOrEqual = new KDNode(newSolution, depth, this);
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

	void checkNearer(list<Element*>::iterator& nearest, double& minDistance, TPoint& Solution) {
		if (distanceChecks > 4)
			return;
		int obj = depth % NumberOfObjectives;

		if (isLeaf) {
			distanceChecks++;
			double distance = (**solution).solution->squareDistance(Solution, *(Problem.IdealPoint), *(Problem.NadirPoint));
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

	list<Element*>::iterator nearest(double& minDistance, TPoint& Solution) {
		int obj = depth % NumberOfObjectives;

		if (isLeaf) {
			minDistance = (**solution).solution->squareDistance(Solution, *(Problem.IdealPoint), *(Problem.NadirPoint));
			return solution;
		}
		// if is not leaf
		else {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (Solution.ObjectiveValues[obj] < splitValue) {
				if (lower != NULL) {
					list<Element*>::iterator n = lower->nearest(minDistance, Solution);
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
					list<Element*>::iterator n = greaterOrEqual->nearest(minDistance, Solution);
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

class KDTree
{
public:
	KDNode* root = NULL;

public:
	void add(list<Element*>::iterator newSolution) {
		if (root == NULL) {
			root = new KDNode(newSolution, -1, NULL);
		}
		else if (root->removed) {
			delete root;
			root = new KDNode(newSolution, -1, NULL);
		}
		else {
			distanceChecks = 0;
			root->add(newSolution);
			//			root->check();
		}
	}

	~KDTree() {
		delete root;
	}
};


template <class TProblemSolution> class MFront 
{
protected:
	unordered_map<Element*, Iterators*> map;

	Iterators* iterators;

	list<Element*> solutions;

	KDTree* kdTree;

	vector <MList*> mlists;

	vector <list<Element*>::iterator> RSU;
	vector <list<Element*>::iterator> RSL;

	list<Element*>::iterator newSol;

	list<Element*>::iterator ref;

	Element* element;

	list<Element*>::iterator findCloseSolution(TPoint& Solution) {
//		return kdTree->root->nearest(Solution);

		list<Element*>::iterator it;
		list<Element*>::iterator minPosition = solutions.begin();
		double minDistance = (*(solutions.begin()))->solution->Distance(Solution, *(Problem.IdealPoint), *(Problem.NadirPoint));
		for (it = solutions.begin(); it != solutions.end(); it++) {
			double distance = (*it)->solution->Distance(Solution, *(Problem.IdealPoint), *(Problem.NadirPoint));
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (minDistance > distance) {
				minPosition = it;
				minDistance = distance;
			}
		}
		return minPosition;
	}

public:
	MFront() {
		kdTree = new KDTree();
	}
	~MFront() {
		int k;
		list<Element*>::iterator it;
		for (it = solutions.begin(); it != solutions.end(); it++) {
//			delete (*it);
		}

		unordered_map<Element*, Iterators*>::iterator it2;
		for (it2 = map.begin(); it2 != map.end(); it2++) {
			delete (*it2).first;
			delete (*it2).second;
		}

		solutions.clear();
		for (k = 0; k < mlists.size(); k++)
			delete mlists[k];
		mlists.clear();
		delete kdTree;
	}

	bool update1(TPoint& Solution) {
		TCompare result = (*ref)->solution->Compare(Solution);
		if (result == _Dominating || result == _EqualSol)
			return false;

		element = new Element;
		element->solution = new TProblemSolution(Solution);
		solutions.push_back(element);
		newSol = solutions.end();
		newSol--;
		iterators = new Iterators;
		map.emplace(element, iterators);


		RSU.clear();
		RSL.clear();

		int j;
		for (j = 0; j < NumberOfObjectives; j++) {
			Iterators* refIterators = map.find(*ref)->second;
			MList::iterator it = (*refIterators)[j];
#ifdef MFPC
			fpc++;
#endif // MFPC
			if ((*newSol)->solution->ObjectiveValues[j] == (*ref)->solution->ObjectiveValues[j]) {
				mlists[j]->insert(it, newSol);
				(*iterators)[j] = --it;
			}
			else {
#ifdef MFPC
				fpc++;
#endif // MFPC
				if ((*newSol)->solution->ObjectiveValues[j] < (*ref)->solution->ObjectiveValues[j]) {
					// increment it to the last position where f(it)==f(ref)
					it++;
					while ((it != (mlists[j])->end()) && ((*(*it))->solution->ObjectiveValues[j] == (*ref)->solution->ObjectiveValues[j])) {
#ifdef MFPC
						fpc++;
#endif // MFPC
						it++;
					}
					it--;
					while ((*(*it))->solution->ObjectiveValues[j] >= (*newSol)->solution->ObjectiveValues[j]) {
#ifdef MFPC
						fpc++;
#endif // MFPC
						RSL.push_back(*it);
						if (it == (mlists[j])->begin()) {
							break;
						}
						it--;
					}
					if ((*(*it))->solution->ObjectiveValues[j] < (*newSol)->solution->ObjectiveValues[j])
						it++;
					mlists[j]->insert(it, newSol);
					(*iterators)[j] = --it;
				}
				else {
#ifdef MFPC
					fpc++;
#endif // MFPC
					if ((*newSol)->solution->ObjectiveValues[j] > (*ref)->solution->ObjectiveValues[j]) {
						// decrement it to the last position where f(it)==f(ref)
						while ((it != (mlists[j])->begin()) && ((*(*it))->solution->ObjectiveValues[j] == (*ref)->solution->ObjectiveValues[j])) {
#ifdef MFPC
							fpc++;
#endif // MFPC
							it--;
						}
#ifdef MFPC
						fpc++;
#endif // MFPC
						if ((*(*it))->solution->ObjectiveValues[j] != (*ref)->solution->ObjectiveValues[j])
							it++;
						MList::iterator ita = it;
						while ((it != (mlists[j])->end()) && (*(*it))->solution->ObjectiveValues[j] <= (*newSol)->solution->ObjectiveValues[j]) {
#ifdef MFPC
							fpc++;
#endif // MFPC
							RSU.push_back(*it);
							it++;
						}
						mlists[j]->insert(it, newSol);
						(*iterators)[j] = --it;
					}
				}
			}
		}
		return true;
	}

	bool update2(TPoint& Solution) {
		int j;
		bool dominated = false;
		int i;
		for (i = 0; !dominated && (i < RSU.size()); i++) {
			TCompare result = (*(RSU[i]))->solution->Compare(Solution);
			if (result == _Dominating || result == _EqualSol) {
				dominated = true;
				for (j = 0; j < NumberOfObjectives; j++) {
					mlists[j]->erase((*iterators)[j]);
				}
				(*newSol)->empty = true;
			}
		}
		if (dominated) {
			return false;
		}

		vector <list<Element*>::iterator> R;
		for (i = 0; i < RSL.size(); i++) {
			TCompare result = (*(RSL[i]))->solution->Compare(Solution);
			if (result == _Dominated) {
				int l;
				bool found = false;
				for (l = 0; !found && (l < R.size()); l++)
					found = R[l] == RSL[i];
				if (!found)
					R.push_back(RSL[i]);
			}
		}

		vector <list<Element*>::iterator>::iterator itR;
		for (itR = R.begin(); itR != R.end(); itR++) {
			Iterators* iterators = map.find(**itR)->second;
			for (j = 0; j < NumberOfObjectives; j++) {
				mlists[j]->erase((*iterators)[j]);
			}
			(***itR).kdNode->remove();

			(**itR)->empty = true;
		}

		kdTree->add(newSol);

		return true;
	}

	virtual bool Update(TPoint& Solution) {
		if (solutions.size() == 0) {
			Element* element = new Element;
			element->solution = new TProblemSolution(Solution);
			solutions.push_back(element);
			kdTree->add(solutions.begin());
			iterators = new Iterators;
			map.emplace(element, iterators);
			int j;
			for (j = 0; j < NumberOfObjectives; j++) {
				MList* mlist = new MList;
				mlist->push_back(solutions.begin());
				(*iterators)[j] = mlist->begin();
				mlists.push_back(mlist);
			}
			return true;
		}

		double minDistance = 1e30;
		ref = kdTree->root->nearest (minDistance, Solution);
		if (!update1(Solution))
			return false;


		return update2(Solution);
	}

	long numberOfSolutions() {
		int n = 0;
		list<Element*>::iterator it;
		for (it = solutions.begin(); it != solutions.end(); it++) {
			if (!(*it)->empty)
				n++;
		}
		return n;
	}

	/*void check() {
		int j;
		for (j = 0; j < NumberOfObjectives; j++) {
			MList::iterator itl = mlists[j]->begin();
			while (itl != mlists[j]->end()) {
				MList::iterator itNext = itl;
				itNext++;
				if (itNext != mlists[j]->end())
					assert((**itl)->solution->ObjectiveValues[j] <= (**itNext)->solution->ObjectiveValues[j]);
				assert((**itl)->iterators[j] == itl);
				itl++;
			}
		}

		list<Element*>::iterator its = solutions.begin();
		while (its != solutions.end()) {
			for (j = 0; j < NumberOfObjectives; j++) {
				MList::iterator itl = (**its).iterators[j];
				assert((*itl) == its);
			}
			assert((**its).kdNode->solution == its);
			its++;
		}

		kdTree->root->check();

	}*/

};

