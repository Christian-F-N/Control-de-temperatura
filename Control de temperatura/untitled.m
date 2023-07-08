clc;
clear;
close all;
fis = readfis('voltaje2.fis');
entrada = [5,5]; % Valores de entrada para la inferencia difusa
salida = evalfis(entrada, fis);
