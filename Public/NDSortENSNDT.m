function [FrontNo,MaxFNo,CountComp,Relation] = NDSortENSNDT(varargin)
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

    PopObj = varargin{2};
    Relation = varargin{4};
    MaxFNo = 1;
    [FrontNo,CountComp] = ENS_NDT(PopObj);
end

function [FrontNo,CountComp] = ENS_NDT(PopObj)
    CountComp = 0;
    B = 2;
    [~,M] = size(PopObj);
    [PopObj,~,Loc] = unique(PopObj(:,M:-1:1),'rows');
    PopObj = PopObj(:,M:-1:1);
    N = size(PopObj,1);
    FrontNo   = inf(1,N);
    % Construct the tree structure
    Forest       = zeros(3,N);
    Father       = zeros(1,N);
    LeftBrother  = zeros(1,N);
    LeftChild    = zeros(1,N);
    RightChild   = zeros(1,N);
    Leaf         = cell(1,N);
    Branched     = true(1,N);
    Leaf{1,1}    = 1:N;
    Forest(3,1)  = 1;
    CurNode      = 1;
    EmpInd       = 1;%the last notempty index of Forest
    % Construct the Splite
    while true
        CurSol = Leaf{1,CurNode};
        if length(CurSol) > B
            obj = mod(Forest(3,CurNode),M-1)+1;
            [~,I] = sort(PopObj(CurSol,obj));
            SS = CurSol(I);
            SSObj = PopObj(SS,obj);
            LeftChild(1,CurNode) = EmpInd+1; EmpInd = EmpInd+1;
            RightChild(1,CurNode) = EmpInd+1; EmpInd = EmpInd+1;
            LeftBrother(1,RightChild(1,CurNode)) = LeftChild(1,CurNode);
            Father(1,LeftChild(1,CurNode)) = CurNode;
            Father(1,RightChild(1,CurNode)) = CurNode;
            Forest(1,CurNode) = obj;
            Forest(2,CurNode) = SSObj(floor(length(SS)/2));
            Leaf{1,LeftChild(1,CurNode)}  = SS(1:floor(length(SS)/2));
            Leaf{1,RightChild(1,CurNode)} = SS(floor(length(SS)/2)+1:end);
            Forest(3,LeftChild(1,CurNode)) = Forest(3,CurNode)+1;
            Forest(3,RightChild(1,CurNode)) = Forest(3,CurNode)+1;
            Leaf{1,CurNode} = [];
            CurNode = RightChild(1,CurNode);
        else
            Branched(1,CurNode) = false;
            while ~LeftBrother(1,CurNode) && Father(1,CurNode)
                CurNode = Father(1,CurNode);
            end
            if LeftBrother(1,CurNode)
                CurNode = LeftBrother(1,CurNode);
            else
                break;
            end
        end
    end
    if size(Forest,2) < length(Leaf)
        Forest(:,length(Leaf)) = zeros(3,1);
    end
    if length(LeftBrother) < length(Leaf)
        LeftBrother(1,length(Leaf)) = 0;
    end
    if length(LeftChild) < length(Leaf)
        LeftChild(1,length(Leaf)) = 0;
    end
    if length(RightChild) < length(Leaf)
        RightChild(1,length(Leaf)) = 0;
    end
    if length(Branched) < length(Leaf)
        Branched(1,length(Leaf)) = true;
    end
    LeafSol = cell(1,length(Leaf));
    Occupy  = false(1,length(Leaf));
    % Insert the first solution into the front
    FrontNo(1,1) = 1;
    CurNode = 1;
    while true
        Occupy(1,CurNode) = true;
        if Branched(1,CurNode)
            if PopObj(1,Forest(1,CurNode)) < Forest(2,CurNode)
                CurNode = LeftChild(1,CurNode);
            else
                CurNode = RightChild(1,CurNode);
            end
        else
            LeafSol{1,CurNode} = [LeafSol{1,CurNode},1];
            break;
        end
    end
    % Select Non-dominated solutions one by one
    for i = 2:N
        CurNode = 1;
        Dominated = false;
        % Check if the current solution is dominated
        while true
            obj = Forest(1,CurNode);
            val = Forest(2,CurNode);
            if Branched(1,CurNode)% if this node is a branch
                if Occupy(1,RightChild(1,CurNode)) && PopObj(i,obj) >= val
                    CurNode = RightChild(1,CurNode);
                    continue;
                end
                if Occupy(1,LeftChild(1,CurNode))
                    CurNode = LeftChild(1,CurNode);
                    continue;
                else
                    while ~LeftBrother(1,CurNode) && Father(1,CurNode)
                        CurNode = Father(1,CurNode);
                    end
                    if LeftBrother(1,CurNode)
                        CurNode = LeftBrother(1,CurNode);
                    else
                        FrontNo(1,i) = 1;
                        break;
                    end
                end
            else% if this node is a leaf
                SolSet = PopObj(LeafSol{1,CurNode},:);
                NS = size(SolSet,1);
                for j = NS:-1:1
                    CountComp = CountComp+1;
                    m = 1;
                    while m < M && SolSet(j,m) <= PopObj(i,m)
                        m = m+1;
                    end
                    if m == M
                        Dominated = true;
                        break;
                    end
                end
                if Dominated
                    break;
                else
                    while ~LeftBrother(1,CurNode) && Father(1,CurNode)
                        CurNode = Father(1,CurNode);
                    end
                    if LeftBrother(1,CurNode)
                        CurNode = LeftBrother(1,CurNode);
                    else
                        FrontNo(1,i) = 1;
                        break;
                    end
                end
            end
        end
        % insert the non-dominated solution into the tree
        if FrontNo(1,i) == 1
            CurNode = 1;
            while true
                Occupy(1,CurNode) = true;
                if Branched(1,CurNode)
                    if PopObj(i,Forest(1,CurNode)) < Forest(2,CurNode)
                        CurNode = LeftChild(1,CurNode);
                    else
                        CurNode = RightChild(1,CurNode);
                    end
                else
                    LeafSol{1,CurNode} = [LeafSol{1,CurNode},i];
                    break;
                end
            end
        end
    end
    FrontNo = FrontNo(:,Loc);
end
