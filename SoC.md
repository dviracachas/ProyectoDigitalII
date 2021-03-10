# Programar un SoC en una FPGA usando Litex
Este proyecto pasa por la creación de un SoC (System On Chip) que integra gran variedad de cosas de acuerdo a nuestras necesidades. En nuestro caso, hacemos uso de un procesador de AMD y por medio de buses de comunicación lo conectamos con memorias, registros y módulos hechos en el lenguaje Verilog que corresponden a los implementos necesarios para formar un robot cartógrafo que es capaz de recorrer cierta variedad de laberintos de manera autónoma.
El proceso para realizar un SoC se realizó con el uso de una herramienta de código abierto llamada Litex, la cual nos permite ensamblar de una manera sencilla todos los módulos separados de FPGA que podamos llegar a tener (claro, sin exceder las capacidades de la tarjeta). 
En esta sección, veremos cómo es el proceso para lograr esta integración.

## Crear módulos compatibles con Litex
Como mencionamos anteriormente, podemos unir módulos que ya hayamos creado (y por supuesto, probado) en un SoC mediante Litex, para esto, es necesario hablar en su idioma. Existen módulos ya realizados para Litex, o incluso, se pueden programar nuevos usando sus directrices; sin embargo, en este curso optamos por haber creado módulos escritos en Verilog de antemano, por lo que contamos con este tipo de escritura en una primera instancia. Para hacer uso de ellos tendremos que crear un archivo de Python (lenguaje en el que está escrito Litex) en el cuál tengamos la estructura:

1. Referencias a las librerías de Litex y Migen necesarias
2. Clase con el nombre de nuestro módulo
3. Función constructora

De estos numerales notamos que solo la función constructora supone un esfuerzo por describir el módulo, por lo cuál ahondaremos en esta. Es importante recordar que no todas las entradas y salidas de un módulo, a diferencia de que lo implementásemos de manera solitaria, necesitan un puerto físico para ser accedidos. En un SoC podemos acceder a las entradas y salidas de un módulo mediante registros internos que pueden ser accedidos por el procesador mediante un bus. Dicho esto, es importante que al declarar la función constructora, como argumentos usemos los pines que tendrán que tener conexiones físicas a pines externos.
```python
class Ejemplo(Module,AutoCSR):
    def __init__(self, pinExterno1, pinExterno2):
```
Una vez hagamos hecho esto, procedemos a asignar ahora todos los pines necesarios para el correcto funcionamiento del módulo. Es importante aclarar que en caso de que se usen varios ficheros de Verilog para realizar un solo módulo, en esta función constructora solo tendremos que usar las entradas y salidas del archivo Top. Dicho esto, analicemos el siguiente fragmento de código:
```python
class Ejemplo(Module,AutoCSR):
    def __init__(self, pinExterno1, pinExterno2):
        self.clk = ClockSignal()   
        self.rst = ResetSignal()

        self.pinExterno1 = pinExterno1
        self.pinExterno2 = pinExterno2

        self.input = CSRStorage(3)
        self.done = CSRStatus(1)
        self.output = CSRStatus(8)

        self.specials +=Instance("ejemplo",
            i_clk = self.clk,
            i_rst = self.rst,
            i_input = self.input.storage,
            i_pinExterno1 = self.pinExterno1,
            o_pinExterno2 = self.pinExterno2,
            o_done = self.done.status,
            o_output = self.output.status,
        )
```
De aquí, podemos apreciar que tenemos un módulo que necesita de una señal de `clock`, `reset`, hace uso de dos pines externos, recibe una señal `input` de 3 bits y produce outputs relacionados a una señal `done` y a una salida `output` de 8 bits. ¿Cómo saber esto? Las señales ClockSignal() y ResetSignal() están dadas por las librerías importadas y conectarán estos pines internamente a las señales generales que son usadas también por el procesdor. Los pines externos se asignan utilizando los argumentos de la función constructura y las entradas y salidas se pueden, en este caso catalogar en de solo lectura o de lectura y escritura. Para los registros de solo lectura usamos la función `CSRStatus()` cuyo argumento es el número de bits que usará y, para las señales de lectura y escritura, de la misma manera, usaremos la función `CSRStorage()`. Usando de manera adecuada estas funciones, Litex se encargará de asignar espacio en memoria para todos nuestros pines y por tanto los tendremos accesibles desde nuestro procesador.

Ahora, veamos que en el espacio de abajo, resta instanciar el nombre de nuestro módulo de verilog y escribir variables que representen nuestros pines previamente definidos, teniendo cuidado con los registros, pues a estos habrá que agregarles su respectivo `status` o `storage` seguido de un punto.

## Compilando nuestro SoC
Una vez que tengamos nuestros módulos en "lenguaje" de Litex, podemos proceder a agregarlos a nuestro SoC. Con los archivos que nos fueron dados, este proceso ocurre en el archivo [buildSoCproject.py](https://github.com/unal-edigital2/w07_entrega-_final-grupo01/blob/main/ProyectoDigitalII/buildSoCproject.py). En este tuvimos como base un procesador conectado mediante un bus Wishbone a los LED's, switches, puerto serial, entre otras cosas de la tarjeta FPGA. Para agregar nuestros propios módulos, tenemos que asegurarnos de añadir en el encabezado nuestros archivos de Python creados con este fin. Esto se vería de la siguiente manera.
```python
from module import ejemplo
```
Y, de la misma manera, tenemos que asegurarnos de agregar todas las direcciones en las que podemos encontrar nuestros archivos de Verilog en el cuerpo del código:
```python
platform.add_source("module/verilog/Ejemplo.v")
```
Habiéndonos asegurado de esto necesitamos hacer un último paso previo, y es el de declarar nuestros pines de la FPGA para las entradas y salidas externas. Esto se hace en un archivo que debió ser previamente indexado en el encabezado, en el cual encontramos todos nuestros constraints. Para agregar una nueva entrada seguimos la plantilla, asignando un nombre arbitrario (pero único) a los pines y relacionándolo con la dirección del mismo en la tarjeta:
```python
("pinExterno1", 0, Pins("E18"), IOStandard("LVCMOS33")),
("pinExterno2", 0, Pins("G17"), IOStandard("LVCMOS33")),
```
De esta manera, finalmente en `buildSoCproject.py` agregamos la siguiente entrada
```python
#Ejemplo
SoCCore.add_csr(self, "ejemplo")
self.submodules.ultrasonido = ejemplo.Ejemplo(platform.request("pinExterno1"), platform.request("pinExterno2"))
```
La cual corresponde a agregar un objeto a la lista de submódulos del SoC y de crearlo teniendo en cuenta que los pines externos `"pinExterno1"` y `"pinExterno2"` que serán buscados en el archivo de las constraints, serán conectados a la dirección asociada a esta.

## Compilar y subir el SoC
Con estos archivos modificados adecuadamente, basta con ejecutar `buildSoCproject.py` para obtener como resultado un archivo .bit (en el caso de las tarjetas de Xilinx) que puede ser subido a la FPGA. En Linux, esto se puede hacer abriendo una terminal en el directorio raíz del proyecto y ejecutando el comando:
```ssh
python3 buildSoCproject.py
```
En nuestro caso, contamos con un archivo `load.py` el cual podemos ejecutar para cargar el archivo compilado a la FPGA mediante un puerto USB serial.

## ¿Qué sigue?
Recordemos que en este punto, tenemos un SoC, es decir, un hardware con la capacidad de ejecutar código y que se encuentra conectado a los módulos que le asignamos. Sin embargo, la capacidad no pasa de ser eso, para poder hacer uso de nuestro SoC, lo que nos resta es escribir software que sea ejecutable por él. En este caso, podemos escribir un firmware en lenguaje C, por lo que podemos hacer uso de toda su sintaxis sin inconvenientes. Para hacer uso de los periféricos, es importante revisar el archivo [CSR.h](https://github.com/unal-edigital2/w07_entrega-_final-grupo01/blob/main/ProyectoDigitalII/build/nexys4ddr/software/include/generated/csr.h) generado al ejecutar `buildSoCproject.py` ya que en este archivo encontraremos todas las funciones en C generadas por Litex y Migen para nuestro uso al momento de escribir software. En este archivo encontraremos cómo leer y escribir los registros que requerimos, por lo que ahora se encuentran a nuestra completa disposición. Como dato final cabe recalcar que estas lecturas y escrituras se pueden realizar usando el sistema decimal. En este punto depende de nosotros desarrollar una aplicación para nuestro SoC, siempre que logremos escribir un firmware que no supere los límites de memoria y/o procesamiento para nuestra tarjeta.
