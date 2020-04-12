// Copyright (C) 2015-17 Andrzej Jaszkiewicz
#pragma once

#include "problem.h"
#include "global.h"
#include "solution.h"
#include "tlistset.h"
#include <algorithm>  

extern int maxBranches;
extern int maxListSize;

extern int nodesCalled;
extern int nodesTested;

//TComponent is a node
template <class TProblemSolution> class TComponent {
public:
	TListSet <TProblemSolution> listSet;
public:

	TPoint approximateIdealPoint;
	TPoint approximateNadirPoint;

	vector <TComponent<TProblemSolution>*> branches;

	TComponent <TProblemSolution>* parent = NULL;

	long numberOfSolutions() {
		if (listSet.size() > 0)
			return listSet.size();
		else {
			long sizeValue = 0;
			unsigned i;
			for (i = 0; i < branches.size(); i++)
				sizeValue += branches[i]->numberOfSolutions();
			return sizeValue;
		}
	}

	long numberOfNodes() {
		long sizeValue = 0;
		if (listSet.size() == 0)
			sizeValue = 1;
		unsigned i;
		for (i = 0; i < branches.size(); i++)
			sizeValue += branches[i]->numberOfNodes();
		return sizeValue;
	}

	long numberOfLeafs() {
		long sizeValue = 0;
		if (listSet.size() != 0)
			sizeValue = 1;
		unsigned i;
		for (i = 0; i < branches.size(); i++)
			sizeValue += branches[i]->numberOfNodes();
		return sizeValue;
	}

	void print(int level, fstream& stream) {
		string s;
		unsigned i;
		for (i = 0; i < level; i++)
			s += ' ';
		stream << s;
		unsigned j;
		stream << s << "Ideal ";

		for (j = 0; j < NumberOfObjectives; j++) {
			stream << approximateIdealPoint.ObjectiveValues[j] << ' ';
			if (approximateIdealPoint.ObjectiveValues[j] == 0)
				int a = 1;
		}
		stream << '\n';
		stream << s << "Nadir ";
		for (j = 0; j < NumberOfObjectives; j++) {
			stream << approximateNadirPoint.ObjectiveValues[j] << ' ';
			if (approximateNadirPoint.ObjectiveValues[j] == 0)
				int a = 1;
		}
		stream << '\n';

		if (listSet.size() > 0) {
			unsigned k;
			for (k = 0; k < listSet.size(); k++) {
				stream << s << " Leaf ";
				unsigned j;
				for (j = 0; j < NumberOfObjectives; j++)
					stream << listSet[k]->ObjectiveValues[j] << ' ';
				stream << '\n';
			}
		}
		else {
			for (j = 0; j < branches.size(); j++)
				branches[j]->print(level + 1, stream);
		}
	}

	void updateIdealNadir() {
		unsigned i;
		for (i = 0; i < branches.size(); i++) {
			if (branches[i]->listSet.size() == 0 && branches[i]->branches.size() == 0)
				continue;
			if (branches[i]->approximateIdealPoint.ObjectiveValues[0] == 0)
				int a = 1;
			if (i == 0) {
				approximateIdealPoint.ObjectiveValues = branches[i]->approximateIdealPoint.ObjectiveValues;
				approximateNadirPoint.ObjectiveValues = branches[i]->approximateNadirPoint.ObjectiveValues;
			}
			else {
				unsigned j;
				for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
					fpc++;
#endif // MFPC
					if (Objectives[j].ObjectiveType == _Max) {
						if (approximateIdealPoint.ObjectiveValues[j] < branches[i]->approximateIdealPoint.ObjectiveValues[j]) {
							approximateIdealPoint.ObjectiveValues[j] = branches[i]->approximateIdealPoint.ObjectiveValues[j];
						}
#ifdef MFPC
						fpc++;
#endif // MFPC
						if (approximateNadirPoint.ObjectiveValues[j] > branches[i]->approximateNadirPoint.ObjectiveValues[j]) {
							approximateNadirPoint.ObjectiveValues[j] = branches[i]->approximateNadirPoint.ObjectiveValues[j];
						}
					}
					else {
#ifdef MFPC
						fpc++;
#endif // MFPC
						if (approximateIdealPoint.ObjectiveValues[j] > branches[i]->approximateIdealPoint.ObjectiveValues[j]) {
							approximateIdealPoint.ObjectiveValues[j] = branches[i]->approximateIdealPoint.ObjectiveValues[j];
						}
#ifdef MFPC
						fpc++;
#endif // MFPC
						if (approximateNadirPoint.ObjectiveValues[j] < branches[i]->approximateNadirPoint.ObjectiveValues[j]) {
							approximateNadirPoint.ObjectiveValues[j] = branches[i]->approximateNadirPoint.ObjectiveValues[j];
						}
					}
				}
			}
		}
		if (parent != NULL)
			parent->updateIdealNadir();
	}

	void updateIdealNadir(TComponent <TProblemSolution>* node) {
		if (node->listSet.size() == 0 && node->branches.size() == 0)
			return;
		bool changed = false;
		unsigned j;
		for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (approximateIdealPoint.ObjectiveValues[j] > node->approximateIdealPoint.ObjectiveValues[j]) {
				approximateIdealPoint.ObjectiveValues[j] = node->approximateIdealPoint.ObjectiveValues[j];
				changed = true;
			}
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (approximateNadirPoint.ObjectiveValues[j] < node->approximateNadirPoint.ObjectiveValues[j]) {
				approximateNadirPoint.ObjectiveValues[j] = node->approximateNadirPoint.ObjectiveValues[j];
				changed = true;
			}
		}
		if (changed && parent != NULL)
			parent->updateIdealNadir(this);
	}

	double insertDistance(TPoint& Solution) {
		double s = 0;
		int iobj; for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
			double Range = Problem.IdealPoint->ObjectiveValues[iobj] - Problem.NadirPoint->ObjectiveValues[iobj];
			if (Range == 0)
				Range = 1;
			double center = (approximateIdealPoint.ObjectiveValues[iobj] + approximateNadirPoint.ObjectiveValues[iobj]) / 2.0;
            //normalization
			double s1 = (center - Solution.ObjectiveValues[iobj]) / Range;
			s += s1 * s1;
		}
		return s;
	}

	void splitByClustering() {
		int numberOfClusert = min(maxListSize + 1, maxBranches);//8

		vector <TProblemSolution*> seeds;

		int j;
		for (j = 0; j < numberOfClusert; j++) {
			branches.push_back(new TComponent(this));
			if (j == 0) {
				// Find solution furthest from all other solutions
				double maxDistance = 0;
				int maxIndex = 0;
				unsigned i;
				for (i = 0; i < listSet.size(); i++) {
					unsigned i2;
					double sumDistance = 0;
					for (i2 = 1; i2 < listSet.size(); i2++) {
						if (i != i2)
							sumDistance += listSet[i]->Distance(*(listSet[i2]), *(Problem.IdealPoint), *(Problem.NadirPoint));
					}
					if (i == 0) {
						maxDistance = sumDistance;
					}
					else {
#ifdef MFPC
						fpc++;
#endif // MFPC
						if (maxDistance < sumDistance) {
							maxDistance = sumDistance;
							maxIndex = i;
						}
					}
				}
				TProblemSolution* solution = (TProblemSolution*)(listSet[maxIndex]);
				branches[j]->add(*solution);
				delete listSet[maxIndex];
				listSet.erase(listSet.begin() + maxIndex);
			}
			else {
				// Find solution furthest from all other clusters
				double maxDistance;
				int maxIndex = 0;
				unsigned i;
				for (i = 0; i < listSet.size(); i++) {
					unsigned i2;
					double sumDistance = 0;
					for (i2 = 0; i2 < seeds.size(); i2++) {
						if (i != i2)
							sumDistance += listSet[i]->Distance(*(seeds[i2]), *(Problem.IdealPoint), *(Problem.NadirPoint));
					}
					if (i == 0) {
						maxDistance = sumDistance;
					}
					else {
#ifdef MFPC
						fpc++;
#endif // MFPC
						if (maxDistance < sumDistance) {
							maxDistance = sumDistance;
							maxIndex = i;
						}
					}
				}
				TProblemSolution* solution = (TProblemSolution*)(listSet[maxIndex]);
				branches[j]->add(*solution);
				delete listSet[maxIndex];
				listSet.erase(listSet.begin() + maxIndex);
			}
		}

		// Now add each solution to closest branch
		unsigned i;
		for (i = 0; i < listSet.size(); i++) {
			TProblemSolution* Solution = (TProblemSolution*)listSet[i];
			double minDistance = branches[0]->insertDistance(*Solution);
			int minIndex = 0;
			for (j = 1; j < branches.size(); j++) {
				double distanceValue = branches[j]->insertDistance(*Solution);
#ifdef MFPC
				fpc++;
#endif // MFPC
				if (minDistance > distanceValue) {
					minDistance = distanceValue;
					minIndex = j;
				}
			}
			branches[minIndex]->insert(*Solution);
		}
		listSet.DeleteAll();
		listSet.clear();
	}

	void add(TPoint& Solution) {
		listSet.Update(Solution);
		approximateIdealPoint = Solution;
		approximateNadirPoint = Solution;
	}

	void insert(TPoint& Solution) {
		if (listSet.size() > 0) {//a leaf
			listSet.Add(Solution);
			if (listSet.idealUpdated || listSet.nadirUpdated) {
				approximateIdealPoint = listSet.ApproximateIdealPoint;
				approximateNadirPoint = listSet.ApproximateNadirPoint;
				if (parent != NULL) {
					parent->updateIdealNadir(this);
				}
			}
			if (listSet.size() > maxListSize) {
				splitByClustering();
			}
		}
		else {
			if (branches.size() == 0) {
				add(Solution);
			}
			else {
				// Find closest branch
				double minDistance = branches[0]->insertDistance(Solution);
				int minIndex = 0;

				unsigned i;
				for (i = 1; i < branches.size(); i++) {
					double distanceValue = branches[i]->insertDistance(Solution);
#ifdef MFPC
					fpc++;
#endif // MFPC
					if (minDistance > distanceValue) {
						minDistance = distanceValue;
						minIndex = i;
					}
				}
				branches[minIndex]->insert(Solution);
			}
		}
	}

	virtual void Update(TPoint& Solution, bool& nondominated, bool& dominated, bool& dominating, bool& equal, 
		bool& added, bool& toInsert) {

		// Compare to approximateIdealPoint
		TCompare NadirComparisonResult = Solution.Compare(approximateNadirPoint);

		if (NadirComparisonResult == _Dominated || NadirComparisonResult == _EqualSol) {
			dominated = true;
			nondominated = false;
			return;
		}

		TCompare IdealComparisonResult = Solution.Compare(approximateIdealPoint);

		if (IdealComparisonResult == _Dominating || IdealComparisonResult == _EqualSol) {
			// if a leaf
			if (listSet.size() > 0) {
				listSet.DeleteAll();
			}
			else {
				int k;
				for (k = 0; k < branches.size(); k++)
					delete branches[k];
				branches.clear();
			}
			dominating = true;
			nondominated = false;
			toInsert = true;
			added = false;
			return;
		}
		else if (IdealComparisonResult == _Dominated || NadirComparisonResult == _Dominating) {
			// if a leaf
			if (listSet.size() > 0) {
				added = listSet.checkUpdate(Solution);
				if (listSet.wasDominated) {
					nondominated = false;
					dominated = true;
					return;
				}
				if (listSet.wasEqual) {
					nondominated = false;
					equal = true;
					return;
				}
				if (added) {
					added = false;
					if (listSet.wasDominating) {
						toInsert = true;
					}
					return;
				}
				else {
					nondominated = false;
					dominated = true;
					return;
				}
			}
			else {
				unsigned i;
				for (i = 0; i < branches.size() && !dominated && !added && ! equal; i++) {
					branches[i]->Update(Solution, nondominated, dominated, dominating, equal, 
						added, toInsert);
					if (branches[i]->listSet.size() == 0 && branches[i]->branches.size() == 0) {
                        //not leaf and dont have branches
						delete branches[i];
						branches.erase(branches.begin() + i);
						i--;
					}
				}
				return;
			}
		}
		// outside the box
		else {
			toInsert = true;
		}
	}

	ostream& Save(ostream& Stream) {
		unsigned i;
		if (listSet.size () > 0) {
			unsigned i;
			for (i = 0; i < listSet.size(); i++) {
				listSet[i]->Save(Stream);
				Stream << '\n';
			}
		}
		else {
			unsigned j;
			for (j = 0; j < branches.size(); j++)
				branches[j]->Save (Stream);
		}

		return Stream;
	}

	void saveToList(TListSet <TProblemSolution>& listSet, bool newOnly) {
		unsigned i;
		if (this->listSet.size() > 0) {
			unsigned i;
			for (i = 0; i < this->listSet.size(); i++) {
				if (!newOnly) {
					TProblemSolution* solution = new TProblemSolution(*((TProblemSolution*)(this->listSet[i])));
					listSet.push_back(solution);
				}
			}
		}
		else {
			unsigned j;
			for (j = 0; j < branches.size(); j++)
				branches[j]->saveToList(listSet, newOnly);
		}
	}


	TComponent(TComponent <TProblemSolution>* parentParam) {
		this->parent = parentParam;
	}

	~TComponent() {
		listSet.DeleteAll();
		listSet.clear();
		int k;
		for (k = 0; k < branches.size(); k++)
			delete branches[k];
		branches.clear();
	}
};

template <class TProblemSolution> class TTreeSet
{
protected:
public:
	TListSet <TProblemSolution> listSet;

	TComponent<TProblemSolution>* root = NULL;

	void saveToList() {
		unsigned i;
		for (i = 0; i < listSet.size(); i++)
			delete listSet[i];
		listSet.clear();
		root->saveToList(listSet, false);
	}
	
	long numberOfSolutions() {
		return root->numberOfSolutions();
	}

	TSolutionsSet allSolutions;

	virtual bool Update(TPoint& Solution) {

		bool nondominated = true;//nondominated to each other
		bool dominated = false;//dominated by solutions in root
		bool dominating = false;//dominates solutions in root
		bool added = false;
		bool toInsert = false;//if insert to the tree
		bool equal = false;
		bool result = false;

		if (root == NULL) {
			root = new TComponent <TProblemSolution>(NULL);
			root->add(Solution);
			result = true;
		}
		else {
			root->Update(Solution, nondominated, dominated, dominating, equal,
				added, toInsert);
			if ((nondominated && toInsert) || (dominating && toInsert)) {
				root->insert(Solution);
				result = true;
			}
			else if (nondominated) {
				root->insert(Solution);
				result = true;
			}
		}

		return result;
	}

	void Save(char* FileName) {
		fstream Stream(FileName, ios::out);
		root->Save(Stream);
		Stream.close();
	}


	virtual void DeleteAll() {
		delete root;
	}

	TTreeSet() {}

	~TTreeSet() {
		DeleteAll();
	};
};


