// Copyright (C) 2015-17 Andrzej Jaszkiewicz

#include "problem.h"
#include "quadtree.h"
#include "ttreeset.h"
#include "mfront.h"
#include "mfront2.h"
#include <sstream>
#include <chrono>


TProblem Problem;

vector <TPoint*> NDS_solutions;

/*inline int sprintf_s(char* buffer, size_t sizeOfBuffer, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(buffer, sizeOfBuffer, format, ap);
    va_end(ap);
    return result;
}

template<size_t sizeOfBuffer>
inline int sprintf_s(char (&buffer)[sizeOfBuffer], const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(buffer, sizeOfBuffer, format, ap);
    va_end(ap);
    return result;
}*/

bool frontsEqual(vector<TPoint*>& front1, vector<TPoint*>& front2) {
	int i;
	for (i = 0; i < front1.size(); i++) {
		std::vector<TPoint*>::iterator it;

		it = find(front2.begin(), front2.end(), front1[i]);
		if (it == front2.end()) {
			return false;
		}
	}
	for (i = 0; i < front2.size(); i++) {
		std::vector<TPoint*>::iterator it;

		it = find(front1.begin(), front1.end(), front2[i]);
		if (it == front1.end()) {
			return false;
		}
	}
	return true;
}

bool frontsSetEqual(vector<vector<TPoint*>>& fronts1, vector<vector<TPoint*>>& fronts2) {
	if (fronts1.size() != fronts2.size()) {
		return false;
	}

	int i;
	for (i = 0; i < fronts1.size(); i++) {
		if (!frontsEqual(fronts1[i], fronts2[i])) {
			return false;
		}
	}

	return true;
}

int sortingObjective;

bool better(TPoint* s1, TPoint* s2) {
	return (s1->ObjectiveValues[sortingObjective] < s2->ObjectiveValues[sortingObjective]);
}

bool worse(TPoint* s1, TPoint* s2) {
	return (s1->ObjectiveValues[sortingObjective] > s2->ObjectiveValues[sortingObjective]);
}

int DDA_NS(vector<vector<TPoint*>>& fronts) {
	vector <TPoint*> unsortedSolutions;
	unsortedSolutions = NDS_solutions;

	vector<vector<vector<int>>> C_matrix;

	vector<vector<int>> D_matrix;

	int i;
	for (i = 0; i < NDS_solutions.size(); i++) {
		NDS_solutions[i]->index = i;
	}

	C_matrix.resize(NumberOfObjectives);
	int j;
	for (j = 0; j < NumberOfObjectives; j++) {
		C_matrix[j].resize(NDS_solutions.size());
		for (i = 0; i < NDS_solutions.size(); i++) {
			C_matrix[j][i].resize(NDS_solutions.size(), 0);
		}
	}

	for (j = 0; j < NumberOfObjectives; j++) {
		sortingObjective = j;
		std::sort(NDS_solutions.begin(), NDS_solutions.end(), worse);

		int b1_row = NDS_solutions[0]->index;
		for (i = 0; i < NDS_solutions.size(); i++) {
			C_matrix[j][b1_row][i] = 1;
		}

		for (i = 1; i < NDS_solutions.size(); i++) {
			int bi_row = NDS_solutions[i]->index;
			if (NDS_solutions[i]->ObjectiveValues[j] == NDS_solutions[i - 1]->ObjectiveValues[j]) {
				int bi_1_row = NDS_solutions[i - 1]->index;
				C_matrix[j][bi_row] = C_matrix[j][bi_1_row];
			}
			else {
				int i2;
				for (i2 = i; i2 < NDS_solutions.size(); i2++) {
					int bi2_row = NDS_solutions[i2]->index;
					C_matrix[j][bi_row][bi2_row] = 1;
				}
			}
		}
	}

	D_matrix.resize(NDS_solutions.size());
	for (i = 0; i < NDS_solutions.size(); i++) {
		D_matrix[i].resize(NDS_solutions.size(), 0);
	}

	for (j = 0; j < NumberOfObjectives; j++) {
		for (i = 0; i < NDS_solutions.size(); i++) {
			int i2;
			for (i2 = 0; i2 < NDS_solutions.size(); i2++) {
				D_matrix[i][i2] += C_matrix[j][i][i2];
			}
		}
	}

	for (i = 0; i < NDS_solutions.size(); i++) {
		D_matrix[i][i] = 0;
	}

	vector<int> M_vector;
	M_vector.resize(NDS_solutions.size(), -1);
	int count = 0;
	while (true) {
		std::fill(M_vector.begin(), M_vector.end(), -1);
		vector<TPoint*> front;
		for (i = 0; i < NDS_solutions.size(); i++) {
			if (D_matrix[i][i] >= 0) {
				int i2;
				for (i2 = 0; i2 < NDS_solutions.size(); i2++) {
					if (M_vector[i] < D_matrix[i][i2]) {
						M_vector[i] = D_matrix[i][i2];
					}
				}
			}
		}

		int newCount = 0;
		for (i = 0; i < NDS_solutions.size(); i++) {
			if (M_vector[i] >= 0 && M_vector[i] < NumberOfObjectives) {
				front.push_back(unsortedSolutions[i]);
				newCount++;
				D_matrix[i][i] = -1;
				int i2;
				for (i2 = 0; i2 < NDS_solutions.size(); i2++) {
					D_matrix[i2][i] = -1;
				}
			}
		}

		count += newCount;
		fronts.push_back(front);

		if (count == NDS_solutions.size()) {
			break;
		}
	}

	return fronts.size();
}

bool frontDominates(vector <TPoint*> front, TPoint* solution) {
	int i;
	for (i = front.size() - 1; i >= 0; i--) {
		TCompare comparisonResult = front[i]->Compare(*solution);
        
		if (comparisonResult == _Dominating) {
			return true;
		}
	}
	return false;
}

int ENS_BS(vector<vector<TPoint*>>& fronts) {
	// Sort solutions according to the first objective
	sortingObjective = 0;
	std::sort(NDS_solutions.begin(), NDS_solutions.end(), better);

	// Sort lexicographically
	for (sortingObjective = 1; sortingObjective < NumberOfObjectives; sortingObjective++) {
		int sortStart = 0;
		int i;
		for (i = 1; i < NDS_solutions.size(); i++) {
			if (NDS_solutions[i]->ObjectiveValues[sortingObjective - 1] != NDS_solutions[i - 1]->ObjectiveValues[sortingObjective - 1]) {
				if (i - sortStart > 1) {
					std::sort(NDS_solutions.begin() + sortStart, NDS_solutions.begin() + i, better);
				}
				sortStart = i;
			}
			else {
				int a = 1;
			}
		}
	}

	vector<TPoint*> front0;
	front0.push_back(NDS_solutions[0]);
	fronts.push_back(front0);

	int i;
	for (i = 1; i < NDS_solutions.size(); i++) {
		if (NDS_solutions[i]->ObjectiveValues[0] == 2901)
			int a = 1;
		int minFront = 0;
		int maxFront = fronts.size();
		while (true) {
			if (maxFront - minFront == 0) {
				if (maxFront == fronts.size()) {
					vector<TPoint*> front;
					fronts.push_back(front);
				}
				fronts[maxFront].push_back(NDS_solutions[i]);
				break;
			}
			if (maxFront - minFront == 1) {
				if (frontDominates(fronts[minFront], NDS_solutions[i])) {
					if (maxFront == fronts.size()) {
						vector<TPoint*> front;
						fronts.push_back(front);
					}
					fronts[maxFront].push_back(NDS_solutions[i]);
					break;
				}
				else {
					fronts[minFront].push_back(NDS_solutions[i]);
					break;
				}
			}
			else {
				int middleFront = (maxFront + minFront) / 2;
				if (frontDominates(fronts[middleFront], NDS_solutions[i])) {
					minFront = middleFront + 1;
				}
				else {
					maxFront = middleFront;
				}
			}
		}
	}

	return fronts.size();
}

int ENS_SS(vector<vector<TPoint*>>& fronts) {
	// Sort solutions according to the first objective
	sortingObjective = 0;
	std::sort(NDS_solutions.begin(), NDS_solutions.end(), better);

	// Sort lexicographically
	for (sortingObjective = 1; sortingObjective < NumberOfObjectives; sortingObjective++) {
		int sortStart = 0;
		int i;
		for (i = 1; i < NDS_solutions.size(); i++) {
			if (NDS_solutions[i]->ObjectiveValues[sortingObjective - 1] != NDS_solutions[i - 1]->ObjectiveValues[sortingObjective - 1]) {
				if (i - sortStart > 1) {
					std::sort(NDS_solutions.begin() + sortStart, NDS_solutions.begin() + i, better);
				}
				sortStart = i;
			}
			else {
				int a = 1;
			}
		}
	}

	vector<TPoint*> front0;
	front0.push_back(NDS_solutions[0]);
	fronts.push_back(front0);

	int i;
	for (i = 1; i < NDS_solutions.size(); i++) {
		int k;
		for (k = 0; k < fronts.size(); k++) {
			if (!frontDominates(fronts[k], NDS_solutions[i])) {
				fronts[k].push_back(NDS_solutions[i]);
				break;
			}
		}
		if (k == fronts.size()) {
			vector<TPoint*> front;
			front.push_back(NDS_solutions[i]);
			fronts.push_back(front);
		}
	}

	return fronts.size();
}

int ENS_SS_ND(vector<TPoint*>& fronts) {
	// Sort solutions according to the first objective
	sortingObjective = 0;
	std::sort(NDS_solutions.begin(), NDS_solutions.end(), better);

	// Sort lexicographically
	for (sortingObjective = 1; sortingObjective < NumberOfObjectives; sortingObjective++) {
		int sortStart = 0;
		int i;
		for (i = 1; i < NDS_solutions.size(); i++) {
			if (NDS_solutions[i]->ObjectiveValues[sortingObjective - 1] != NDS_solutions[i - 1]->ObjectiveValues[sortingObjective - 1]) {
				if (i - sortStart > 1) {
					std::sort(NDS_solutions.begin() + sortStart, NDS_solutions.begin() + i, better);
				}
				sortStart = i;
			}
			else {
				int a = 1;
			}
		}
	}

	fronts.push_back(NDS_solutions[0]);
	int i;
	for (i = 1; i < NDS_solutions.size(); i++) {
        if (!frontDominates(fronts, NDS_solutions[i])) {
            fronts.push_back(NDS_solutions[i]);
        }
	}

	return fronts.size();
}


int NDS_NDTree(vector<vector<TPoint*>>& fronts) {
	int frontsNum = 0;

	vector <TPoint*> solutions;
	int i;
	for (i = 0; i < NDS_solutions.size(); i++) {
		solutions.push_back(NDS_solutions[i]);
	}

	while (solutions.size() > 0) {
		frontsNum++;
		TTreeSet <TPoint> treeSet;

		int l;
		for (l = 0; l < solutions.size(); l++) {
			solutions[l]->index = l;
			bool treeAdded = treeSet.Update(*((TPoint*)(solutions[l])));
		}
		treeSet.saveToList();
		vector <TPoint*> front;
		for (l = 0; l < treeSet.listSet.size(); l++) {
			front.push_back(solutions[treeSet.listSet[l]->index]);
			solutions[treeSet.listSet[l]->index]->index = -1;
		}
		fronts.push_back(front);

		for (l = 0; l < solutions.size(); l++) {
			if (solutions[l]->index == -1) {
				solutions.erase(solutions.begin() + l);
				l--;
			}
		}

	}

	return frontsNum;
}

int NDS_MFront2(vector<vector<TPoint*>>& fronts) {
	int frontsNum = 0;

	vector <TPoint*> solutions;
	int i;
	for (i = 0; i < NDS_solutions.size(); i++) {
		solutions.push_back(NDS_solutions[i]);
	}

	while (solutions.size() > 0) {
		frontsNum++;
		MFront2 <TPoint> mFront2;

		int l;
		for (l = 0; l < solutions.size(); l++) {
			solutions[l]->index = l;
			bool treeAdded = mFront2.Update(*((TPoint*)(solutions[l])));
		}
		vector <TPoint*> front;
		for (l = 0; l < mFront2.solutions.size(); l++) {
			front.push_back(solutions[mFront2.solutions[l]->solution->index]);
			solutions[mFront2.solutions[l]->solution->index]->index = -1;
		}
		fronts.push_back(front);

		for (l = 0; l < solutions.size(); l++) {
			if (solutions[l]->index == -1) {
				solutions.erase(solutions.begin() + l);
				l--;
			}
		}

	}

	return frontsNum;
}

int NDS_List(vector<vector<TPoint*>>& fronts) {
	int frontsNum = 0;

	vector <TPoint*> solutions;
	int i;
	for (i = 0; i < NDS_solutions.size(); i++) {
		solutions.push_back(NDS_solutions[i]);
	}

	while (solutions.size() > 0) {
		frontsNum++;
		TListSet <TPoint> list;

		int l;
		for (l = 0; l < solutions.size(); l++) {
			solutions[l]->index = l;
			bool treeAdded = list.Update(*((TPoint*)(solutions[l])));
		}
		vector <TPoint*> front;
		for (l = 0; l < list.size(); l++) {
			front.push_back(solutions[list[l]->index]);
			solutions[list[l]->index]->index = -1;
		}
		fronts.push_back(front);

		for (l = 0; l < solutions.size(); l++) {
			if (solutions[l]->index == -1) {
				solutions.erase(solutions.begin() + l);
				l--;
			}
		}

	}

	return frontsNum;
}

void ndsTest(int max_points) {
	char buffer[1024];
	int j;
	for (j = 2; j <= 10; j++) {
		int iter;
		for (iter = 0; iter < 10; iter++) {
			Problem.setArtificialProblem(j);
			int i;
			for (i = 3; i <= 3; i++) {
				// Random populations
//				sprintf_s(buffer, "Uniform\\uniform%d_%d", j, iter + 1);
				// Our populations
				if (j < 7)
					snprintf(buffer, 1024, "DataValMax10000\\ArtSet%d_200000_%d_10000.txt", j, i);
				else
					snprintf(buffer, 1024, "DataValMax10000\\ArtSet%d_100000_%d.txt", j, i);

				TListSet <TPoint> allSolutions;
				allSolutions.Load(buffer);
				cout << allSolutions.size() << '\n';

				int l;

				srand(iter);
				for (l = 0; l < allSolutions.size(); l++) {
					TPoint* sol = allSolutions[l];
					int rpos = rand() % allSolutions.size();
					allSolutions[l] = allSolutions[rpos];
					allSolutions[rpos] = sol;
				}


				int k;
				for (k = max_points; k <= max_points; k += 1) {
					cout << k << ' ';
					NDS_solutions.clear();
					for (l = 0; l < k; l++) {
						bool equal = false;
						int ll;
						for (ll = 0; ll < NDS_solutions.size(); ll++) {
							equal = NDS_solutions[ll]->Compare(*(allSolutions[l])) == _EqualSol;
							if (equal)
								break;
						}
						if (!equal) {
							NDS_solutions.push_back(allSolutions[l]);
						}
					}

					std::ostringstream streamStr;

					clock_t runningTime;

					clock_t t0 = clock();

					int fronts = 0;

					TPoint::Comparisons = 0;
					t0 = clock();
					vector<vector<TPoint*>> frontsNDTree;
					fronts = NDS_NDTree(frontsNDTree);

					runningTime = clock() - t0;
					fstream stream("res.txt", ios::out | ios::ate | ios::app);
					streamStr << iter << '\t' << "NDTree\t" << j << "\t" << i << "\t" << fronts << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
					cout << streamStr.str();
					stream << streamStr.str();
					streamStr.str("");
					stream.close();

					TPoint::Comparisons = 0;
					t0 = clock();
					vector<vector<TPoint*>> frontsDDA_NS;
					fronts = DDA_NS(frontsDDA_NS);
					{
						runningTime = clock() - t0;
						fstream stream("res.txt", ios::out | ios::ate | ios::app);
						streamStr << iter << '\t' << "DDA_NS\t" << j << "\t" << i << "\t" << fronts << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
						cout << streamStr.str();
						stream << streamStr.str();
						streamStr.str("");
						stream.close();
					}

					vector<vector<TPoint*>> frontsENS_BS;
					TPoint::Comparisons = 0;
					t0 = clock();
					fronts = ENS_BS(frontsENS_BS);

					runningTime = clock() - t0;
					fstream stream2("res.txt", ios::out | ios::ate | ios::app);
					streamStr << iter << '\t' << "ENS_BS\t" << j << "\t" << i << "\t" << fronts << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
					cout << streamStr.str();
					stream2 << streamStr.str();
					streamStr.str("");
					stream2.close();

					vector<vector<TPoint*>> frontsENS_SS;
					TPoint::Comparisons = 0;
					t0 = clock();
					fronts = ENS_SS(frontsENS_SS);

					runningTime = clock() - t0;
					fstream stream3("res.txt", ios::out | ios::ate | ios::app);
					streamStr << iter << '\t' << "ENS_SS\t" << j << "\t" << i << "\t" << fronts << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
					cout << streamStr.str();
					stream3 << streamStr.str();
					streamStr.str("");
					stream3.close();

					vector<vector<TPoint*>> frontsMFront2;
					TPoint::Comparisons = 0;
					t0 = clock();
					fronts = NDS_MFront2(frontsMFront2);

					runningTime = clock() - t0;
					fstream stream4("res.txt", ios::out | ios::ate | ios::app);
					streamStr << iter << '\t' << "MFront2\t" << j << "\t" << i << "\t" << fronts << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
					cout << streamStr.str();
					stream4 << streamStr.str();
					streamStr.str("");
					stream4.close();

					/*					vector<vector<TPoint*>> frontsList;
										t0 = clock();
										fronts = NDS_List(frontsList);

										runningTime = clock() - t0;
										fstream stream5("res.txt", ios::out | ios::ate | ios::app);
										streamStr << iter << '\t' << "List\t" << j << "\t" << i << "\t" << fronts << "\t" << runningTime << "\t" << TPoint::Comparisons  << "\t" << "\n";
										cout << streamStr.str();
										stream5 << streamStr.str();
										streamStr.str("");
										stream5.close();*/


				}


			}
		}
		fstream stream("res.txt", ios::out | ios::ate | ios::app);
		stream << "\n";
		stream.close();
	}
}


void max_heapify(vector<double> &sumValue, TListSet<TPoint> &SolutionSet, int beg, int end)
{
    int curr = beg;
    int child = curr*2+1;
    while (child < end){
        if (child + 1 < end && sumValue[child] < sumValue[child+1])
            child++;
        if (sumValue[curr] < sumValue[child]){
            double temp     = sumValue[curr];
            sumValue[curr]  = sumValue[child];
            sumValue[child] = temp;
            TPoint *Ptemp      = SolutionSet[curr];
            SolutionSet[curr]  = SolutionSet[child];
            SolutionSet[child] = Ptemp;
            curr = child;
            child = 2*curr+1;
        }else{
            break;
        }
    }
}

void heap_sort(TListSet<TPoint> &SolutionSet)
{
    int n = SolutionSet.size();
    vector<double> sumValue(n,0);
    int i = 0;
    for (i = 0; i < n; i++){
        for (int j = 0; j < NumberOfObjectives; j++){
            sumValue[i]+= SolutionSet[i]->ObjectiveValues[j];
        }
    }
    for (i = n/2 - 1; i >= 0; i--){
        max_heapify(sumValue, SolutionSet, i, n-1);
    }
    for (i = n - 1; i > 0; i--){
        double temp = sumValue[i];
        sumValue[i] = sumValue[0];
        sumValue[0] = temp;
        TPoint *Ptemp  = SolutionSet[i];
        SolutionSet[i] = SolutionSet[0];
        SolutionSet[0] = Ptemp;
        max_heapify(sumValue, SolutionSet, 0, i);
    }
}


void generateUniformInstances() {
	int iter;
	for (iter = 0; iter < 10; iter++) {
		int j;
		for (j = 2; j <= 10; j++) {
			vector <TPoint*> solutions;
			int size = 100000;
			int i;
			for (i = 0; i < size; i++) {
				NumberOfObjectives = j;
				TPoint* point = new TPoint;

				double s = 0;
				int jj;
				for (jj = 0; jj < j; jj++) {
					point->ObjectiveValues[jj] = rand() % 1000;
				}

				solutions.push_back(point);
			}

			std::ostringstream fileName;
			fileName << "Uniform\\" << "uniform" << j << "_" << iter + 1;
			cout << fileName.str() << ' ';

			fstream Stream(fileName.str(), ios::out);

			for (i = 0; i < size; i++) {
				int j;
				for (j = 0; j < NumberOfObjectives; j++) {
					Stream << (double)solutions[i]->ObjectiveValues[j] << ' ';
				}

				Stream << '\n';
				Stream.flush();
			}
			Stream << '#';
			Stream.flush();

			Stream.close();
		}
	}
}

void ndTestFinal() {
	char buffer[1024];
	char outFileName[1024];
	int j;
    cout<<"iter\t"<<"Selection\t"<<"Obj\t"<<"Run\t"<<"UEASize\t"<<"Time\t"<<"Comp\n";
    int obj[] = {2,3,5,10};
    const char *proName[] = {"MPDMP","MOKP","PS","Art"};
    int serLis[] = {0,1,2,3};
    for(int proSerInd = 0; proSerInd < 4; proSerInd++){ 
        int proSer = serLis[proSerInd];
        int objMax = 3;
        if(proSer == 3){
            objMax = 0;
        }
        for (j = 0; j <= objMax; j++) {//objective
            int iter;
            for (iter = 0; iter < 1; iter++) {
                Problem.setArtificialProblem(obj[j]);
                int proInd = 1;
                int proNum = 9;
                if(proSer == 2)
                    proNum = 2;
                else
                    proNum = 1;
                for(proInd = 1; proInd <= proNum; proInd++){
                    int i;
                    for (i = 1; i <= 21; i++) {//run index
                        if(proSer <= 3 || proSer == 6){
                            snprintf(buffer, 1024, "Data%s/%s%d%d_200000_%d_10000.txt",proName[proSer],proName[proSer],proInd,obj[j], i);
                        }
                        else{
                            snprintf(buffer, 1024, "Data%s/%s%d_200000_%d_10000.txt",proName[proSer],proName[proSer],obj[j], i);
                        }
                        TListSet <TPoint> allSolutions;
                        cout << buffer << endl;
                        allSolutions.Load(buffer);
                        cout << allSolutions.size() << '\n';
                        
                        TListSet <TPoint> listSet;
                        TListSet <TPoint> listSetSorted;
                        listSetSorted.useSortedList = true;
                        TTreeSet <TPoint> treeSet;
                        QuadTree <TPoint> quadTree;
                        MFront2 <TPoint> mFront2;
                        MFront <TPoint> mFront;

                        std::ostringstream streamStr;

                        clock_t runningTime;
                        
                        //run time of ND-Tree
                        TPoint::Comparisons = 0;
                        clock_t t0 = clock();
                        int l;

                        for (l = 0; l < allSolutions.size(); l++) {
                            bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
                        }
                        runningTime = clock() - t0;
                        if(proSer <= 3 || proSer == 6)
                            snprintf(outFileName, 1024, "./RunTime_Mem_C=10/RunTime_%s%d_M%d.txt",proName[proSer],proInd,obj[j]);
                        else
                            snprintf(outFileName, 1024, "./RunTime_Mem_C=10/RunTime_%s_M%d.txt",proName[proSer],obj[j]);
                        cout<<outFileName<<endl;
                        fstream stream("res.txt", ios::out | ios::ate | ios::app);
                        fstream streamRunTime(outFileName, ios::out | ios::ate | ios::app);
                        streamStr << iter << '\t' << "NDTree\t\t" << obj[j] << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
                        cout << streamStr.str();
                        stream << streamStr.str();
                        streamStr.str("");

                        streamStr << runningTime << " " << treeSet.numberOfSolutions() << " " << TPoint::Comparisons << " ";
                        streamRunTime << streamStr.str();
                        streamStr.str("");
                        treeSet.root = NULL;

        //***************************************************************************
                        TPoint::Comparisons = 0;
                        int updGen = 5;
                        t0 = clock();
                        int genInd = 0;
                        //heap_sort(allSolutions);
                        for (genInd = 0; genInd<200/updGen; genInd++){
                            int startInd = (genInd+1)*allSolutions.size()/(200/updGen)-1;
                            int endInd   = genInd*allSolutions.size()/(200/updGen);
                            for (l = startInd; l >= endInd; l--) {
                                bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
                            }
                        }
                        runningTime = clock() - t0;
                        streamStr << iter << '\t' << "RNDTree5\t" << obj[j] << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
                        cout << streamStr.str();
                        stream << streamStr.str();
                        streamStr.str("");

                        streamStr << runningTime << " " << treeSet.numberOfSolutions() << " " << TPoint::Comparisons << " ";
                        streamRunTime << streamStr.str();
                        streamStr.str("");
                        treeSet.root = NULL;
        //***************************************************************************
                        TPoint::Comparisons = 0;
                        updGen = 10;
                        t0 = clock();
                        //heap_sort(allSolutions);
                        for (genInd = 0; genInd<200/updGen; genInd++){
                            int startInd = (genInd+1)*allSolutions.size()/(200/updGen)-1;
                            int endInd   = genInd*allSolutions.size()/(200/updGen);
                            for (l = startInd; l >= endInd; l--) {
                                bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
                            }
                        }
                        runningTime = clock() - t0;
                        streamStr << iter << '\t' << "RNDTree10\t" << obj[j] << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
                        cout << streamStr.str();
                        stream << streamStr.str();
                        streamStr.str("");

                        streamStr << runningTime << " " << treeSet.numberOfSolutions() << " " << TPoint::Comparisons << " ";
                        streamRunTime << streamStr.str();
                        streamStr.str("");
                        treeSet.root = NULL;
        //***************************************************************************
                        TPoint::Comparisons = 0;
                        updGen = 20;
                        t0 = clock();
                        //heap_sort(allSolutions);
                        for (genInd = 0; genInd<200/updGen; genInd++){
                            int startInd = (genInd+1)*allSolutions.size()/(200/updGen)-1;
                            int endInd   = genInd*allSolutions.size()/(200/updGen);
                            for (l = startInd; l >= endInd; l--) {
                                bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
                            }
                        }
                        runningTime = clock() - t0;
                        streamStr << iter << '\t' << "RNDTree20\t" << obj[j] << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
                        cout << streamStr.str();
                        stream << streamStr.str();
                        streamStr.str("");

                        streamStr << runningTime << " " << treeSet.numberOfSolutions() << " " << TPoint::Comparisons << " ";
                        streamRunTime << streamStr.str();
                        streamStr.str("");
                        treeSet.root = NULL;
        //***************************************************************************
                        TPoint::Comparisons = 0;
                        updGen = 50;
                        t0 = clock();
                        //heap_sort(allSolutions);
                        for (genInd = 0; genInd<200/updGen; genInd++){
                            int startInd = (genInd+1)*allSolutions.size()/(200/updGen)-1;
                            int endInd   = genInd*allSolutions.size()/(200/updGen);
                            for (l = startInd; l >= endInd; l--) {
                                bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
                            }
                        }
                        runningTime = clock() - t0;
                        streamStr << iter << '\t' << "RNDTree50\t" << obj[j] << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
                        cout << streamStr.str();
                        stream << streamStr.str();
                        streamStr.str("");

                        streamStr << runningTime << " " << treeSet.numberOfSolutions() << " " << TPoint::Comparisons << " ";
                        streamRunTime << streamStr.str();
                        streamStr.str("");
                        treeSet.root = NULL;
        //***************************************************************************
                        TPoint::Comparisons = 0;
                        updGen = 100;
                        t0 = clock();
                        //heap_sort(allSolutions);
                        for (genInd = 0; genInd<200/updGen; genInd++){
                            int startInd = (genInd+1)*allSolutions.size()/(200/updGen)-1;
                            int endInd   = genInd*allSolutions.size()/(200/updGen);
                            for (l = startInd; l >= endInd; l--) {
                                bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
                            }
                        }
                        runningTime = clock() - t0;
                        streamStr << iter << '\t' << "RNDTree100\t" << obj[j] << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
                        cout << streamStr.str();
                        stream << streamStr.str();
                        streamStr.str("");

                        streamStr << runningTime << " " << treeSet.numberOfSolutions() << " " << TPoint::Comparisons << " ";
                        streamRunTime << streamStr.str();
                        streamStr.str("");
                        treeSet.root = NULL;
        //***************************************************************************
                        TPoint::Comparisons = 0;
                        t0 = clock();
                        //heap_sort(allSolutions);
                        for (l = allSolutions.size()-1; l >= 0; l--) {
                        //for (l = 0; l < allSolutions.size(); l++) {
                            bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
                        }
                        runningTime = clock() - t0;
                        streamStr << iter << '\t' << "RNDTree\t\t" << obj[j] << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << TPoint::Comparisons << "\n";
                        cout << streamStr.str();
                        stream << streamStr.str();
                        streamStr.str("");

                        streamStr << runningTime << " " << treeSet.numberOfSolutions() << " " << TPoint::Comparisons << "\n";
                        streamRunTime << streamStr.str();
                        streamStr.str("");
                        treeSet.root = NULL;
        //***************************************************************************
        /*
                        vector<TPoint*> frontsENS_SS;
                        TPoint::Comparisons = 0;
                        NDS_solutions.clear();
                        for (l = 0; l < allSolutions.size(); l++) {
                            NDS_solutions.push_back(allSolutions[l]);
                        }
                        cout<<"I am here!"<< NDS_solutions.size() <<"\n";
                        t0 = clock();
                        int fronts = ENS_SS_ND(frontsENS_SS);
                        runningTime = clock() - t0;
                        streamStr << iter << '\t' << "ENS-SS\t\t" << obj[j] << "\t" << i << "\t" << frontsENS_SS.size() << "\t" << runningTime << "\t" << "\n";
                        cout << streamStr.str();
                        streamStr.str("");
                        streamStr << runningTime << "\n";
                        streamRunTime << streamStr.str();
                        streamStr.str("");


                        t0 = clock();
                        int genInd = 0;
                        for (genInd = 0; genInd<10; genInd++){
                            int startInd = (genInd+1)*allSolutions.size()/10-1;
                            int endInd   = genInd*allSolutions.size()/10;
                            for (l = startInd; l >= endInd; l--) {
                                bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
                            }
                        }
                        runningTime = clock() - t0;
                        streamStr << iter << '\t' << "RMNDTree\t" << obj[j] << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << "\n";
                        cout << streamStr.str();
                        stream << streamStr.str();
                        streamStr.str("");

                        streamStr << runningTime << "\n";
                        streamRunTime << streamStr.str();
                        streamStr.str("");

                        streamRunTime.close();
                        stream.close();
                        cout<<"************************************************************************************************"<<endl;
                        //run time of quadTree
                        t0 = clock();
                        for (l = 0; l < allSolutions.size(); l++) {
                            quadTree.Update(*((TPoint*)(allSolutions[l])));
                        }
                        runningTime = clock() - t0;
                        fstream stream3("res.txt", ios::out | ios::ate | ios::app);
                        streamStr << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
                        cout << streamStr.str();
                        stream3 << streamStr.str();
                        streamStr.str("");
                        stream3.close();

                        t0 = clock();
                        for (l = allSolutions.size()-1; l >= 0; l--) {
                            quadTree.Update(*((TPoint*)(allSolutions[l])));
                        }
                        runningTime = clock() - t0;
                        fstream stream3R("res.txt", ios::out | ios::ate | ios::app);
                        streamStr << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
                        cout << streamStr.str();
                        stream3R << streamStr.str();
                        streamStr.str("");
                        stream3R.close();
                        cout<<"************************************************************************************************"<<endl;

                        //run time of Mfront
                        
                        t0 = clock();
                        for (l = 0; l < allSolutions.size(); l++) {
                            mFront.Update(*((TPoint*)(allSolutions[l])));
                        }
                        runningTime = clock() - t0;
                        fstream stream4("res.txt", ios::out | ios::ate | ios::app);
                        streamStr << iter << "\t" << "MFront\t\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
                        cout << streamStr.str();
                        stream4 << streamStr.str();
                        streamStr.str("");
                        stream4.close();

                        t0 = clock();
                        for (l = allSolutions.size()-1; l >= 0; l--) {
                            mFront.Update(*((TPoint*)(allSolutions[l])));
                        }
                        runningTime = clock() - t0;
                        fstream stream4R("res.txt", ios::out | ios::ate | ios::app);
                        streamStr << iter << "\t" << "MFront\t\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
                        cout << streamStr.str();
                        stream4R << streamStr.str();
                        streamStr.str("");
                        stream4R.close();
                        cout<<"************************************************************************************************"<<endl;

                        //run time of Mfront2
                        
                        t0 = clock();
                        for (l = 0; l < allSolutions.size(); l++) {
                            bool mfront2Added = mFront2.Update(*((TPoint*)(allSolutions[l])));
                        }
                        runningTime = clock() - t0;
                        fstream stream5("res.txt", ios::out | ios::ate | ios::app);
                        streamStr << iter << "\t" << "MFront-II\t" << j << "\t" << i << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
                        cout << streamStr.str();
                        stream5 << streamStr.str();
                        streamStr.str("");
                        stream5.close();
                        t0 = clock();

                        for (l = allSolutions.size()-1; l >= 0; l--) {
                            bool mfront2Added = mFront2.Update(*((TPoint*)(allSolutions[l])));
                        }
                        runningTime = clock() - t0;
                        fstream stream5R("res.txt", ios::out | ios::ate | ios::app);
                        streamStr << iter << "\t" << "MFront-II\t" << j << "\t" << i << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
                        cout << streamStr.str();
                        stream5R << streamStr.str();
                        streamStr.str("");
                        stream5R.close();
                        cout<<"************************************************************************************************"<<endl;
                        */
                    }
                }
            }
            fstream stream("res.txt", ios::out | ios::ate | ios::app);
            stream << "\n";
            stream.close();
        }
    }
}

void ndTestNonConvex() {
	char buffer[1024];
	int j;
	for (j = 2; j <= 6; j++) {
		int iter;
		for (iter = 0; iter < 10; iter++) {
			Problem.setArtificialProblem(j);
			int i;
			for (i = 1; i <= 5; i++) {
				snprintf(buffer, 1024, "DataValMax10000\\ArtSet%d_200000_%d_10000.txt", j, i);
				TListSet <TPoint> allSolutions;
				allSolutions.Load(buffer);
				cout << allSolutions.size() << '\n';

				int l;
				for (l = 0; l < allSolutions.size(); l++) {
					int iobj;
					for (iobj = 0; iobj < NumberOfObjectives; iobj++) {
						allSolutions[l]->ObjectiveValues[iobj] = -allSolutions[l]->ObjectiveValues[iobj];
					}
				}

				srand(iter);
				for (l = 0; l < allSolutions.size(); l++) {
					TPoint* sol = allSolutions[l];
					int rpos = rand() % allSolutions.size();
					allSolutions[l] = allSolutions[rpos];
					allSolutions[rpos] = sol;
				}

				TListSet <TPoint> listSet;
				TListSet <TPoint> listSetSorted;
				listSetSorted.useSortedList = true;
				TTreeSet <TPoint> treeSet;
				QuadTree <TPoint> quadTree;
				MFront2 <TPoint> mFront2;
				MFront <TPoint> mFront;

				clock_t runningTime;
				clock_t t0 = clock();

				for (l = 0; l < allSolutions.size(); l++) {
					bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream("res.txt", ios::out | ios::ate | ios::app);
				stream << iter << '\t' << "NDTree\t" << j << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "NDTree\t\t" << j << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
				stream.close();

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					listSet.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream2("res.txt", ios::out | ios::ate | ios::app);
				stream2 << iter << '\t' << "List\t" << j << "\t" << i << "\t" << listSet.size() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "List\t\t" << j << "\t" << i << "\t" << listSet.size() << "\t" << runningTime << "\n";
				stream2.close();

				if (j == 2) {
					t0 = clock();
					for (l = 0; l < allSolutions.size(); l++) {
						listSetSorted.Update(*((TPoint*)(allSolutions[l])));
					}
					runningTime = clock() - t0;
					fstream stream2("res.txt", ios::out | ios::ate | ios::app);
					stream2 << iter << '\t' << "Sorted list\t" << j << "\t" << i << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
					cout << iter << '\t' << "Sorted list\t\t" << j << "\t" << i << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
					stream2.close();

				}

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					quadTree.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream3("res.txt", ios::out | ios::ate | ios::app);
				stream3 << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
				stream3.close();

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					mFront.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream4("res.txt", ios::out | ios::ate | ios::app);
				stream4 << iter << '\t' << "MFront\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "MFront\t\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
				stream4.close();

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					bool mfront2Added = mFront2.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream5("res.txt", ios::out | ios::ate | ios::app);
				stream5 << iter << '\t' << "MFront-II\t" << j << "\t" << i << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "MFront-II\t" << j << "\t" << i << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
				stream5.close();
			}
		}
		fstream stream("res.txt", ios::out | ios::ate | ios::app);
		stream << "\n";
		stream.close();
	}
}

void ndTestClustred() {
	char buffer[1024];
	int j;
	for (j = 2; j <= 6; j++) {
		int iter;
		for (iter = 0; iter < 10; iter++) {
			Problem.setArtificialProblem(j);
			int i;
			for (i = 1; i <= 5; i++) {
				snprintf(buffer, 1024, "ClusteredSets100\\ArtSetClust%d_200000_%d.txt", j, i);
				TListSet <TPoint> allSolutions;
				allSolutions.Load(buffer);
				cout << allSolutions.size() << '\n';

				int l;

				srand(iter);
				for (l = 0; l < allSolutions.size(); l++) {
					TPoint* sol = allSolutions[l];
					int rpos = rand() % allSolutions.size();
					allSolutions[l] = allSolutions[rpos];
					allSolutions[rpos] = sol;
				}

				TListSet <TPoint> listSet;
				TListSet <TPoint> listSetSorted;
				listSetSorted.useSortedList = true;
				TTreeSet <TPoint> treeSet;
				QuadTree <TPoint> quadTree;
				MFront2 <TPoint> mFront2;
				MFront <TPoint> mFront;

				clock_t runningTime;
				clock_t t0 = clock();

				for (l = 0; l < allSolutions.size(); l++) {
					bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream("res.txt", ios::out | ios::ate | ios::app);
				stream << iter << '\t' << "NDTree\t" << j << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "NDTree\t\t" << j << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
				stream.close();

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					listSet.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream2("res.txt", ios::out | ios::ate | ios::app);
				stream2 << iter << '\t' << "List\t" << j << "\t" << i << "\t" << listSet.size() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "List\t\t" << j << "\t" << i << "\t" << listSet.size() << "\t" << runningTime << "\n";
				stream2.close();

				if (j == 2) {
					t0 = clock();
					for (l = 0; l < allSolutions.size(); l++) {
						listSetSorted.Update(*((TPoint*)(allSolutions[l])));
					}
					runningTime = clock() - t0;
					fstream stream2("res.txt", ios::out | ios::ate | ios::app);
					stream2 << iter << '\t' << "Sorted list\t" << j << "\t" << i << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
					cout << iter << '\t' << "Sorted list\t\t" << j << "\t" << i << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
					stream2.close();

				}

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					quadTree.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream3("res.txt", ios::out | ios::ate | ios::app);
				stream3 << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
				stream3.close();

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					mFront.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream4("res.txt", ios::out | ios::ate | ios::app);
				stream4 << iter << '\t' << "MFront\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "MFront\t\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
				stream4.close();

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					bool mfront2Added = mFront2.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream5("res.txt", ios::out | ios::ate | ios::app);
				stream5 << iter << '\t' << "MFront-II\t" << j << "\t" << i << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "MFront-II\t" << j << "\t" << i << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
				stream5.close();
			}
		}
		fstream stream("res.txt", ios::out | ios::ate | ios::app);
		stream << "\n";
		stream.close();
	}
}

void ndTestNotUniform() {
	char buffer[1024];
	int j;
	for (j = 2; j <= 6; j++) {
		int iter;
		for (iter = 0; iter < 1; iter++) {
			Problem.setArtificialProblem(j);
			int i;
			for (i = 1; i <= 5; i++) {
				snprintf(buffer, 1024, "NotUniform\\ArtSetNotUniform%d_200000_%d.txt", j, i);
				TListSet <TPoint> allSolutions;
				allSolutions.Load(buffer);
				cout << allSolutions.size() << '\n';

				int l;

				srand(iter);
				for (l = 0; l < allSolutions.size(); l++) {
					TPoint* sol = allSolutions[l];
					int rpos = rand() % allSolutions.size();
					allSolutions[l] = allSolutions[rpos];
					allSolutions[rpos] = sol;
				}

				TListSet <TPoint> listSet;
				TListSet <TPoint> listSetSorted;
				listSetSorted.useSortedList = true;
				TTreeSet <TPoint> treeSet;
				QuadTree <TPoint> quadTree;
				MFront2 <TPoint> mFront2;
				MFront <TPoint> mFront;

				clock_t runningTime;
				clock_t t0 = clock();

				for (l = 0; l < allSolutions.size(); l++) {
					bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream("res.txt", ios::out | ios::ate | ios::app);
				stream << iter << '\t' << "NDTree\t" << j << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "NDTree\t\t" << j << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
				stream.close();

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					listSet.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream2("res.txt", ios::out | ios::ate | ios::app);
				stream2 << iter << '\t' << "List\t" << j << "\t" << i << "\t" << listSet.size() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "List\t\t" << j << "\t" << i << "\t" << listSet.size() << "\t" << runningTime << "\n";
				stream2.close();

				if (j == 2) {
					t0 = clock();
					for (l = 0; l < allSolutions.size(); l++) {
						listSetSorted.Update(*((TPoint*)(allSolutions[l])));
					}
					runningTime = clock() - t0;
					fstream stream2("res.txt", ios::out | ios::ate | ios::app);
					stream2 << iter << '\t' << "Sorted list\t" << j << "\t" << i << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
					cout << iter << '\t' << "Sorted list\t\t" << j << "\t" << i << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
					stream2.close();

				}

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					quadTree.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream3("res.txt", ios::out | ios::ate | ios::app);
				stream3 << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
				stream3.close();

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					mFront.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream4("res.txt", ios::out | ios::ate | ios::app);
				stream4 << iter << '\t' << "MFront\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "MFront\t\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
				stream4.close();

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					bool mfront2Added = mFront2.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream5("res.txt", ios::out | ios::ate | ios::app);
				stream5 << iter << '\t' << "MFront-II\t" << j << "\t" << i << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
				cout << iter << '\t' << "MFront-II\t" << j << "\t" << i << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
				stream5.close();
			}
		}
		fstream stream("res.txt", ios::out | ios::ate | ios::app);
		stream << "\n";
		stream.close();
	}
}

void ndTestFinalPLS() {
	char buffer[1024];
	char s[256];
	int j;
	for (j = 2; j <= 6; j++) {
		Problem.setArtificialProblem(j);
		snprintf(buffer, 1024, "DataPLS150\\Pop100000_150_%d.txt", j);
		TListSet <TPoint> allSolutions;
		allSolutions.Load(buffer);
		cout << allSolutions.size() << '\n';

		int l;

		TListSet <TPoint> listSet;
		TListSet <TPoint> listSetSorted;
		listSetSorted.useSortedList = true;
		TTreeSet <TPoint> treeSet;
		QuadTree <TPoint> quadTree;
		MFront2 <TPoint> mFront2;
		MFront <TPoint> mFront;

		clock_t runningTime;
		clock_t t0 = clock();

		for (l = 0; l < allSolutions.size(); l++) {
			bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream("res.txt", ios::out | ios::ate | ios::app);
		stream << "NDTree\t" << j << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
		cout << "NDTree\t\t" << j << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
		stream.close();

		t0 = clock();
		for (l = 0; l < allSolutions.size(); l++) {
			listSet.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream2("res.txt", ios::out | ios::ate | ios::app);
		stream2 << "List\t" << j << "\t" << listSet.size() << "\t" << runningTime << "\n";
		cout << "List\t\t" << j << "\t" << listSet.size() << "\t" << runningTime << "\n";
		stream2.close();

		if (j == 2) {
			t0 = clock();
			for (l = 0; l < allSolutions.size(); l++) {
				listSetSorted.Update(*((TPoint*)(allSolutions[l])));
			}
			runningTime = clock() - t0;
			fstream stream2("res.txt", ios::out | ios::ate | ios::app);
			stream2 << "Sorted list\t" << j << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
			cout << "Sorted list\t\t" << j << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
			stream2.close();

		}

		t0 = clock();
		for (l = 0; l < allSolutions.size(); l++) {
			quadTree.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream3("res.txt", ios::out | ios::ate | ios::app);
		stream3 << "QuadTree\t" << j << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
		cout << "QuadTree\t" << j << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
		stream3.close();

		t0 = clock();
		for (l = 0; l < allSolutions.size(); l++) {
			mFront.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream4("res.txt", ios::out | ios::ate | ios::app);
		stream4 << "MFront\t" << j << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
		cout << "MFront\t\t" << j << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
		stream4.close();

		t0 = clock();
		for (l = 0; l < allSolutions.size(); l++) {
			bool mfront2Added = mFront2.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream5("res.txt", ios::out | ios::ate | ios::app);
		stream5 << "MFront-II\t" << j << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
		cout << "MFront-II\t" << j << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
		stream5.close();
	}
}

void ndTestFinalMOEAD() {
	char buffer[1024];
	char s[256];
	int j;
	for (j = 2; j <= 6; j++) {
		Problem.setArtificialProblem(j);
		snprintf(buffer, 1024, "DataMOEAD\\PointsG_MOEAD_KS500%d_R1.DAT", j);
		TListSet <TPoint> allSolutions;
		allSolutions.Load(buffer);
		cout << allSolutions.size() << '\n';

		int l;

		TListSet <TPoint> listSet;
		TListSet <TPoint> listSetSorted;
		listSetSorted.useSortedList = true;
		TTreeSet <TPoint> treeSet;
		QuadTree <TPoint> quadTree;
		MFront2 <TPoint> mFront2;
		MFront <TPoint> mFront;

		clock_t runningTime;
		clock_t t0 = clock();

		for (l = 0; l < allSolutions.size(); l++) {
			bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream("res.txt", ios::out | ios::ate | ios::app);
		stream << "NDTree\t" << j << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
		cout << "NDTree\t\t" << j << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
		stream.close();

		t0 = clock();
		for (l = 0; l < allSolutions.size(); l++) {
			listSet.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream2("res.txt", ios::out | ios::ate | ios::app);
		stream2 << "List\t" << j << "\t" << listSet.size() << "\t" << runningTime << "\n";
		cout << "List\t\t" << j << "\t" << listSet.size() << "\t" << runningTime << "\n";
		stream2.close();

		if (j == 2) {
			t0 = clock();
			for (l = 0; l < allSolutions.size(); l++) {
				listSetSorted.Update(*((TPoint*)(allSolutions[l])));
			}
			runningTime = clock() - t0;
			fstream stream2("res.txt", ios::out | ios::ate | ios::app);
			stream2 << "Sorted list\t" << j << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
			cout << "Sorted list\t\t" << j << "\t" << listSetSorted.size() << "\t" << runningTime << "\n";
			stream2.close();

		}

		t0 = clock();
		for (l = 0; l < allSolutions.size(); l++) {
			quadTree.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream3("res.txt", ios::out | ios::ate | ios::app);
		stream3 << "QuadTree\t" << j << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
		cout << "QuadTree\t" << j << "\t" << quadTree.numberOfSolutions() << "\t" << runningTime << "\n";
		stream3.close();

		t0 = clock();
		for (l = 0; l < allSolutions.size(); l++) {
			mFront.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream4("res.txt", ios::out | ios::ate | ios::app);
		stream4 << "MFront\t" << j << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
		cout << "MFront\t\t" << j << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\n";
		stream4.close();

		t0 = clock();
		for (l = 0; l < allSolutions.size(); l++) {
			bool mfront2Added = mFront2.Update(*((TPoint*)(allSolutions[l])));
		}
		runningTime = clock() - t0;
		fstream stream5("res.txt", ios::out | ios::ate | ios::app);
		stream5 << "MFront-II\t" << j << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
		cout << "MFront-II\t" << j << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\n";
		stream5.close();
	}
}

void ndTestFinalNumberOfSolutions() {
	char buffer[1024];
	int iter;
	for (iter = 0; iter <= 10; iter++) {
		int j;
		for (j = 4; j <= 4; j++) {
			Problem.setArtificialProblem(j);
			int i;
			for (i = 3; i <= 3; i++) {
				snprintf(buffer, 1024, "DataValMax10000\\ArtSet%d_200000_%d_10000.txt", j, i);
				TListSet <TPoint> allSolutions;
				allSolutions.Load(buffer);
				cout << allSolutions.size() << '\n';

				int l;

				srand(iter);
				for (l = 0; l < allSolutions.size(); l++) {
					TPoint* sol = allSolutions[l];
					int rpos = rand() % allSolutions.size();
					allSolutions[l] = allSolutions[rpos];
					allSolutions[rpos] = sol;
				}

				TListSet <TPoint> listSet;
				TTreeSet <TPoint> treeSet;
				QuadTree <TPoint> quadTree;
				MFront2 <TPoint> mFront2;
				MFront <TPoint> mFront;

				clock_t runningTime;
				clock_t t0 = clock();

				for (l = 0; l < allSolutions.size(); l++) {
					bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
					if ((l + 1) % 10000 == 0) {
						runningTime = clock() - t0;
						fstream stream("res.txt", ios::out | ios::ate | ios::app);
						stream << iter << '\t' << "NDTree\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						cout << iter << '\t' << "NDTree\t\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						stream.close();
						t0 = clock() - runningTime;
					}
				}

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					listSet.Update(*((TPoint*)(allSolutions[l])));
					if ((l + 1) % 10000 == 0) {
						runningTime = clock() - t0;
						fstream stream("res.txt", ios::out | ios::ate | ios::app);
						stream << iter << '\t' << "List\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						cout << iter << '\t' << "List\t\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						stream.close();
						t0 = clock() - runningTime;
					}
				}

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					quadTree.Update(*((TPoint*)(allSolutions[l])));
					if ((l + 1) % 10000 == 0) {
						runningTime = clock() - t0;
						fstream stream("res.txt", ios::out | ios::ate | ios::app);
						stream << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						cout << iter << '\t' << "QuadTree\t\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						stream.close();
						t0 = clock() - runningTime;
					}
				}

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					mFront.Update(*((TPoint*)(allSolutions[l])));
					if ((l + 1) % 10000 == 0) {
						runningTime = clock() - t0;
						fstream stream("res.txt", ios::out | ios::ate | ios::app);
						stream << iter << '\t' << "MFront\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						cout << iter << '\t' << "MFront\t\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						stream.close();
						t0 = clock() - runningTime;
					}
				}

				t0 = clock();
				for (l = 0; l < allSolutions.size(); l++) {
					bool mfront2Added = mFront2.Update(*((TPoint*)(allSolutions[l])));
					if ((l + 1) % 10000 == 0) {
						runningTime = clock() - t0;
						fstream stream("res.txt", ios::out | ios::ate | ios::app);
						stream << iter << '\t' << "MFront-II\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						cout << iter << '\t' << "MFront-II\t\t" << j << "\t" << i << "\t" << l + 1 << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						stream.close();
						t0 = clock() - runningTime;
					}
				}
			}
		}
	}
}

void ndTestFinalParameters() {
	char buffer[1024];
	int maxListSizes[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 50, 100, 200, 500 };

	int iter;
	for (iter = 0; iter <= 10; iter++) {
		int j;
		for (j = 4; j <= 4; j++) {
			Problem.setArtificialProblem(j);
			int i;
			for (i = 3; i <= 3; i++) {
				snprintf(buffer, 1024, "DataValMax10000\\ArtSet%d_200000_%d_10000.txt", j, i);
				TListSet <TPoint> allSolutions;
				allSolutions.Load(buffer);
				cout << allSolutions.size() << '\n';

				int l;

				srand(iter);
				for (l = 0; l < allSolutions.size(); l++) {
					TPoint* sol = allSolutions[l];
					int rpos = rand() % allSolutions.size();
					allSolutions[l] = allSolutions[rpos];
					allSolutions[rpos] = sol;
				}

				int ls;
				for (ls = 0; ls < 16; ls++) {
					int bs;
					for (bs = 2; bs < 20; bs++) {
						if (bs > maxListSizes[ls] + 1)
							continue;

						TTreeSet <TPoint> treeSet;
						maxListSize = maxListSizes[ls];
						maxBranches = bs;

						clock_t runningTime;
						clock_t t0 = clock();

						for (l = 0; l < allSolutions.size(); l++) {
							bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
						}
						runningTime = clock() - t0;
						fstream stream("res.txt", ios::out | ios::ate | ios::app);
						stream << iter << '\t' << "NDTree\t" << j << "\t" << i << "\t" << maxListSizes[ls] << "\t" << bs << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						cout << iter << '\t' << "NDTree\t\t" << j << "\t" << i << "\t" << maxListSizes[ls] << "\t" << bs << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\n";
						stream.close();
					}
				}
			}
		}
	}
}

void ndTestFPC() {
	char buffer[1024];
	int iter;
	for (iter = 1; iter < 10; iter++) {
		int j;
		for (j = 2; j <= 10; j++) {
			Problem.setArtificialProblem(j);
			int i;
			for (i = 3; i <= 3; i++) {

				if (j >= 7) {
					snprintf(buffer, 1024, "DataValMax10000\\ArtSet%d_100000_%d.txt", j, i);
				}
				else {
					snprintf(buffer, 1024, "DataValMax10000\\ArtSet%d_200000_%d_10000.txt", j, i);
				}
				TListSet <TPoint> allSolutions;
				allSolutions.Load(buffer);
				cout << allSolutions.size() << '\n';

				int l;

				srand(iter);
				for (l = 0; l < allSolutions.size(); l++) {
					TPoint* sol = allSolutions[l];
					int rpos = rand() % allSolutions.size();
					allSolutions[l] = allSolutions[rpos];
					allSolutions[rpos] = sol;
				}

				TListSet <TPoint> listSet;
				TListSet <TPoint> listSetSorted;
				listSetSorted.useSortedList = true;
				TTreeSet <TPoint> treeSet;
				QuadTree <TPoint> quadTree2;
				MFront2 <TPoint> mFront2;
				MFront <TPoint> mFront;

				clock_t runningTime;
				clock_t t0;

				t0 = clock();
				TPoint::Comparisons = 0;
				fpc = 0;

				for (l = 0; l < allSolutions.size(); l++) {
					bool treeAdded = treeSet.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream("res_fpc.txt", ios::out | ios::ate | ios::app);
				stream << iter << '\t' << "NDTree\t" << j << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
				cout << iter << '\t' << "NDTree\t\t" << j << "\t" << i << "\t" << treeSet.numberOfSolutions() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
				stream.close();

				t0 = clock();
				TPoint::Comparisons = 0;
				for (l = 0; l < allSolutions.size(); l++) {
					bool added = listSet.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream2("res_fpc.txt", ios::out | ios::ate | ios::app);
				stream2 << iter << '\t' << "List\t" << j << "\t" << i << "\t" << listSet.size() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
				cout << iter << '\t' << "List\t\t" << j << "\t" << i << "\t" << listSet.size() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
				stream2.close();

				if (j == 2) {
					t0 = clock();
					TPoint::Comparisons = 0;
					fpc = 0;
					for (l = 0; l < allSolutions.size(); l++) {
						listSetSorted.Update(*((TPoint*)(allSolutions[l])));
					}
					runningTime = clock() - t0;
					fstream stream2("res_fpc.txt", ios::out | ios::ate | ios::app);
					stream2 << iter << '\t' << "Sorted list\t" << j << "\t" << i << "\t" << listSetSorted.size() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
					cout << iter << '\t' << "Sorted list\t\t" << j << "\t" << i << "\t" << listSetSorted.size() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
					stream2.close();

				}

				t0 = clock();
				fpc = 0;
				TPoint::Comparisons = 0;
				for (l = 0; l < allSolutions.size(); l++) {
					quadTree2.Update(*((TPoint*)(allSolutions[l])));
				}

				runningTime = clock() - t0;
				fstream stream3("res_fpc.txt", ios::out | ios::ate | ios::app);
				stream3 << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree2.numberOfSolutions() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
				cout << iter << '\t' << "QuadTree\t" << j << "\t" << i << "\t" << quadTree2.numberOfSolutions() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
				stream3.close();

				t0 = clock();
				TPoint::Comparisons = 0;
				fpc = 0;
				TPoint::Comparisons = 0;
				for (l = 0; l < allSolutions.size(); l++) {
					mFront.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream4("res_fpc.txt", ios::out | ios::ate | ios::app);
				stream4 << iter << '\t' << "MFront\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
				cout << iter << '\t' << "MFront\t\t" << j << "\t" << i << "\t" << mFront.numberOfSolutions() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\n";
				stream4.close();

				t0 = clock();
				TPoint::Comparisons = 0;
				fpc = 0;
				TPoint::Comparisons = 0;
				for (l = 0; l < allSolutions.size(); l++) {
					bool mfront2Added = mFront2.Update(*((TPoint*)(allSolutions[l])));
				}
				runningTime = clock() - t0;
				fstream stream5("res_fpc.txt", ios::out | ios::ate | ios::app);
				stream5 << iter << '\t' << "MFront-II\t" << j << "\t" << i << "\t" << mFront2.numberOfSolutions() << "\t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\t" << mFront2.kdTreeRunningTime << "\t" << runningTime - mFront2.kdTreeRunningTime << "\n";
				cout << iter << '\t' << "MFront-II\t" << j << "\t" << i << " \t" << mFront2.numberOfSolutions() << " \t" << runningTime << "\t" << fpc << "\t" << TPoint::Comparisons << "\t" << mFront2.kdTreeRunningTime << " \t" << runningTime - mFront2.kdTreeRunningTime << "\n";
				stream5.close();
			}
		}
	}
}


	int main() {
		srand(13);
		//	ndTestFPC();
		//		ndTestFinalParameters();
		//		ndTestFinalNumberOfSolutions();
		//		ndTestFinalPLS();
		//		ndTestFinalMOEAD();
		ndTestFinal();
		//	generateUniformInstances();
//		ndsTest(5000);
		//		ndTestClustred();
		//		ndTestNotUniform();
		return 0;
	}
