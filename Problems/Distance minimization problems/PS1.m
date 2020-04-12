classdef PS1 < PROBLEM
% <problem> <Distance minimization>
% The multi-point distance minimization problem
% lower --- -100 --- Lower bound of decision variables
% upper ---  100 --- Upper bound of decision variables

%------------------------------- Reference --------------------------------
% M. Koppen and K. Yoshida, Substitute distance assignments in NSGA-II for
% handling many-objective optimization problems, Proceedings of the
% International Conference on Evolutionary Multi-Criterion Optimization,
% 2007, 727-741.
%------------------------------- Copyright --------------------------------
% Copyright (c) 2018-2019 BIMK Group. You are free to use the PlatEMO for
% research purposes. All publications which use this platform or any code
% in the platform should acknowledge the use of "PlatEMO" and reference "Ye
% Tian, Ran Cheng, Xingyi Zhang, and Yaochu Jin, PlatEMO: A MATLAB platform
% for evolutionary multi-objective optimization [educational forum], IEEE
% Computational Intelligence Magazine, 2017, 12(4): 73-87".
%--------------------------------------------------------------------------

    properties(Access = private)
        Points; % Vertexes
    end
    methods
        %% Initialization
        function obj = PS1()
            % Parameter setting
            [lower,upper] = obj.Global.ParameterSet(-10,10);
            if isempty(obj.Global.M)
                obj.Global.M = 10;
            end
            if isempty(obj.Global.D)
                obj.Global.D    = obj.Global.M;
            end
            obj.Global.lower = zeros(1, obj.Global.D) + lower;
            obj.Global.upper = zeros(1, obj.Global.D) + upper;
            obj.Global.encoding = 'real';
            % Generate vertexes
            obj.Points = [eye(obj.Global.M),zeros(obj.Global.M,obj.Global.D-obj.Global.M)];
        end
        %% Calculate objective values
        function PopObj = CalObj(obj,PopDec)
            PopObj = (pdist2(PopDec,obj.Points)).^2;
        end
        %% Sample reference points on Pareto front
        function P = PF(obj,N)
            P     = rand(N,obj.Global.D);
        end
        %% Draw special figure
        function Draw(obj,PopDec)
            cla; Draw(PopDec);
            plot(obj.Points([1:end,1],1),obj.Points([1:end,1],2),'-k','LineWidth',1.5);
            plot(obj.Points(:,1),obj.Points(:,2),'ok','MarkerSize',6,'Marker','o','Markerfacecolor',[1 1 1],'Markeredgecolor',[.4 .4 .4]);
            xlabel('\itx\rm_1'); ylabel('\itx\rm_2');
        end            
    end
end