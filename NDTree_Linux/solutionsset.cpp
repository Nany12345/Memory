// Copyright (C) 2015-17 Andrzej Jaszkiewicz

#include "solutionsset.h"

void TSolutionsSet::DeleteAll () 
{
	unsigned int i; for (i = 0; i < size (); i++)
    {
		if ((*this)[i] != NULL)
        {
            delete (*this)[i];
            (*this)[i] = NULL;
        }
    }
	clear ();
}

void TSolutionsSet::UpdateIdealNadir()
{
	// For all points
	unsigned int i; for (i = 0; i < size (); i++) {
        if ((*this)[i] == NULL) {
            continue;
        }

        // If first point
		if (i == 0) {
			int iobj; for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
				ApproximateIdealPoint.ObjectiveValues [iobj] = (*this)[i]->ObjectiveValues [iobj];
				ApproximateNadirPoint.ObjectiveValues [iobj] = (*this)[i]->ObjectiveValues [iobj];
			}
		}
		else {
			int iobj; for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
				{
						if (ApproximateIdealPoint.ObjectiveValues [iobj] > (*this)[i]->ObjectiveValues [iobj])
							ApproximateIdealPoint.ObjectiveValues [iobj] = (*this)[i]->ObjectiveValues [iobj];
						if (ApproximateNadirPoint.ObjectiveValues [iobj] < (*this)[i]->ObjectiveValues [iobj])
							ApproximateNadirPoint.ObjectiveValues [iobj] = (*this)[i]->ObjectiveValues [iobj];
				}
			}
		}
	}
}

void TSolutionsSet::UpdateIdealNadir (TPoint& Solution) 
{	
	int iobj; for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
		{
				if (ApproximateIdealPoint.ObjectiveValues[iobj] > Solution.ObjectiveValues[iobj]) {
					ApproximateIdealPoint.ObjectiveValues[iobj] = Solution.ObjectiveValues[iobj];
					idealUpdated = true;
				}
				if ((ApproximateNadirPoint.ObjectiveValues[iobj] < Solution.ObjectiveValues[iobj]) || (size() == 0)) {
					ApproximateNadirPoint.ObjectiveValues[iobj] = Solution.ObjectiveValues[iobj];
					nadirUpdated = true;
				}
		}
	}
}

void TSolutionsSet::Save(char* FileName)
{
	fstream Stream (FileName, ios::out);

	unsigned int i; for (i = 0; i < size (); i++) {
        if ((*this)[i] == NULL) {
            continue;
        }

        (*this) [i]->Save (Stream);
		Stream << '\n';
		Stream.flush();
	}

	Stream.close ();
}

void TSolutionsSet::Load(char* FileName)
{
	fstream Stream (FileName, ios::in);
	while (Stream.rdstate () == ios::goodbit) {
		TPoint* Solution = new TPoint;

		Solution->Load (Stream);

		if (Stream.rdstate () == ios::goodbit) {
			push_back (Solution);
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
