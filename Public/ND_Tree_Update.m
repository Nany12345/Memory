function ND_Tree = ND_Tree_Update(ND_Tree,solution)
    % ND_Tree{1,n}: bool, if this node is existing or not
    % ND_Tree{2,n}: int, the number of solutions in the node
    % ND_Tree{3,n}: array, Solutions in the node
    % ND_Tree{4,n}: int, the number of branches of the node
    % ND_Tree{5,n}: array, branch nodes of the node
    % ND_Tree{6,n}: array(2,m), the approximate idea and nadir point
    % ND_Tree{7,n}: int, the father of each node
    % ND_Tree{8,n}: int, the right brother of each node

    MAX_BRANCH_SIZE = 5;
    MAX_NODE_SIZE   = 20;
    M               = length(solution.obj);

    if ~ND_Tree{1,1}
        ND_Tree{1,1} = true;
        ND_Tree{2,1} = 1;
        ND_Tree{3,1} = solution;
        ND_Tree{4,1} = 0;
        ND_Tree{5,1} = [];
        ND_Tree{6,1}(1,:) = solution.obj;
        ND_Tree{6,1}(2,:) = solution.obj;
        ND_Tree{7,1}      = -1;
        ND_Tree{8,1}      = -1;
    else
        curNode = 1;
        while true
            % if this point is dominatde by the nadir of curnode
            m = 1; equNum = 0;
            while m <= M && Solution(1,m) >= ND_Tree{6,curNode}(1,m)
                m = m + 1;
                if Solution(1,m) == ND_Tree{6,curNode}(1,m)
                    equNum = equNum+1;
                end
            end
            if m > M && equNum ~= M
                break;
            end
            % if this point dominats the ideal of curnode
            m = 1; equNum = 0;
            while m <= M && Solution(1,m) <= ND_Tree{6,curNode}(2,m)
                m = m + 1;
                if Solution(1,m) == ND_Tree{6,curNode}(2,m)
                    equNum = equNum+1;
                end
            end
            if m > M && equNum ~= M
                if ND_Tree{7,curNode} ~= -1
                    father = ND_Tree{7,curNode};
                    ND_Tree{4,father} = ND_Tree{4,father}-1;
                    branches = ND_Tree{5,father};
                    branches(find(branches == curNode)) = [];
                    ND_Tree{5,father} = branches;
                end
                while true
                    ND_Tree{1,curNode} = false;
                    if ND_Tree{4,curNode} > 0
                        ND_Tree{
                end
            end
        end
    end
end
