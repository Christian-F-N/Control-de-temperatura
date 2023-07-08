% Ruta del archivo Excel
excelFile = 'tabla.xlsx';

% Leer el archivo Excel
[numeros, ~] = xlsread(excelFile);

% Obtener los valores de las dos primeras columnas
columna1 = numeros(:, 1);
columna2 = numeros(:, 2);

% Cargar el archivo de función FIS
fis = readfis('voltaje2.fis');

% Inicializar una matriz para almacenar los resultados
resultados = zeros(length(columna1), 1);

% Recorrer los valores de las columnas y evaluar la función FIS
for i = 1:length(columna1)
    % Reemplazar los valores de X y Y en la función FIS
    entrada = [columna1(i), columna2(i)];
    salida = evalfis(entrada, fis);
    
    % Almacenar el resultado en la matriz de resultados
    resultados(i) = salida;
end

% Guardar los resultados en un archivo Excel
resultadosFile = 'resultados.xlsx';
xlswrite(resultadosFile, resultados);
