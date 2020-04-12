function [PS, PF] = PSnPF(VertexSet, D, N)
    [M, ~, K] = size(VertexSet);
    
    % Obtain PS
    [X, Y] = ndgrid(linspace(-2, 10, ceil(sqrt(N))));
    nondominated = zeros(N, 1);
    for i=1:K
        polygon = VertexSet(:, :, i);
        ND = inpolygon(X(:), Y(:), polygon(:, 1), polygon(:, 2));
        nondominated(ND) = 1;
    end
    PS = [X(nondominated == 1), Y(nondominated == 1)];
    
    % Obtain PF
    num_PS = length(PS);
    objectives = zeros(num_PS, M);
    for i = 1:num_PS
        point = PS(i, :);
        objectives(i, :) = Inf;
        for j = 1:K
            dist = pdist2(transform(point, D), transform(VertexSet(:, :, j), D));
            objectives(i, :) = min(objectives(i, :), dist);
        end
    end
    PF = objectives;
end

function B = transform(A, D)
    B = repmat(A, 1, D / 2);
end
