# Control de Temperatura con Lógica Difusa y Arduino

Este proyecto implementa un sistema de control de temperatura utilizando lógica difusa y Arduino. El objetivo es regular la temperatura ambiente mediante un motor de 5V, luces LED y un display. La lógica difusa se implementa inicialmente en MATLAB y luego se convierte en código Arduino. 

## Requisitos de Hardware

- Sensor de temperatura DS18B20
- Motor de 5V
- Luces LED
- Display

## Requisitos de Software

- MATLAB
- Arduino IDE

## Diagrama de Conexiones

A continuación se muestra el diagrama de conexiones entre el sensor de temperatura, el motor, las luces LED y el display:

                 +-----------+
      +----------|   Motor   |
      |          +-----------+
      |+---------|---------+
      | Sensor | || de Temp.| Arduino |+---------|---------+|
| +-----------++----------| Luces LED |+-----------++-------------------+
      |      Display      |
      +-------------------+

## Pasos de Implementación

1. **Configuración del hardware**: Conecta el sensor de temperatura, el motor, las luces LED y el display a las salidas correspondientes en Arduino según el diagrama de conexiones.

2. **Implementación de la lógica difusa en MATLAB**: Utiliza MATLAB para desarrollar la lógica difusa del controlador de temperatura. Define las variables de entrada (temperatura ambiente) y salida (potencia del motor) junto con los conjuntos difusos y las reglas difusas. Ajusta y optimiza los parámetros según sea necesario.

3. **Generación del código Arduino**: Utilizando el *Fuzzy Logic Toolbox* de MATLAB, genera el código Arduino a partir de la lógica difusa desarrollada en el paso anterior. Este código generará la implementación de la lógica difusa en Arduino, incluyendo las funciones de pertenencia y las reglas difusas.

4. **Carga del código en Arduino**: Abre el Arduino IDE, crea un nuevo proyecto y copia el código generado por MATLAB. Conecta Arduino a tu computadora y carga el código en la placa Arduino.

5. **Pruebas y ajustes**: Realiza pruebas del sistema de control de temperatura y realiza ajustes en las reglas difusas y las funciones de pertenencia según sea necesario para obtener un control preciso y confiable de la temperatura.

## Contribuciones

Si deseas contribuir a este proyecto, siéntete libre de hacer fork del repositorio y enviar pull requests con tus mejoras.

## Licencia

Este proyecto se distribuye bajo la Licencia MIT. Consulta el archivo [LICENSE](./LICENSE) para obtener más detalles.
