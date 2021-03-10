# SERVOMOTOR

Para el movimiento del radar se hizo uso de un servomotor donde se realizaban los respectivos giros de 90 grados y -90 grados, para asi realizar el mapeo mediante la camara y tambien el sensor ultrasonido. Para lograr que se den estos movimientos,  se debe generar una señal PWM que dependera del angulo al que se requiera realizar el movimiento. Teniendo esto en cuenta se procedio a diseñar el driver para este periferico, donde inicialmente se realizo un divisor de frecuencia para el servomotor, el cual llevaria el reloj del sistema de una frecuencia de 100MHz hasta una frecuencia de 20kHz. Luego se procedio a realizar la generacion de la señal PWM como se observa a continuacion: 

```verilog

 if(ENABLE==1)
  begin  
        contador<=contador+1;
    
  
        if(grados==2'b00) begin
         ancho<=30; end            
        else if(grados==2'b01) begin
         ancho<=50; end                
        else if(grados==2'b10) begin
         ancho<=10; end               
        else begin
         ancho<=30; end 
         
        //Creación del pulso modulado      
        if(contador<ancho) begin 
        pwm<=1; end
        else if(contador>=ancho) begin
        pwm<=0; end 
        else  begin
        pwm<=0; end
    
        if(contador>=top-1)begin 
        contador<=0; end
                                                       
  end

```
Lo que se busca es que para cada una de las opciones de giro, se debe generar una señal PWM con cierto ancho de pulso, entonces dependiendo de cada ancho se le asignara un numero de ciclos de reloj equivalente, y mientras ese contador llega a cero se mantendra la señal de salida en  1, y cuando se alcance ese valor la señal pasara a cero; obteneindo de este modo la PWM requerida para el movimiento del servomotor. Ahora usando un case sabremos cuantos grados debera girar el motor dependiendo de la eleccion que se realice, tal como se muestra a continuacion: 


``` verilog

case({orden[1],orden[0]})
		2'b00:	
		begin
			grados <=  0; //00 van a ser 0grados
	      	
		end
		2'b01:
		begin
			grados <=  1;  //01 van a ser 90grados
		end
		
		2'b10:
		begin

			grados <=  2;  //10 van a ser -90grados
		end
		
		default:
		begin 	

			grados<=  2'b00;   //11 va a ser un indicador de error
		end
	endcase
	
```

Para este caso se usara la variable orden que sera de 3 bits, donde el bit mas signifitivo sera usado como el reset del sistema, y los dos bits restantes seran usados para realizar la eleccion del giro que se llevara a cabo. Por lo tanto, en caso de que estos bits sean 00, no habra giro en el servomotor; si son 01 se hara un giro de 90 grados, para el caso en el que se tenga 10 el giro sera de -90 grados, y para el resto de hara un giro de cero grados como default. Con todos estos modulos que vimos con anterioridad ya tendremos el driver que hara que nuestro servomotor funcione apropiadamente. Ahora para la apropiada integracion con Litex, debemos definir nuestras entradas y salidas, como se muestra en la siguiente imagen: 

![Screenshot](/Imagenes/ModuloPWM.png)

Para este periferico, solo tendremos un pin que se comunicara con el mundo exterior, y sera por donde emitamos nuestra señal PWM, tambien tendremos el reloj que sera el mismo del sistema, y finalmente la variable Orden, la cual sera almacenada en memoria, pues es mediante esta como se contralara el movimiento del servotor; a continuacion podemos observar la funcionalidad de este modulo en python: 

```python

class PWM(Module,AutoCSR):
    def __init__(self, pwm):
        self.clk = ClockSignal()   
        self.orden = CSRStorage(3)
        self.pwm = pwm

```

y estas variables tambien deben ser instanciadas para que puedan ser asignadas posteriormente en el buildSoCproject.py esto se realiza como se presenta a continuacion: 

```python

self.specials +=Instance("BloquePWM",
            i_clk = self.clk,
            i_orden = self.orden.storage,
            o_pwm = self.pwm,
        )

```

Dado que ya hemos realizado la construccion del modulo PWM.py donde se definen las entradas y salidas, ahora debemos instanciarlo en el buildSoCproject.py, esto con el fin de tambien crear el mapa de memoria donde se almacenara el registro de la variable orden, esto se realiza como se muestra a continuacion: 

```python

SoCCore.add_csr(self,"pwm_cntrl")
self.submodules.pwm_cntrl = pwm.PWM(platform.request("pwm_out"))

```
