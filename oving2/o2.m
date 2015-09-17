clear all
close all
clc

start=0;
slutt=10;
n=linspace(start, slutt, 100);
r=1;
h_g=1;
h_c=10;

GPU=@(n) 2*n/r+5*n/r+5*h_g*n*7*h_g.*log2(n);
CPU=@(n) 5*h_c*n*7*h_c.*log2(n);

figure (1)
hold on
plot(n, GPU(n), '-')
plot(n, CPU(n), '-')
legend('GPU', 'CPU')
hold off