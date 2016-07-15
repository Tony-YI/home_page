function [optD optR] = tsp_ga1(xy, times)

%times = 100;

%x = randperm(100,25);
%y = randperm(100,25);
%xy = [x' y'];

% Initiate the population
popSize = 20; % Population size is 20
groupSize = 4; % Group size is 4
numCity = 25; % number of different cities is 25
pop = zeros(popSize, numCity); % A matrix with 20 rows and 25 columns
for i = 1:popSize
	pop(i, :) = randperm(numCity);
end

% Initiate the distance matrix
disMatx = zeros(numCity, numCity);
for j = 1:numCity
	for k = 1:numCity
		disMatx(j, k) = sqrt((xy(j, 1) - xy(k, 1))^2 + (xy(j, 2) - xy(k, 2))^2); 
	end
end

% Run the GA
globalMin = Inf;	% Global minimum distance
totalDist  = zeros(1, popSize); % Total distance for each solution
disHistory = zeros(1, times);
tempPop = zeros(4, numCity);
newPop = zeros(popSize, numCity); % New population after mutaions

for t = 1:times
	% Calculate the total distance
	for p = 1:popSize
		d = disMatx(pop(p, numCity), pop(p, 1));
		for q = 2:numCity
			d = d + disMatx(pop(p, q - 1), pop(p, q));
		end
		totalDist(p) = d;
	end

	% Find the best solution in the population
	[minDist index] = min(totalDist);
	disHistory(t) = minDist;
	if minDist < globalMin
		globalMin = minDist;
		optRoute = pop(index, :); % Best solution for now
	end

	% GA opeartors
	randomOrder = randperm(popSize);
	for p = 4:4:popSize
		group = pop(randomOrder(p - 3:p), :);
		groupDist = totalDist(randomOrder(p - 3:p));
		[groupMinDist groupIndex] = min(groupDist);
		groupOptRoute = group(groupIndex, :); % Get the best solution in one group
		routeInsertionPoints = sort(ceil(numCity*rand(1,2))); % Randomly get the mutaion point
		I = routeInsertionPoints(1);
		J = routeInsertionPoints(2);
		for k = 1:4 % Mutate the best and get three new solution
			tempPop(k, :) = groupOptRoute;
			switch k
			case 2 % Flip
				tempPop(k, I:J) = tempPop(k, J:-1:I);
			case 3 %Swap
				tempPop(k,[I J]) = tempPop(k, [J I]);
			case 4 %Slide
				tempPop(k, I:J) = tempPop(k, [I+1:J I]);
			end
		end
		newPop(p-3:p, :) = tempPop;
	end
	pop = newPop;
end

% Plot the optRoute
figure('Name', 'TSP_GA Result');
subplot(1, 2, 1);
plot(xy(:,1), xy(:,2), '.');
title('City Locations');
subplot(1, 2, 2);
rte = optRoute([1:numCity 1]);
plot(xy(rte, 1), xy(rte, 2), 'r.-');
title(sprintf('Final Minimum Distance = %1.4f', globalMin));
optD = globalMin;
optR = optRoute;
end