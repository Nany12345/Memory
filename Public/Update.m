function Update(root,individual)
    if root.isLeaf && root.solNum == 0
        insert(root,individual);
    else
        [dominated,nodeDel,nodeRep] = update(root,individual);
        if ~dominated
            if nodeDel
                root.isLeaf = true;
                root.solNum = 0;
                root.solSet = [];
                root.braNum = 0;
                root.braSet = [];
                root.ideSol = [];
                root.nadSol = [];
            elseif nodeRep
                root.isLeaf = root.braSet(1).isLeaf;
                root.solNum = root.braSet(1).solNum;
                root.solSet = root.braSet(1).solSet;
                root.braNum = root.braSet(1).braNum;
                root.ideSol = root.braSet(1).ideSol;
                root.nadSol = root.braSet(1).nadSol;
                branch = root.braSet(1);
                root.braSet = branch.braSet;
            end
            insert(root,individual);
        end
    end
end
function [dominated,nodeDel,nodeRep] = update(node,individual)
    dominated = false;
    nodeDel   = false;
    nodeRep   = false;
    % if this point is dominatde by the nadir of curnode
    m = 1; equNum = 0; M = length(node.ideSol);
    while m <= M && individual.obj(1,m) >= node.nadSol(1,m)
        if individual.obj(1,m) == node.nadSol(1,m)
            equNum = equNum+1;
        end
        m = m + 1;
    end
    isIden = sum(node.nadSol==node.ideSol) == M;
    if m > M && equNum ~= M
        dominated = true;
        return;
    elseif equNum == M && isIden
        return;
    elseif equNum == M && ~isIden
        dominated = true;
        return;
    end
    % if this point dominatdes the ideal of curnode
    m = 1; equNum = 0;
    while m <= M && individual.obj(1,m) <= node.ideSol(1,m)
        if individual.obj(1,m) == node.ideSol(1,m)
            equNum = equNum+1;
        end
        m = m + 1;
    end
    if m > M && equNum ~= M
        nodeDel = true;
        return;
    elseif equNum == M && isIden
        return;
    elseif equNum == M && ~isIden
        nodeDel = true;
        return;
    end
    % ideal doesn't dominate the point and the point doesn't dominates nadir
    mi = 1;
    while mi <= M && individual.obj(1,mi) >= node.ideSol(1,mi)
        mi = mi + 1;
    end
    mn = 1;
    while mn <= M && individual.obj(1,mn) <= node.nadSol(1,mn)
        mn = mn + 1;
    end
    if mi <= M && mn <= M
        return;
    end
    % Analyse the Node
    if node.isLeaf
        if ~isIden
            solInd = 1;
            while solInd <= node.solNum
                m = 1; smaNum = 0; larNum = 0; equNum = 0;
                objVal = node.solSet(1,solInd).obj;
                while m <= M
                    if individual.obj(1,m) <= objVal(1,m)
                        smaNum = smaNum+1;
                    elseif individual.obj(1,m) >= objVal(1,m)
                        larNum = larNum+1;
                    end
                    if individual.obj(1,m) == objVal(1,m)
                        equNum = equNum+1;
                    end
                    m = m + 1;
                end
                if equNum < M
                    if larNum == M
                        dominated = true;
                        break
                    end
                    if smaNum == M
                        node.solSet(solInd) = [];
                        node.solNum = node.solNum-1;
                        solInd = solInd-1;
                    end
                end
                solInd = solInd+1;
            end
            if node.solNum == 0
                nodeDel = true;
            end
        end
    else
        childInd = 1;
        while childInd <= node.braNum
            child = node.braSet(1,childInd);
            [dominated,nodeDelCur,nodeRepCur] = update(child,individual);
            if nodeDelCur
                node.braSet(childInd) = [];
                node.braNum = node.braNum-1;
                childInd = childInd-1;
            end
            if nodeRepCur
                grandSon = node.braSet(1,childInd).braSet(1);
                node.braSet(1,childInd) = grandSon;
            end
            if dominated
                break;
            end
            childInd = childInd+1;
        end
        if node.braNum == 1
            nodeRep = true;
        elseif node.braNum == 0
            nodeDel = true;
        end
    end
end
function insert(node,individual)
    MAX_SET_SIZE = 20;
    if node.isLeaf && node.solNum == 0
        node.solNum = 1;
        node.solSet = individual;
        node.ideSol = individual.obj;
        node.nadSol = individual.obj;
    else
        if node.isLeaf
            node.solSet = [node.solSet,individual];
            node.solNum = node.solNum+1;
            updateIdealNadir(node,individual);
            if node.solNum > MAX_SET_SIZE
                node.isLeaf = false;
                split(node);
            end
        else
            minInd = 0; minDis = inf;
            for braInd = 1:node.braNum
                center = (node.braSet(1,braInd).nadSol+node.braSet(1,braInd).ideSol)/2;
                curDis = sqrt(sum(abs(individual.obj-center).^2));
                if curDis < minDis
                    minDis = curDis;
                    minInd = braInd;
                end
            end
            insert(node.braSet(1,minInd),individual);
        end
    end
end
function updateIdealNadir(node,individual)
    change = false;
    M = length(individual.obj);
    for m = 1:M
        if individual.obj(1,m) < node.ideSol(1,m)
            change = true;
            node.ideSol(1,m) = individual.obj(1,m);
        elseif individual.obj(1,m) > node.nadSol(1,m)
            change = true;
            node.nadSol(1,m) = individual.obj(1,m);
        end
    end
    if change && ~isempty(node.father)
        updateIdealNadir(node.father,individual);
    end
end
function split(node)
    MAX_BRA_SIZE = 5;
    M = length(node.ideSol);
    node.braNum = MAX_BRA_SIZE;
    for braInd = 1:MAX_BRA_SIZE
        newNode = Node(true,1,[],0,[],[],[],node);
        node.braSet = [node.braSet,newNode];
        distance = sum(pdist2(node.solSet.objs,node.solSet.objs));
        [~,maxInd] = max(distance);
        node.braSet(1,braInd).solSet = node.solSet(1,maxInd);
        node.braSet(1,braInd).ideSol = node.solSet(1,maxInd).obj;
        node.braSet(1,braInd).nadSol = node.solSet(1,maxInd).obj;
        node.solSet(maxInd) = [];
        node.solNum = node.solNum-1;
    end
    while node.solNum > 0
        minInd = 0; minDis = inf;
        for braInd = 1:node.braNum
            center = (node.braSet(1,braInd).nadSol+node.braSet(1,braInd).ideSol)/2;
            curDis = sqrt(sum(abs(node.solSet(1,1).obj-center).^2));
            if curDis < minDis
                minDis = curDis;
                minInd = braInd;
            end
        end
        for m = 1:M
            if node.solSet(1,1).obj(1,m) < node.braSet(1,minInd).ideSol(1,m)
                node.braSet(1,minInd).ideSol(1,m) = node.solSet(1,1).obj(1,m);
            elseif node.solSet(1,1).obj(1,m) > node.braSet(1,minInd).nadSol(1,m)
                node.braSet(1,minInd).nadSol(1,m) = node.solSet(1,1).obj(1,m);
            end
        end
        node.braSet(1,minInd).solSet = [node.braSet(1,minInd).solSet,node.solSet(1,1)];
        node.braSet(1,minInd).solNum = node.braSet(1,minInd).solNum+1;
        node.solSet(1) = [];
        node.solNum = node.solNum-1;
    end
end
