% This function generates the city locations and
% make sure all the city have different locations [x y]

function xy = genCityLocation(num_city)
bound = 2 .* num_city;
x = randperm(bound, num_city);
y = randperm(bound, num_city);
xy = [x' y'];
end