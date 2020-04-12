function [PS,PF] = CalculatePSnPF(row, col, distance, M, D, step_size)
    VertexSet = CreatePolygons(row, col, distance, M);
    % 1. generate uniform samples
    lo = -1;
    hi_x = (col-1)*distance + 1;
    hi_y = (row-1)*distance + 1;

    [X, Y] = ndgrid(lo:step_size:hi_x, lo:step_size:hi_y); % grid coordinates
    N = row * col;
    sz = size(X);
    num_samples = sz(1) * sz(2);

    % 2. calculate objective vector for all samples
    objectives = zeros(num_samples, M);
    for i = 1:num_samples
        point = [X(i), Y(i)];
        objectives(i, :) = Inf;
        for j = 1:N
            obj = pdist2(transform(point, D), transform(VertexSet(:, :, j), D));
            objectives(i, :) = min(objectives(i, :), obj);
        end
    end

    % 3. test pareto optimality
    dominated = zeros(num_samples, 1);
    for i = 1:num_samples
        a = round(objectives(i, :), 3);
        % check if a is dominated by some value
        for j = 1:num_samples
            if j ~= i
                b = round(objectives(j, :), 3);
                diff = b - a;
                % b dominates a if all <= 0 -- minimization problem
                if all(diff <= 0) && any(diff < 0)
                    dominated(i) = 1;
                end  
            end
        end
    end

    % 4. return true pareto front
    PS = [X(dominated == 0), Y(dominated == 0)];
    PF = objectives(dominated == 0, :);
end

function B = transform(A, D)
    B = repmat(A, 1, D / 2);
end

