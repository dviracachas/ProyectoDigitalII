# Infrarrojo



El sensor infrarrojo utilizado fue el POLOLU-2459, este periferico se usa para limitar variables en el movimiento del robot, de este modo tendremos menos grados de libertad, para ello implementamos mediante este dispositivo un detector de linea el cual permite al carro idientificar cuando entra a una nueva cuadricula dentro del leberinto. 
Para la construccion del Driver de este dispositivo debemos tener en cuenta que posee 3 pines donde 2 de ellos son la alimentacion y el tercer pin es para el envio y la recepcion de informacion. Es a partir del funcionamiento del ultrasonido a emplear que se debe consturir el driver del mismo.

A partir del diagrama de circuito: 


![Screenshot](/Imagenes/circuito_IR.png)


Se puede derivar que el funcionamiento del circuito consiste en medir el tiempo de carga del condensador siendo este dependiente de la luz incidente en el fototransistor. Así, pódemos derivar que la distancia de un objeto es proporcional al tiempo en que la diferencia de potencial del condensador demora de pasar de 0 hasta 1. 

para activar el sensor es necesario primero descargar el condensador lo que se hace con la señal de salida trigger, cuando la señal de salida trigger es enviada el condensador se descarga con la resistencia de entrada, una vez descargado se cambia la salida de baja a alta impedancia lo que permite al condensador cargarse con el fototransistor. el tiempo en el que se demora la carga del condensador es usado para calcular la distancia a un objeto o para significar su detección, si esta carga se demora más que un tiempo especifico el ciclo se reinicia y no se da detección.

El codigo en verilog del hardware del infrarrojo puede encontrarse [aqui](ProyectoDigitalII/module/verilog/Infrarrojo/modulo_ir.v)

Con esta información en mente procedemos con el codigo, iniciando con los pines de entrada y salida (en donde unicamente ir_io sale fisicamente del SoC) y los registros internos:

```verilog
module modulo_ir(
    input clk,
    input rst,
    inout ir_io,                // Pin de descarga del capacitor y lectura.  
    output descarga,            // Salida para uso interno   
    output [7:0] distancia      // Salida legible desde el procesador
    );
    
    reg [15:0] contador;        // Contador general
    reg leer;                   // Bit que activa la lectura
    reg reg_descarga;           // Registro de descarga
    reg [15:0] cont_ancho;      // Contador para el ancho de pulso de lectura
    reg [15:0] ancho_pulso;     // Registro de ancho de pulso
    reg [7:0] reg_distancia;    // Registro con la distancia medida.
    
```
ir_io es el unico de los pines de salida y de entrada que será conectado por fuera del soc, el resto de valores o se conectan al reloj interno de la tarjeta o de la función de reset o se guardan en memoria para su uso en otros modulos o en el software de control.

Para obtener un pin de entrada y salida es necesario tener un triestado el cual varia entre el estado de alta impedancia `1'bz` cuando se da la orden de lectura al estado `descarga` cuando esta orden no está presente.
```v
assign ir_io = (leer) ? 1'bz : descarga;
```

Para contar el numero de ciclos para la carga del condensador tenemos primeramente el reset el cual reinicia todos los registros y los contadores y la orden de lectura:

```V
always@(posedge clk) begin
        // Reinicio
        if (rst) begin
            contador = 16'b0;
            leer = 1'b0;
            ancho_pulso = 16'b0;
            cont_ancho = 16'b0;
            reg_distancia = 8'b0;
        end
```

cada vez que el clock marca una pendiente positiva el contador se incrementa en 1:

```V
contador = contador + 1;
```

para la Nexys 4 eso es una cuenta cada 10ns. Para este caso necesitamos que el condensador se deascargue por 1.3us en un ciclos de 655us lo que corresponde a 130 ciclos de reloj

```V
 if (contador < 129) begin
            leer = 0;
            reg_descarga = 1; 
        end else begin
            leer = 1;
            reg_descarga = 0;
            // Obtenemos el ancho de pulso
            if (ir_io) begin
                cont_ancho = cont_ancho + 1;
            end
        end
```

A partir de 130 ciclos el condensador ya se encuentra descargado y se procede a iniciar la carga, se cambia a `leer = 1` lo que cambia la salida a entrada de alta impedancia e inicia la carga del condensador. Cuando el condensador pasa determinado nivel de carga el valor de la entrada `ir_io` pasa de 0 a 1. Y es precisamente aqui cuando el ancho empieza a contarse hasta el final del ciclo de 655us
```V
if (contador >= 32750) begin
            contador = 16'b0;
            ancho_pulso = cont_ancho;
            cont_ancho = 16'b0;
            reg_distancia = ancho_pulso;
        end
```

en este final del ciclo el valor de los contadores es reiniciado a 0 y el valor del ancho del pulso es guardado en registro que posteriormente es enviado a ser guardado en los registros y posteriormente en la memoria.

Una vez terminado el ciclo, el modulo es añadido al sistema donde litex emplea el archivo [infrarrojo.py](ProyectoDigitalII/module/infrarrojo.py) para unir los datos de entrada y salida del modulo a el reloj de la fpga y para enviar el valor de la distancia a la memoria

```py
class Infrarrojo(Module,AutoCSR):
    def __init__(self, ir_io):
        self.clk = ClockSignal()   
        self.rst = ResetSignal()  

        self.ir_io = ir_io

        #self.descarga = descarga
        self.distancia = CSRStatus(8)

        self.specials +=Instance("modulo_ir",
            i_clk = self.clk,
            i_rst = self.rst,
            io_ir_io = self.ir_io,
            o_distancia = self.distancia.status,
        )
```

y el pin `ir_io`es adignado a una de las salidas fisicas de la fpga en el archivo [buildSoC](ProyectoDigitalII/buildSoCproject.py)
```py
SoCCore.add_csr(self,"infrarrojo_cntrl")
		self.submodules.infrarrojo_cntrl = infrarrojo.Infrarrojo(platform.request("ir_inout"))
```
