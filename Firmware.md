# Firmware
## Funciones de prueba
Una vez construído nuestro SoC, tenemos a nuestra disposición todos los módulos construídos, para usarlos de la manera que definamos. Para aprovecharlos, debemos escribir el software que los va a controlar y de esta manera los pondrá a nuestra orden.

Para construir nuestro firmware, tenemos que tener claro cómo funcionan nuestros módulos y qué podemos leer y escribir de ellos. Sabiendo esto, Litex y Migen se han encargado de abstraer estos procesos mediante funciones generadas que se encuentran en el archivo [CSR.h](https://github.com/unal-edigital2/w07_entrega-_final-grupo01/blob/main/ProyectoDigitalII/build/nexys4ddr/software/include/generated/csr.h) que podremos utilizar al escribir el programa que ejecutará nuestro SoC.

En nuestro caso, se escribieron funciones de prueba para cada módulo implementado con el objetivo de comprobar su respectiva operabilidad y, por supuesto, el programa principal con el cual el carro se encontrará en capacidad de recorrer un laberinto, reconocer su entorno y transmitir la información encontrada a un usuario por medio de la terminal.

A continuación presentaremos las funciones integradas junto con sus respectivas explicaciones.

### test_ir()

```C
static void test_ir(void){
	while(!(buttons_in_read()&1)) {
		leds_out_write(infrarrojo_cntrl_distancia_read());
		delay_ms(50);
		}
}
```
En esta función vamos a presentar mediante los LED's integrados de la FPGA la lectura obtenida por el sensor infrarrojo en intervalos de 50ms hasta que presionemos un pulsador de su tablero.
### test_us()
```C
static void test_us(void){
	while(!(buttons_in_read()&1)) {
		ultrasonido_orden_write(1);
		bool done = false;
		while(!done){
			done = ultrasonido_done_read();
		}
		leds_out_write(ultrasonido_d_read());
		ultrasonido_orden_write(0);
		delay_ms(50);
		}
}
```
En esta función, haremos algo similar a lo hecho con nuestra prueba del funcionamiento del sensor infrarrojo, ahora con el sensor de ultrasonido. Este difiere en que el módulo de ultrasonido no tiene lecturas repetidas sino que se le debe dar la orden realizar todo el proceso relacionado con hacer la respectiva medición y de la misma manera establecer un reset para que este esté disponible para hacerla nuevamente.
### pwm_test()
```C
static void test_pwm(void){
	pwm_cntrl_orden_write(4);
	delay_ms(3000);
	pwm_cntrl_orden_write(5);
	delay_ms(3000);
	pwm_cntrl_orden_write(6);
	delay_ms(3000);
	pwm_cntrl_orden_write(4);
	delay_ms(3000);
	pwm_cntrl_orden_write(5);
	delay_ms(3000);
	pwm_cntrl_orden_write(6);
}
```
Con esta función, hacemos uso de la entrada "orden" previamente definida para el módulo PWM que nos permite girar un servomotor a -90°, 0° y 90°. 
### uart1_test()
```C
static void uart1_test(void){
	while(!(buttons_in_read()&1)) {
		uart1_write('H');
		delay_ms(50);
		uart_write(uart1_read());
		}
}
```
En esta función, probaremos la UART que implementamos en pines arbitrarios de la FPGA, para ello, ponemos los respectivos pines de Rx y Tx en corto, pues probaremos de esta manera en la consola que tanto la escritura como la lectura de estos es correcta.
### rotate_car()
```C
if (right){
	printf("Rotación a la derecha\n");
	uart1_write('J');
	delay_ms(350);
	uart1_write('I');
	delay_ms(500);
} else {
	printf("Rotación a la izquierda\n");
	uart1_write('H');
	delay_ms(350);
	uart1_write('I');
	delay_ms(500);
}
```
Esta función nos permite rotar el carro a la izquierda o a la derecha, para ello, hacemos uso de la UART1 que escribirá al Arduino la instrucción relacionada al giro de los motores con este fin. Al escribir la instrucción, le daremos un espacio de 350ms que es el tiempo que en nuestro caso resultó efctivo para realizar un giro adecuado y posteriormente detenemos todo el movimiento por 500ms para lograr una mayor estabilidad.
### forward()
```C
static void forward(void){
	printf("Avanzando\n");
	bool stop = false;
	uart1_write('G');
	while(!stop) {
		if (infrarrojo_cntrl_distancia_read()>2)
			stop = true;
	}
	uart1_write('I');
	delay_ms(500);
}
```
Con esta función, buscamos que el carro avance hasta encontrar una línea negra en el suelo dadas las condiciones que se nos fueron dadas para el laberinto. Para ello escribimos la señal de avanzar por medio de la UART1 al Arduino y de allí en adelante revisamos constantemente la salida provista por el sensor infrarrojo que, cuando detecte una distancia mayor a 2 (parámetro que nos entregó buenos resultados para detectar líneas negras), envíe la señal de alto para detener el movimiento.

Todos los resultados se pueden ver en vídeo [aquí](https://drive.google.com/drive/folders/1TjqHZUeRFE6-v9n2TI3CKVCBduTdpxww?usp=sharing).

## Programa principal  Recorrido del laberinto 
Antes de proceder con la explicación de nuestro algoritmo para recorrer el laberinto, recordaremos algunas condiciones y aspectos generales planteados.

 - El laberinto consiste en una matriz 3x3 o 4x4 en el cual no se presentarán disyuntivas para que el carro decida que dirección tomar.
 - El carro siempre empezará en la coordenada 00 en dirección hacia arriba, y deberá cruzar la totalidad del laberinto de manera autónoma.
 - El carro siempre tendrá una vía libre a la cual avanzar, de lo contrario, se asumirá que es el final del recorrido.
 - El robot deberá presentar la información que encontró durante su recorrido mediante coordenadas y en direcciones absolutas, es decir que cada coordenada tendrá la misma notación para expresar los obstáculos encontrados sin importar la dirección en la que se encuentre el carro.
 - La información se presentará en un vector en la que la información en direcciones absolutas se presenta de la manera [Izquierda, Arriba, Derecha, Abajo].
 
A continuación se presentarán algunos de los aspectos más relevantes del programa, cabe recordar que este se encuentra adecuadamente comentado y en su totalidad en el archivo [main.c](https://github.com/unal-edigital2/w07_entrega-_final-grupo01/blob/main/ProyectoDigitalII/firmware/main.c) de la carpeta firmware.

### Funciones rotate_array()
```C
static void rotate_bool_array_right(bool array[], int size){
    bool first_element = array[0];
    for(int i = size-1; i >= 0; i--){
        array[(i+1)%size] = array[i];
    }
    array[1] = first_element;
}
```
Para este programa contamos con las funciones `rotate_array()`, que son funciones auxiliares para arreglos de booleanos y caracteres, las cuales "rotan" los arreglos dados en el argumento hacia la derecha o hacia la izquierda, es decir, esta función, en el caso de la rotación hacia la derecha, toma el primer elemento y lo pasa a ser segundo, toma el segundo y lo pasa a ser tercero, y continúa de esta manera hasta tomar el último elemento y convertirlo en el primero. Estas funciones nos sirven para transformar la información relativa al carro que hemos recolectado a información en dirección absoluta como la tenemos que transmitir.
### Algoritmo general
Este algoritmo se basa en un ciclo que solo culminará cuando nuestro carro no encuentre rutas disponibles por las cuales avanzar. En este ciclo la primera acción que realizará siempre es la de tomar la información del laberinto en su posición actual mediante:
```C
pwm_cntrl_orden_write(4);
walls[0] = read_us();
color[0] = read_camera();

pwm_cntrl_orden_write(6);
walls[2] = read_us();
color[2] = read_camera();
		
pwm_cntrl_orden_write(5);
walls[1] = read_us();
color[1] = read_camera();
```
De esta manera rotamos el servomotor que dicta la posición que tiene la cámara y el ultrasonido para adquirir la información que está a la izquierda, al frente y a la derecha relativa del carro.

En este punto, ya somos capaces de decidir que dirección tomar en el siguiente paso, entonces vamos a almacenar esta información en forma de un entero, que será el índice del primer elemento vacío del arreglo de las paredes. Notemos que tomando la información relativa, atrás siempre estará libre, por lo que si este es el único espacio que lo está, podremos terminar el recorrido.
```C
int free_index = 0;
for (int i = 0; i < 4; i++) {
	if (walls[i] == 0) {
		free_index = i;
		break;
	}
}
```
Ahora, sabiendo nuestra dirección absoluta, decidiremos cómo rotar los arreglos que obtuvimos en el paso anterior para de esta manera transmitirlos mediante la terminal en coordenadas absolutas.
```C
switch (absolute_direction) {
	case 0: // Apunta a la izquierda absoluta
		rotate_bool_array_left(walls, 4);
		rotate_char_array_left(color, 4);
		break;

	case 1: // Apunta al arriba absoluto
		break;

	case 2: // Apunta a la derecha absoluta
		rotate_bool_array_right(walls, 4);
		rotate_char_array_right(color, 4);
		break;

	default: // Apunta al abajo absoluto
		rotate_bool_array_right(walls, 4);
		rotate_bool_array_right(walls, 4);
		rotate_char_array_right(color, 4);
		rotate_char_array_right(color, 4);
		break;
}

		printf("\nCoordenada %d%d:    ", current_x, current_y);
		printf("Paredes: %d%d%d%d   ", walls[0], walls[1], walls[2], walls[3]);
		printf("Color: %c%c%c%c\n", color[0], color[1], color[2], color[3]);
```
Una vez hecho esto, nos resta continuar el camino, para ello usamos el índice obtenido anteriormente, el cual nos indica la dirección que está libre, con base en eso decidimos si tenemos que rotar el carro, avanzamos y actualizamos nuestra dirección absoluta tal que:
```C
switch (free_index) {
	case 0: // Izquierda relativa está libre
		rotate_car(false);
		absolute_direction--;
		if (absolute_direction < 0)
			absolute_direction = 3;
		forward();
		break;
			
	case 1: // Adelante relativo está libre
		forward();
		break;

	case 2: // Derecha relativa está libre
		rotate_car(true);
		absolute_direction = (absolute_direction + 1)%4;
		forward();
		break;

	default: // Solo atrás relativo está libre (Fin del laberinto)
		printf("Fin del recorrido\n");
		end_map = true;
		break;
}
```
Finalmente, según la dirección absoluta decidimos cómo actualizar las coordenadas del carro para ejecutar el siguiente ciclo (si lo hay):
```C
switch (absolute_direction) {
	case 0: // Apunta a la izquierda absoluta
		current_x--;
		break;

	case 1: // Apunta al arriba absoluto
		current_y++;
		break;

	case 2: // Apunta a la derecha absoluta
		current_x++;
		break;

	default: // Apunta al abajo absoluto
		current_y--;
		break;
}		
```
