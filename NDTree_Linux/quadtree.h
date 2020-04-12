// Copyright (C) 2015-17 Andrzej Jaszkiewicz
#pragma once

#include "global.h"
#include "solution.h"
#include "tlistset.h"
#include <algorithm>  

extern int maxK;
extern bool added;

template <class TProblemSolution> class TNode {
private:
	TProblemSolution solution;
	vector <int> solutionVector;
	vector <TNode<TProblemSolution>*> branches;
	TNode <TProblemSolution>* parent = NULL;
	bool empty = false;

public:

	~TNode() {
		int i;
		for (i = 0; i < branches.size(); i++)
			delete branches[i];
		branches.clear();
	}


	long numberOfSolutions() {
		long sizeValue = 1;
		unsigned i;
		for (i = 1; i < branches.size() - 1; i++)
			if (branches[i] != NULL)
				sizeValue += branches[i]->numberOfSolutions();
		return sizeValue;
	}

	void print(int level, fstream& stream) {

		string s;
		unsigned i;
		for (i = 0; i < level; i++)
			s += ' ';
		stream << s;
		unsigned j;
		for (j = 0; j < NumberOfObjectives; j++) {
			stream << this->solutionVector[j] << ' ';
		}
		stream << '\n';
		stream << s;
		for (j = 0; j < NumberOfObjectives; j++)
			stream << solution.ObjectiveValues[j] << ' ';
		stream << '\n';
		for (j = 0; j < branches.size(); j++)
			if (branches[j] != NULL)
				branches[j]->print(level + 1, stream);
	}

	void replace(TProblemSolution& solution) {
		TNode <TProblemSolution>* copyNode = new TNode <TProblemSolution>(this->solution, NULL);
		int i;
		copyNode->branches = branches;

		branches.clear();
		branches.resize(maxK, NULL);
		this->solution = solution;
		added = true;

		for (i = 1; i < maxK - 1; i++) {
			if (copyNode->branches[i] != NULL) {
				reconsider(copyNode->branches[i]);
			}
		}

		delete copyNode;
	}

	void reconsider(TNode<TProblemSolution>* node) {
		int i;
		for (i = 1; i < maxK - 1; i++) {
			if (node->branches[i] != NULL) {
				reconsider(node->branches[i]);
			}
		}

		unsigned j;
		int power = 1;
		vector <int> newVector;
		newVector.resize(NumberOfObjectives);
		bool wasBetter = false;
		bool wasWorse = false;

		TPoint::Comparisons++;

		int newK = 0;
		for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (node->solution.ObjectiveValues[j] > this->solution.ObjectiveValues[j]) {
				wasWorse = true;
			}
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (node->solution.ObjectiveValues[j] < this->solution.ObjectiveValues[j]) {
				newVector[j] = 0;
				wasBetter = true;
			}
			else {
				newVector[j] = 1;
				newK += power;
			}
			power *= 2;
		}

		if (wasWorse && !wasBetter) {
//			cout << "wasWorse && !wasBetter \n";
			return;
		}

		if (!node->empty && wasBetter && wasWorse) {
			if (branches[newK] == NULL) {
				branches[newK] = new TNode(node->solution, this, newVector);
				node->empty = true;
				added = true;
			}
			else {
				branches[newK]->reinsert(node, this->solution);
			}
		}
	}

	void reinsert(TNode<TProblemSolution>* node, TProblemSolution& newSolution) {
		int i;
		for (i = 1; i < maxK - 1; i++) {
			if (node->branches[i] != NULL) {
				reinsert(node->branches[i], newSolution);
			}
		}

		if (node->empty)
			return;

		TPoint::Comparisons++;
		unsigned j;
		bool wasBetter = false;
		bool wasWorse = false;
		for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (node->solution.ObjectiveValues[j] > newSolution.ObjectiveValues[j]) {
				wasWorse = true;
			}
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (node->solution.ObjectiveValues[j] < newSolution.ObjectiveValues[j]) {
				wasBetter = true;
			}
		}
		if (wasWorse && !wasBetter)
			return;

		TPoint::Comparisons++;
		int power = 1;
		vector <int> newVector;
		newVector.resize(NumberOfObjectives);
		int newK = 0;
		for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (node->solution.ObjectiveValues[j] < this->solution.ObjectiveValues[j]) {
				newVector[j] = 0;
			}
			else {
				newVector[j] = 1;
				newK += power;
			}
			power *= 2;
		}

		if (!node->empty && wasBetter && wasWorse) {
			if (branches[newK] == NULL) {
				branches[newK] = new TNode(node->solution, this, newVector);
				node->empty = true;
				added = true;
			}
			else {
				branches[newK]->reinsert(node, newSolution);
			}
		}
	}


	virtual void Update(TProblemSolution& solution, bool& dominated, vector <TNode<TProblemSolution>*>& reInsertNodes) {

		TPoint::Comparisons++;

		bool wasBetter = false;
		bool wasWorse = false;
		vector <int> newVector;
		newVector.resize(NumberOfObjectives);
		int newK = 0;
		unsigned j;
		int power = 1;
		for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (solution.ObjectiveValues[j] > this->solution.ObjectiveValues[j]) {
				wasWorse = true;
			}
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (solution.ObjectiveValues[j] < this->solution.ObjectiveValues[j]) {
				newVector[j] = 0;
				wasBetter = true;
			}
			else {
				newVector[j] = 1;
				newK += power;
			}
			power *= 2;
		}

		if (newK == maxK - 1) {
			dominated = true;
			return;
		}
		else
			if (newK == 0 || (wasBetter && !wasWorse)) {
				// new solution dominates this
				// delete y and its subtree, /* y is dominated by x */
				// replace y by x and reinsert the subtree of y from x
				replace(solution);

				return;
			}
			else {
				int i;
				for (i = 1; !dominated && (i < newK); i++) {
					if (branches[i] != NULL) {
						bool toCheck = true;
						TPoint::Comparisons++;
						for (j = 0; toCheck && (j < NumberOfObjectives); j++) {
#ifdef MFPC
							fpc++;
#endif // MFPC
							toCheck = newVector[j] >= branches[i]->solutionVector[j];
						}
						if (toCheck)
							branches[i]->test1(solution, dominated);
					}
				}

				for (i = newK + 1; i < maxK - 1; i++) {
					if (branches[i] != NULL) {
						bool toCheck = true;
						TPoint::Comparisons++;
						for (j = 0; toCheck && (j < NumberOfObjectives); j++) {
#ifdef MFPC
							fpc++;
#endif // MFPC
							toCheck = newVector[j] <= branches[i]->solutionVector[j];
						}
						if (toCheck) {
							bool dominating = false;
							branches[i]->test2(solution, dominating, reInsertNodes);
							if (dominating) {
								branches[i]->deleteSubtree(solution);
								if (branches[i]->empty) {
									delete branches[i];
									branches[i] = NULL;
								}
							}
						}
					}
				}
			}

			if (!dominated) {
				if (branches[newK] != NULL) {
					branches[newK]->Update(solution, dominated, reInsertNodes);
				}
				else {
					branches[newK] = new TNode(solution, this, newVector);
					added = true;
				}
			}
	}

	void reInsert(TNode <TProblemSolution>* node) {
		int i;
		for (i = 1; i < maxK - 1; i++) {
			if (node->branches[i] != NULL) {
				bool dominated = false;
				vector <TNode<TProblemSolution>*> reInsertNodes;
				Update(node->branches[i]->solution, dominated, reInsertNodes);
				assert(reInsertNodes.size() == 0);
				reInsert(node->branches[i]);
			}
		}
	}

	void reInsertFromRoot(TNode <TProblemSolution>* node) {
		// find root
		TNode <TProblemSolution>* root = this;
		while (root->parent != NULL)
			root = root->parent;

		root->reInsert(node);
	}

	void test1(TProblemSolution& solution, bool& dominated) {

		TPoint::Comparisons++;
		bool isEqual = true;
		unsigned j;
		for (j = 0; isEqual && (j < NumberOfObjectives); j++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			isEqual = solution.ObjectiveValues[j] == this->solution.ObjectiveValues[j];
		}
		if (isEqual)
			return;

		TPoint::Comparisons++;
		vector <int> newVector;
		newVector.resize(NumberOfObjectives);
		int newK = 0;
		int power = 1;
		for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (solution.ObjectiveValues[j] < this->solution.ObjectiveValues[j])
				newVector[j] = 0;
			else {
				newVector[j] = 1;
				newK += power;
			}
			power *= 2;
		}

		if (newK == maxK - 1) {
			dominated = true;
			return;
		}

		int i;
		for (i = 1; i < maxK - 1; i++) {
			if (branches[i] != NULL) {
#ifdef MFPC
				fpc++;
#endif // MFPC
				TPoint::Comparisons++;
				bool toCheck = true;
				for (j = 0; toCheck && (j < NumberOfObjectives); j++) {
#ifdef MFPC
					fpc++;
#endif // MFPC
					toCheck = newVector[j] >= branches[i]->solutionVector[j];
				}
				if (toCheck)
					branches[i]->test1(solution, dominated);
			}
		}

	}

	TProblemSolution* findSubstitute(TProblemSolution& newSolution) {
		bool found = false;
		int i;
		for (i = 1; i < maxK - 1 && !found; i++) {
			if (branches[i] != NULL) {
				if (!found) {
					if (!newSolution.Compare((branches[i]->solution)) == _Dominating) {
						found = true;
						return &(branches[i]->solution);
					}
				}
			}
		}
		for (i = 1; i < maxK - 1 && !found; i++) {
			if (branches[i] != NULL) {
				TProblemSolution* tempSolution = branches[i]->findSubstitute(newSolution);
				if (tempSolution != NULL) {
					found = true;
					return tempSolution;
				}
			}
		}
		return NULL;
	}

	void deleteSubtree(TProblemSolution& newSolution) {
		TNode <TProblemSolution>* copyNode = new TNode <TProblemSolution>(this->solution, NULL);
		copyNode->branches = branches;
		branches.clear();
		branches.resize(maxK, NULL);

		TProblemSolution* substitute = copyNode->findSubstitute(newSolution);
		if (substitute == NULL) {
			empty = true;
			return;
		}
		else {
			solution = *substitute;
		}

		int i;
		for (i = 1; i < maxK - 1; i++) {
			if (copyNode->branches[i] != NULL) {
				reinsert(copyNode->branches[i], newSolution);
			}
		}

		delete copyNode;
	}

	void test2(TProblemSolution& solution, bool& dominating, vector <TNode<TProblemSolution>*>& reInsertNodes) {

		bool wasBetter = false;
		bool wasWorse = false;

		TPoint::Comparisons++;
		vector <int> newVector;
		newVector.resize(NumberOfObjectives);
		unsigned j;
		for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (solution.ObjectiveValues[j] > this->solution.ObjectiveValues[j]) {
				wasWorse = true;
			}
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (solution.ObjectiveValues[j] < this->solution.ObjectiveValues[j]) {
				newVector[j] = 0;
				wasBetter = true;
			}
			else {
				newVector[j] = 1;
			}
		}

		if (wasBetter && !wasWorse) {
			dominating = true;
			return;
		}

		int i;
		for (i = 1; i < maxK - 1; i++) {
#ifdef MFPC
			fpc++;
#endif // MFPC
			if (branches[i] != NULL) {
				TPoint::Comparisons++;
				bool toCheck = true;
				for (j = 0; toCheck && (j < NumberOfObjectives); j++) {
#ifdef MFPC
					fpc++;
#endif // MFPC
					toCheck = newVector[j] <= branches[i]->solutionVector[j];
				}
				if (toCheck) {
					bool dominating = false;
					branches[i]->test2(solution, dominating, reInsertNodes);
					if (dominating) {
						branches[i]->deleteSubtree(solution);
						if (branches[i]->empty) {
							delete branches[i];
							branches[i] = NULL;
						}
				

					}
				}
			}
		}

	}

	ostream& Save(ostream& Stream) {
		solution.Save(Stream);
		Stream << '\n';
		unsigned j;
		for (j = 1; j < branches.size() - 1; j++)
			if (branches[j] != NULL)
				branches[j]->Save(Stream);
		return Stream;
	}

	void saveToList(TListSet <TProblemSolution>& listSet) {
		TProblemSolution* tempSolution = new TProblemSolution(solution);
		listSet.push_back(tempSolution);
		unsigned j;
		for (j = 1; j < branches.size() - 1; j++)
			if (branches[j] != NULL)
				branches[j]->saveToList(listSet);
	}

	bool checkTree() {
		bool proper = true;
		unsigned i;
		for (i = 1; i < branches.size() - 1 && proper; i++)
			if (branches[i] != NULL) {
				int power = 1;
				int newK = 0;
				TPoint::Comparisons++;
				unsigned j;
				for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
					fpc++;
#endif // MFPC
					if (branches[i]->solution.ObjectiveValues[j] >= this->solution.ObjectiveValues[j]) {
						newK += power;
					}
					power *= 2;
				}
				proper = proper && (newK == i);
				if (!proper)
					int a = 1;

				proper = proper && branches[i]->checkTree ();
			}
		return proper;
	}

	TNode(TProblemSolution& solution, TNode <TProblemSolution>* parentParam, vector <int> solutionVector) {
		this->solutionVector.resize(NumberOfObjectives);
		this->solutionVector = solutionVector;
		this->solution = solution;
		int numberOfBranches = maxK;
		branches.resize(numberOfBranches, NULL);
		this->parent = parentParam;
	}

	TNode(TProblemSolution& solutionParam, TNode <TProblemSolution>* parentParam) {
		solutionVector.resize(NumberOfObjectives);
		this->solution = solutionParam;
		int numberOfBranches = maxK;
		branches.resize(numberOfBranches, NULL);
		this->parent = parentParam;
	}

};

template <class TProblemSolution> class QuadTree
{
protected:
public:
	TNode<TProblemSolution>* root = NULL;

	TListSet <TProblemSolution> listSet;

	long numberOfSolutions() {
		return root->numberOfSolutions();
	}

	virtual bool Update(TProblemSolution& Solution) {
		if (root == NULL) {
			root = new TNode <TProblemSolution>(Solution, NULL);
			return true;
		}

		bool dominated = false;
		vector <TNode<TProblemSolution>*> reInsertNodes;

		added = false;
		root->Update(Solution, dominated, reInsertNodes);
		unsigned i;
		for (i = 0; i < reInsertNodes.size(); i++) {
			root->reInsert(reInsertNodes[i]);
		}
		return added;
	}

	void Save(char* FileName) {
		fstream Stream(FileName, ios::out);
		root->Save(Stream);
		Stream.close();
	}

	void saveToList() {
		unsigned i;
		for (i = 0; i < listSet.size(); i++)
			delete listSet[i];
		listSet.clear();
		root->saveToList(listSet);
	}

	virtual void DeleteAll() {
		delete root;
	}

	QuadTree() {
		maxK = pow(2, NumberOfObjectives);
	}

	~QuadTree() {
		DeleteAll();
	};
};


