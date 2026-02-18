clc; clear; close all;

% Define parameters
a = 0.5;  % Aspect ratio of the tube
grid_size = 50;  % Grid resolution for plotting
N = 50;  % Number of terms in series expansion

x_star = linspace(-1, 1, grid_size);  % X range
y_star = linspace(-a, a, grid_size);  % Y range
[X, Y] = meshgrid(x_star, y_star);

% Compute alpha_n values.
n = (1:N)';  % Series index
alpha_n = (2*n - 1) * pi / (2 * a);

% Compute the velocity profile w*
w_star = 0.5 * (a^2 - Y.^2);  % First term in solution

for i = 1:N
    term = ((-1)^n(i) / alpha_n(i)^3) * cos(alpha_n(i) * Y) .* (cosh(alpha_n(i) * X) / cosh(alpha_n(i)));
    w_star = w_star + (2/a) * term;
end

% Plot velocity contours:
figure;
contourf(X, Y, w_star, 20, 'LineColor', 'none');
colorbar;
xlabel('x^*');
ylabel('y^*');
title('Velocity Contours for Flow in a Rectangular Duct (a = 0.5)');
