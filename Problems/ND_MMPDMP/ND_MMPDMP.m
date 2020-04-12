classdef ND_MMPDMP < PROBLEM
% <problem> <Distance minimization>
% The multi-modal multi-objective polygon distance minimization problem
%
% lower --- -100 --- Lower bound of decision variables
% upper ---  100 --- Upper bound of decision variables
% row   ---   2   --- Number of polygon in a row
% col   ---  2   --- Number of polygon in a column
% distance ---  5 --- Distance between polygons center

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
        Polygons;  % Polygons
        row;
        col;
        distance;
    end

    methods
        %% Initialization
        function obj = ND_MMPDMP()
            % Parameter setting
            [lower, upper, obj.row, obj.col, obj.distance] = obj.Global.ParameterSet(-100, 100, 2, 2, 5);
            if isempty(obj.Global.M)
                obj.Global.M = 6;
            end

            % Dimension of decision space
            if ~isempty(obj.Global.D)
                if mod(obj.Global.D, 2) ~= 0
                    error('D should be and even integer >= 2');
                end
            else
                obj.Global.D = 2;
            end

            obj.Global.lower = zeros(1, obj.Global.D) + lower;
            obj.Global.upper = zeros(1, obj.Global.D) + upper;
            obj.Global.encoding = 'real';

            % Generate polygons
            obj.Polygons = CreatePolygons(obj.row, obj.col, obj.distance, obj.Global.M);
        end

        %% Calculate objective values
        function PopObj = CalObj(obj, PopDec)
            N = obj.row * obj.col;

            % Min distance to point i in each polygon
            PopObj = Inf;
            for i=1:N
                % Transform 2d to D-dimensional plane.
                vertices = repmat(obj.Polygons(:, :, i), 1, obj.Global.D / 2);

                val = pdist2(PopDec, vertices);
                PopObj = min(PopObj, val);
            end
        end

        %% Sample reference points on Pareto front
        function P = PF(obj, N)
            if obj.distance > 4
                [~, P] = PSnPF(obj.Polygons, obj.Global.D, N);
            else
                [~, P] = CalculatePSnPF(obj.row, obj.col, obj.distance, obj.Global.M, obj.Global.D, 0.1);
            end
        end

        %% Draw pareto set
        function Draw(obj, PopDec)
            cla; 
            dim = length(PopDec(1, :));
            xlabel("$$x_1$$", 'Interpreter', 'latex', 'FontSize', 20);
            ylabel("$$x_2$$", 'Interpreter', 'latex', 'FontSize', 20);
            axis equal
            box on
            hold on

            % Draw obtained solutions
            %% Transform decision variable back to 2-dimenisonal vectors
            X2d = PopDec(:, dim/2); 
            Y2d = PopDec(:, dim/2+1);
            scatter(X2d, Y2d, 15, 'black');

            % Draw polygons
            K = size(obj.Polygons);
            for i=1:K(3)
                points = obj.Polygons(:, :, i);
                X = points(:, 1);
                Y = points(:, 2);
                
                % vertices
                scatter(X, Y, 50, 'red', 'filled')
                polygon = polyshape(X, Y);
                pg = plot(polygon);
                pg.LineWidth = 2;
                pg.FaceColor = [0.85, 0.85, 0.85];
            end

            hold off
        end
    end
end