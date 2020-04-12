classdef Node < handle
    properties
        isLeaf;
        solNum;
        solSet;
        braNum;
        braSet;
        ideSol;
        nadSol;
        father;
    end
    methods
        function obj = Node(isLeaf,solNum,solSet,braNum,braSet,ideSol,nadSol,father)
            obj.isLeaf = isLeaf;
            obj.solNum = solNum;
            obj.solSet = solSet;
            obj.braNum = braNum;
            obj.braSet = braSet;
            obj.ideSol = ideSol;
            obj.nadSol = nadSol;
            obj.father = father;
        end
        function ndSolSet = getNdSol(obj)
            ndSolSet = [];
            if obj.isLeaf
                ndSolSet = obj.solSet;
            else
                for braInd = 1:obj.braNum
                    ndSolSet = [ndSolSet,getNdSol(obj.braSet(braInd))];
                end
            end
        end
    end
end
