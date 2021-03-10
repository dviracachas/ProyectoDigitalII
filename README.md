# W07_Entrega-_final

# Grupo 1

 - Juan Pablo Fiagá
 - Juan David Pabón
 - David Ramírez
 - David Viracachá

Repositorio del proyecto final de la asignatura Electrónica Digital II de la Universidad Nacional de Colombia - Sede Bogotá.
Aquí encontrará el proyecto de Robot Cartográfico propuesto para la clase listo para usar junto con la respectiva documentación de cada módulo implementado.

![Screenshot](/Imagenes/Carro.JPG)

A continuación se presentan las secciones en las que se divide el proyecto, haciendo click en cada enlace encontrará los directorios pertinentes dentro del Git y la documentación relacionada a todas ellas.

## [SoC](/ProyectoDigitalII)
En esta subsección, se encontrará con la carpeta raíz del proyecto, y allí encontrará toda la información correspondiente a cómo se escribe y como se ensambla un SoC que contiene un procesador, un bus Wishbone, módulos escritos en Verilog, entre otras cosas, así como los archivos necesarios para, por ejemplo realizar una nueva compilación del proyecto.

## [Firmware](/ProyectoDigitalII/firmware)
Al seguir este enlace, encontrará la carpeta en la que se encuentra toda la información del firmware creado para nuestro SoC previamente compilado con Litex, explicando el código usado y el algoritmo general que permite al robot recorrer un laberinto y transmitir la información que lo describa adecuadamente por medio de la terminal.

## [Módulos de Verilog](/ProyectoDigitalII/module)
En el enlace de la subsección se encontrará con el directorio en el que están los archivos de Python que integran los módulos con Litex, sin embargo en los listados aquí abajo encontrará los directorios que contienen sus archivos en Verilog acompañados de la explicación de su funcionamiento.
- [Ultrasonido](/ProyectoDigitalII/module/verilog/Ultrasonido(NexysA7))
- [IR](/ProyectoDigitalII/module/verilog/Infrarrojo)
- [Motor](/Motor.md) 
- [PWM](/ProyectoDigitalII/module/verilog/PWM)
- [Cámara](/ProyectoDigitalII/module/verilog/camara)

## [Videos de pruebas de funcionamiento](https://drive.google.com/drive/folders/1TjqHZUeRFE6-v9n2TI3CKVCBduTdpxww?usp=sharing)
Encontrará aquí una carpeta pública de Google Drive con videos tomados en la fase de pruebas del carro, haciendo funciones de prueba detalladas en el [firmware](/ProyectoDigitalII/firmware) junto con sus respectivos resultados en el robot.
