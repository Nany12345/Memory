// Copyright (C) 2015-17 Andrzej Jaszkiewicz


#ifndef __TLISTSET_CPP_
#define __TLISTSET_CPP_

#include <algorithm>
#include "tlistset.h"

template <class TProblemSolution>
TListSet <TProblemSolution>::TListSet() : TSolutionsSet () 
{
};

template <class TProblemSolution>
bool TListSet <TProblemSolution>::UpdateBiObjective(TPoint& Solution)
{
	// Find last position where all <= positions are not better on the first objective
	int iLastNotBetter;
	
	// If the first solution is already better
	if ((*this)[0]->IsBetterOnObjective (Solution, 0))
		iLastNotBetter = -1;
	// If the last solution is not better
	else if (!(*this)[size() - 1]->IsBetterOnObjective (Solution, 0))
		iLastNotBetter = size() - 1;
	else {
		// Find last not better by bisection
		int first = 0; // not better
		int last = size() - 1; // ! not better - equal or better
		while (last - first > 1) {
			int middle = (last + first) / 2;
			if (!(*this)[middle]->IsBetterOnObjective (Solution, 0))
				first = middle;
			else
				last = middle;
		}
		iLastNotBetter = first;
	}

	// At this moment 0...iLastNotBetter could be worse or equal on objective 0or equal
	//   thus cannot dominat Solution
	// and iLastNotBetter+1... are better on objective 0
	//   thus cannot be dominated nor equal to solution


	// Check if the next solution is dominatig
	unsigned int iNext = iLastNotBetter + 1;
	if (iNext < size ()) {
		if (!Solution.IsBetterOnObjective (*(*this)[iNext], 1))
			return false;
	}

	// Check if there is an equal solution or dominating solution
	if (iLastNotBetter >= 0) {
		TCompare ComparisonResult = (*this)[iLastNotBetter]->Compare(Solution);
		if ((ComparisonResult == _EqualSol) || (ComparisonResult == _Dominating))
			return false;
	}
	//Check if some solutions are dominated and erase them
	if (iLastNotBetter >= 0) {
		bool DominatedFound = false;
		do {
			DominatedFound = false;
			if ((*this)[iLastNotBetter]->Compare(Solution) == _Dominated) {
				delete (*this)[iLastNotBetter];
				erase (begin() + iLastNotBetter);
				iLastNotBetter--;
				DominatedFound = true;
			}
		} while ((iLastNotBetter >= 0) && DominatedFound);
	}

	TProblemSolution* pPattern = (TProblemSolution*)&Solution;
	TProblemSolution* pNewSolution = new TProblemSolution(*pPattern);
	UpdateIdealNadir (*pNewSolution);
	insert (begin() + (iLastNotBetter + 1), pNewSolution);

	return true;
}

template <class TProblemSolution>
void TListSet <TProblemSolution>::Add(TPoint& Solution)
{
	idealUpdated = false;
	nadirUpdated = false;

	if (NumberOfObjectives == 2){
        //cout<< size() << endl;
		//UpdateBiObjective(Solution);
        //cout<< size() << endl;
        //exit(1);
		TProblemSolution* pPattern = (TProblemSolution*)&Solution;
		TProblemSolution* pNewSolution = new TProblemSolution(*pPattern);
		UpdateIdealNadir(*pNewSolution);
		push_back(pNewSolution);
    }
	else {
		TProblemSolution* pPattern = (TProblemSolution*)&Solution;
		TProblemSolution* pNewSolution = new TProblemSolution(*pPattern);
		UpdateIdealNadir(*pNewSolution);
		push_back(pNewSolution);
	}
}

template <class TProblemSolution>
bool TListSet <TProblemSolution>::Update(TPoint& Solution)
{
	equal = false;
	wasEqual = false;
	wasDominated = false;
	wasDominating = false;

	idealUpdated = false;
	nadirUpdated = false;

	updates++;

	bool bEqual, bDominated, bDominating;
	
	bool bAdded = false;

	if (size () == 0) {
		bAdded = true;
		TProblemSolution* pPattern = (TProblemSolution *)&Solution;
		TProblemSolution* pNewSolution = new TProblemSolution(*pPattern);
		push_back(pNewSolution);
		UpdateIdealNadir ();
	}
	else {
		if (NumberOfObjectives == 2 && useSortedList)
			bAdded = UpdateBiObjective (Solution);
		else {
			bEqual = bDominated = bDominating = false;

			unsigned int i; 
			for (i = 0; (i < size ()) && !bEqual && !bDominated ; i++) {

				TCompare ComparisonResult = Solution.Compare (*(*this)[i]);

				switch (ComparisonResult) {
			case _Dominating:
				delete (*this)[i];
				erase (begin() + i);
				i--;
				bDominating = true;
				break;
			case _Dominated:
			{
				bDominated = true;
			}
				break;
			case _Nondominated:
				break;
			case _EqualSol:
				bEqual = true;
				break;
				}
			}

			if (bDominated && bDominating) {
				// Exception
				cout << Solution.ObjectiveValues [0] << "  " << Solution.ObjectiveValues [1] << "  " ;
				cout << "Exception\n";
				cout << "void TListSet::Update (TPoint& Point)\n";
				cout << "bDominated && bDominating\n";
				exit (0);
			}

			if (!bDominated && !bEqual) {
				TProblemSolution* pPattern = (TProblemSolution*)&Solution;
				TProblemSolution* pNewSolution = new TProblemSolution(*pPattern);
				push_back (pNewSolution);
				bAdded = true;
			}
			if (bDominated) {
				wasDominated = true;
				dominated++;
			}
			else if (bDominating)
				dominating++;
			else if (bEqual) {
				equal++;
				wasEqual = true;
			}
			else
				nondominated++;

		}
	}

	iSetSize = size();

	return bAdded;
}

template <class TProblemSolution>
bool TListSet <TProblemSolution>::checkUpdate(TPoint& Solution)
{
	equal = false;
	wasEqual = false;
	wasDominating = false;
	wasDominated = false;

	bool bEqual, bDominated, bDominating;
	bool bAdded = false;

	if (size() == 0) {
		bAdded = true;
	}
	else {
		bEqual = bDominated = bDominating = false;
		unsigned int i; for (i = 0; (i < size()) && !bEqual && !bDominated; i++) {
			TCompare ComparisonResult = Solution.Compare(*(*this)[i]);
			switch (ComparisonResult) {
			case _Dominating://this point dominates a solution in the list
				UpdateNadir(i);
				delete (*this)[i];
				erase(begin() + i);
				i--;
				wasDominating = true;
				bDominating = true;
				break;
			case _Dominated://this point is dominated
			{
				wasDominated = true;
				bDominated = true;
			}
			break;
			case _Nondominated:
				break;
			case _EqualSol:
				bEqual = true;
				break;
			}
		}

		if (bDominated && bDominating) {
			// Exception
			cout << Solution.ObjectiveValues[0] << "  " << Solution.ObjectiveValues[1] << "  ";
			cout << "Exception\n";
			cout << "void TListSet::Update (TPoint& Point)\n";
			cout << "bDominated && bDominating\n";
			exit(0);
		}

		if (!bDominated && !bEqual) {//not dominated by list and not equal to list
			bAdded = true;
		}
		if (bEqual) {
			wasEqual = true;
		}
	}

	return bAdded;
}

template <class TProblemSolution>
void TListSet <TProblemSolution>::UpdateNadir(int iRemovedSolution) {
	int iobj; for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
		{
			if ((*this)[iRemovedSolution]->ObjectiveValues [iobj] == ApproximateNadirPoint.ObjectiveValues [iobj]) {
				bool bFirst = true;
				unsigned int i; for (i = 0; i < size (); i++) {
					if (i != iRemovedSolution) {
						if (bFirst) {
							ApproximateNadirPoint.ObjectiveValues [iobj] = (*this)[i]->ObjectiveValues [iobj];
							bFirst = false;
						}
						else {
							if (Objectives [iobj].ObjectiveType == _Max) {
								if (ApproximateNadirPoint.ObjectiveValues [iobj] > (*this)[i]->ObjectiveValues [iobj])
									ApproximateNadirPoint.ObjectiveValues [iobj] = (*this)[i]->ObjectiveValues [iobj];
							}
							if (Objectives [iobj].ObjectiveType == _Min) {
								if (ApproximateNadirPoint.ObjectiveValues [iobj] < (*this)[i]->ObjectiveValues [iobj])
									ApproximateNadirPoint.ObjectiveValues [iobj] = (*this)[i]->ObjectiveValues [iobj];
							}
						}
					}
				}
			}
		}
	}
}

template <class TProblemSolution>
void TListSet <TProblemSolution>::DeleteAll()
{
	TSolutionsSet::DeleteAll();

	iSetSize = 0;
};

 
template <class TProblemSolution>
void TListSet <TProblemSolution>::GetRandomSolution(TPoint* &pSolution)
{
    int iIndex = 0;
	// old code
	if (iSetSize <= 0) {
		pSolution = NULL;
	} else {
		iIndex = rand() % iSetSize;
		pSolution = (TPoint*)(*this)[iIndex];
	}
};

template <class TProblemSolution>
void TListSet <TProblemSolution>::RemoveDominating (TProblemSolution& Solution) {
	if (NumberOfObjectives != 2 )
		return;

	// Find last position where all <= positions are not better on the first objective
	int iLastNotBetter;
	
	// If the first solution is already better
	if ((*this)[0]->IsBetterOnObjective (Solution, 0))
		iLastNotBetter = -1;
	// If the last solution is not better
	else if (!(*this)[size() - 1]->IsBetterOnObjective (Solution, 0))
		iLastNotBetter = size() - 1;
	else {
		// Find last not better by bisection
		int first = 0; // not better
		int last = size() - 1; // ! not better - equal or better
		while (last - first > 1) {
			int middle = (last + first) / 2;
			if (!(*this)[middle]->IsBetterOnObjective (Solution, 0))
				first = middle;
			else
				last = middle;
		}
		iLastNotBetter = first;
	}

	// At this moment 0...iLastNotBetter could be worse or equal on objective 0or equal
	//   thus cannot dominat Solution
	// and iLastNotBetter+1... are better on objective 0
	//   thus cannot be dominated nor equal to solution


	// Check if the next solution is dominatig
	unsigned int iNext = iLastNotBetter + 1;
	bool DominatingFound = false;
	do {
		DominatingFound = false;
		if (iNext < size ()) {
			if (!Solution.IsBetterOnObjective (*(*this)[iNext], 1)) {
				DominatingFound = true;
				erase (begin() + iNext);
			}
		}
	} while (DominatingFound && (iNext < size ()));

	// Check if there is an equal solution or dominating solution
	if (iLastNotBetter >= 0) {
		TCompare ComparisonResult = (*this)[iLastNotBetter]->Compare(Solution);
		if (ComparisonResult == _EqualSol) 
			return;
		else if (ComparisonResult == _Dominating){
			erase (begin() + iLastNotBetter);
			iLastNotBetter--;
		}
	}

	//Check if some solutions are dominated and erase them
	if (iLastNotBetter >= 0) {
		bool DominatedFound = false;
		do {
			DominatedFound = false;
			if ((*this)[iLastNotBetter]->Compare(Solution) == _Dominated) {
				delete (*this)[iLastNotBetter];
				erase (begin() + iLastNotBetter);
				iLastNotBetter--;
				DominatedFound = true;
			}
		} while ((iLastNotBetter >= 0) && DominatedFound);
	}

	TProblemSolution* pPattern = (TProblemSolution*)&Solution;
	TProblemSolution* pNewSolution = new TProblemSolution(*pPattern);
	UpdateIdealNadir (*pNewSolution);
	insert (begin() + (iLastNotBetter + 1), pNewSolution);
}

template <class TProblemSolution>
bool TListSet <TProblemSolution>::before(TPoint* solution1, TPoint* solution2) {
	int j;
	for (j = 0; j < NumberOfObjectives; j++) {
#ifdef MFPC
		fpc++;
#endif // MFPC
		if (solution1->ObjectiveValues[j] < solution2->ObjectiveValues[j])
			return true;
#ifdef MFPC
		fpc++;
#endif // MFPC
		if (solution1->ObjectiveValues[j] > solution2->ObjectiveValues[j])
			return false;
	}
	return false;
}

template <class TProblemSolution>
void TListSet <TProblemSolution>::sortLex() {
	std::sort(begin(), end(), before);
}

template <class TProblemSolution>
bool TListSet <TProblemSolution>::equalSets(TListSet <TProblemSolution>& otherSet) {
	if (size() != otherSet.size())
		return false;

	bool setsEqual = true;
	unsigned i;
	for (i = 0; i < size() && setsEqual; i++) {
		setsEqual = (*this)[i]->Compare(*(otherSet[i])) == _EqualSol;
	}
	return setsEqual;
}


template <class TProblemSolution>
void TListSet <TProblemSolution>::Load(char* FileName)
{
	fstream Stream (FileName, ios::in);

	int i = 0;
	while (Stream.rdstate () == ios::goodbit && size () < 100000) {
		i++;
		TProblemSolution* Solution = new TProblemSolution;

		Solution->Load (Stream);

		if (Stream.rdstate () == ios::goodbit) {
			push_back(Solution);
			// Read the rest of the line
			char c;
			do {
				Stream.get (c);
			}
			while (c != '\n');
		}
		else
			delete Solution;

	}

	Stream.close ();
}



#endif
