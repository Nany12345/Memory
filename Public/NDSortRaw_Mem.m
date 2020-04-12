function [FrontNo,MaxFNo,CountComp,Relation] = NDSortRaw_Mem(varargin)
    PopObj1 = varargin{1};
    PopObj2 = varargin{2};
    Relation = varargin{4};
    [FrontNo,MaxFNo,CountComp] = NDSort_Raw_Mem(PopObj1,PopObj2);
end
function [FrontNo,MaxFNo,CountComp] = NDSort_Raw_Mem(PopObj1,PopObj2)
    CountComp = 0;
    [N1,~] = size(PopObj1);
    [N2,M] = size(PopObj2);
    PopObj = [PopObj1;PopObj2];
    N = N1+N2;
    MaxFNo = 1;
    FrontNo = inf(1,N);
    isD = false(1,N);
    for i = N:-1:N1+1%new added solutions
        if ~isD(i)
            for j = [1:i-1,i+1:N]%all solutions except i
                if ~isD(j)
                    CountComp = CountComp+2;
                    counts = 0;%new added solution is larger than all solutions
                    countb = 0;
                    counte = 0;
                    for m = 1:M
                        if PopObj(i,m) > PopObj(j,m)
                            countb = countb+1;
                        elseif PopObj(i,m) < PopObj(j,m)
                            counts = counts+1;
                        else
                            counts = counts+1;
                            countb = countb+1;
                            counte = counte+1;
                        end
                    end
                    if counte < M%if the two solutions is not equal
                        if counts == M
                            isD(j) = true;
                        elseif countb == M
                            isD(i) = true;
                            break;
                        end
                    end
                end
            end
        end
    end
    FrontNo(~isD) = 1;
end
