function [FrontNo,MaxFNo,CountComp,Relation] = NDSortTENS_MemSS(varargin)
%NDSort - Do non-dominated sorting by efficient non-dominated sort.
%
%   FrontNo = NDSort(F,s) does non-dominated sorting on F, where F is the
%   matrix of objective values of a set of individuals, and s is the number
%   of individuals to be sorted at least. FrontNo(i) denotes the front
%   number of the i-th individual. The individuals have not been sorted are
%   assigned a front number of inf.
%
%   FrontNo = NDSort(F,C,s) does non-dominated sorting based on constrained
%   domination, where C is the matrix of constraint values of the
%   individuals. In this case, feasible solutions always dominate
%   infeasible solutions, and one infeasible solution dominates another
%   infeasible solution if the former has a smaller overall constraint
%   violation than the latter.
%
%   In particular, s = 1 indicates finding only the first non-dominated
%   front, s = size(F,1)/2 indicates sorting only half the population
%   (which is often used in the algorithm), and s = inf indicates sorting
%   the whole population.
%
%   [FrontNo,K] = NDSort(...) also returns the maximum front number besides
%   inf.
%
%   Example:
%       [FrontNo,MaxFNo] = NDSort(PopObj,1)
%       [FrontNo,MaxFNo] = NDSort(PopObj,PopCon,inf)

%------------------------------- Reference --------------------------------
% [1] X. Zhang, Y. Tian, R. Cheng, and Y. Jin, An efficient approach to
% nondominated sorting for evolutionary multiobjective optimization, IEEE
% Transactions on Evolutionary Computation, 2015, 19(2): 201-213.
% [2] X. Zhang, Y. Tian, R. Cheng, and Y. Jin, A decision variable
% clustering based evolutionary algorithm for large-scale many-objective
% optimization, IEEE Transactions on Evolutionary Computation, 2018, 22(1):
% 97-112.
%------------------------------- Copyright --------------------------------
% Copyright (c) 2018-2019 BIMK Group. You are free to use the PlatEMO for
% research purposes. All publications which use this platform or any code
% in the platform should acknowledge the use of "PlatEMO" and reference "Ye
% Tian, Ran Cheng, Xingyi Zhang, and Yaochu Jin, PlatEMO: A MATLAB platform
% for evolutionary multi-objective optimization [educational forum], IEEE
% Computational Intelligence Magazine, 2017, 12(4): 73-87".
%--------------------------------------------------------------------------

    PopObj1  = varargin{1};
    PopObj2  = varargin{2};
    nSort    = varargin{3};
    Relation = vargin{4}
    [FrontNo,MaxFNo,CountComp] = T_ENS_Mem_SS(PopObj1,PopObj2,nSort);
end

function [FrontNo,MaxFNo,CountComp] = T_ENS_Mem_SS(PopObj1,PopObj2,nSort)
    CountComp = 0;
    [PopObj,Loa,Loc] = unique([PopObj1;PopObj2],'rows');
    N1 = size(PopObj1,1);
    [N,M] = size(PopObj);
    Table     = hist(Loc,1:N);
    LoaOld = find(Loa<=N1);
    LoaNew = find(Loa>N1);
    FrontNo   = inf(1,N);
    MaxFNo    = 0;
    Forest    = zeros(1,N);
    Children  = zeros(N,M-1);
    LeftChild = zeros(1,N) + M;%the position of leftest child of current solution q
    Father    = zeros(1,N);
    Brother   = zeros(1,N) + M;%the right branch of the solution q
    [~,ORank] = sort(PopObj(:,2:M),2,'descend');
    ORank     = ORank + 1;
    while sum(Table(FrontNo<inf)) < min(nSort,length(Loc))
        MaxFNo = MaxFNo + 1;
        root   = find(FrontNo==inf,1);
        Forest(MaxFNo) = root;
        FrontNo(root)  = MaxFNo;
        COO = 1; CNO = 1;
        for p = 1 : N
            if N1 > 0 && COO <= length(LoaOld) && p == LoaOld(COO)
                if FrontNo(p) == inf
                    Dominated = false;
                    for j = CNO-1 : -1 : 1
                        if FrontNo(LoaNew(j)) == MaxFNo
                            CountComp = CountComp+1;
                            m = 2;
                            while m <= M && PopObj(p,m) >= PopObj(LoaNew(j),m)
                                m = m + 1;
                            end
                            Dominated = m > M;
                            if Dominated || M == 2
                                break;
                            end
                        end
                    end
                    if ~Dominated
                        q = Forest(MaxFNo);
                        Pruning = zeros(1,N);%the m number of p according to current q
                        while true
                            CountComp = CountComp+1;
                            m = 1;
                            while m < M && PopObj(p,ORank(q,m)) >= PopObj(q,ORank(q,m))
                                m = m + 1;
                            end
                            if m == M
                                break;
                            else
                                Pruning(q) = m;
                                if Children(q,Pruning(q))
                                    q = Children(q,Pruning(q));
                                else
                                    FrontNo(p) = MaxFNo;
                                    Children(q,Pruning(q)) = p;
                                    Father(p) = q;
                                    if LeftChild(q) > Pruning(q)
                                        Brother(p)   = LeftChild(q);
                                        LeftChild(q) = Pruning(q);
                                    else
                                        bro = Children(q,LeftChild(q));
                                        while Brother(bro) < Pruning(q)
                                            bro = Children(q,Brother(bro));
                                        end
                                        Brother(p)   = Brother(bro);
                                        Brother(bro) = Pruning(q);
                                    end
                                    break;
                                end
                            end
                        end
                    end
                end
                COO = COO+1;
            elseif CNO <= length(LoaNew) && p == LoaNew(CNO) 
                if FrontNo(p) == inf
                    Pruning = zeros(1,N);
                    q = Forest(MaxFNo);
                    while true
                        CountComp = CountComp+1;
                        m = 1;
                        while m < M && PopObj(p,ORank(q,m)) >= PopObj(q,ORank(q,m))
                            m = m + 1;
                        end
                        if m == M
                            break;
                        else
                            Pruning(q) = m;
                            if LeftChild(q) <= Pruning(q)
                                q = Children(q,LeftChild(q));
                            else
                                while Father(q) && Brother(q) > Pruning(Father(q))
                                    q = Father(q);
                                end
                                if Father(q)
                                    q = Children(Father(q),Brother(q));
                                else
                                    break;
                                end
                            end
                        end
                    end
                    if m < M
                        FrontNo(p) = MaxFNo;
                        q = Forest(MaxFNo);
                        while Children(q,Pruning(q))
                            q = Children(q,Pruning(q));
                        end
                        Children(q,Pruning(q)) = p;
                        Father(p) = q;
                        if LeftChild(q) > Pruning(q)
                            Brother(p)   = LeftChild(q);
                            LeftChild(q) = Pruning(q);
                        else
                            bro = Children(q,LeftChild(q));
                            while Brother(bro) < Pruning(q)
                                bro = Children(q,Brother(bro));
                            end
                            Brother(p)   = Brother(bro);
                            Brother(bro) = Pruning(q);
                        end
                    end
                end
                CNO = CNO+1;
            end 
        end
    end
    FrontNo = FrontNo(:,Loc);
end
