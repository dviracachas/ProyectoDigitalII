#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>

#include "delay.h"
#include "display.h"
#include "camara.h"
#include "uart1.h"

static char *readstr(void)
{
	char c[2];
	static char s[64];
	static int ptr = 0;

	if(readchar_nonblock()) {
		c[0] = readchar();
		c[1] = 0;
		switch(c[0]) {
			case 0x7f:
			case 0x08:
				if(ptr > 0) {
					ptr--;
					putsnonl("\x08 \x08");
				}
				break;
			case 0x07:
				break;
			case '\r':
			case '\n':
				s[ptr] = 0x00;
				putsnonl("\n");
				ptr = 0;
				return s;
			default:
				if(ptr >= (sizeof(s) - 1))
					break;
				putsnonl(c);
				s[ptr] = c[0];
				ptr++;
				break;
		}
	}
	return NULL;
}

static char *get_token(char **str)
{
	char *c, *d;

	c = (char *)strchr(*str, ' ');
	if(c == NULL) {
		d = *str;
		*str = *str+strlen(*str);
		return d;
	}
	*c = 0;
	d = *str;
	*str = c+1;
	return d;
}

static void prompt(void)
{
	printf("RUNTIME>");
}

static void help(void)
{
	puts("Available commands:");
	puts("help                            - this command");
	puts("reboot                          - reboot CPU");
	puts("led                             - led test");
	puts("switch                          - switch test");
	puts("display                         - display test");
	puts("rgbled                          - rgb led test");
	puts("vga                             - vga test");
	puts("camara                          - camara test");
	puts("car                             - Car main");
	puts("us                              - Test US");
	puts("pwm                             - Test PWM");
	puts("ir                              - Test IR");
	puts("uart                            - Test UART1");
	puts("motor                           - Rotate car");
	puts("av                              - Test Avanzar");
}

static void reboot(void)
{
	ctrl_reset_write(1);
}

//******** Funciones para el programa completo del carro *********/
static void rotate_bool_array_right(bool array[], int size){
    bool first_element = array[0];
    for(int i = size-1; i >= 0; i--){
        array[(i+1)%size] = array[i];
    }
    array[1] = first_element;
}

static void rotate_bool_array_left(bool array[], int size){
    bool first_element = array[0];
    for(int i = 0; i < size; i++){
        array[i] = array[(i+1)%size];
    }
    array[size-1] = first_element;
}

static void rotate_char_array_right(char array[], int size){
    char first_element = array[0];
    for(int i = size-1; i >= 0; i--){
        array[(i+1)%size] = array[i];
    }
    array[1] = first_element;
}

static void rotate_char_array_left(char array[], int size){
    char first_element = array[0];
    for(int i = 0; i < size; i++){
        array[i] = array[(i+1)%size];
    }
    array[size-1] = first_element;
}

static void rotate_car(bool right){
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
}

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

/* static bool read_servo(void){
	unsigned short temp2 =0;
	while(!(buttons_in_read()&1)) {
		unsigned short temp = switchs_in_read();
		if (temp2 != temp){
			leds_out_write(temp);
			temp2 = temp;
		}
	}

	bool wall;
	if (temp2 >= 1){
		wall = true;
	} else {
		wall = false;
	}

	printf("%d		", wall);
	delay_ms(500);
	
	return wall;
} */

static bool read_us(void){
	ultrasonido_orden_write(1);
	bool done = false;
	while(!done){
		done = ultrasonido_done_read();
	}
	int d = ultrasonido_d_read();
	ultrasonido_orden_write(0);
	if(d<5)
		return 1;
	else
		return 0;
}

static char read_camera(void){
	char color = 'N';
	return color;
}

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

static void test_ir(void){
	while(!(buttons_in_read()&1)) {
		leds_out_write(infrarrojo_cntrl_distancia_read());
		delay_ms(50);
		}
}

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

static void uart1_test(void){
	printf("Test de la uart 1, debe poner en corto el pon RX Tx de la la UART1.\n");
	
	printf("se envia el caracter A por la uart 1  y al estar en loopback se recibe el caracter  y se retrasmite por la uart 0\n");
	printf("se interrumpe con el botton 1\n");

	while(!(buttons_in_read()&1)) {
		uart1_write('H');
		delay_ms(50);
		uart_write(uart1_read());
		}
}

static void motor_test(void){
	rotate_car(false);
	rotate_car(true);
}

//****************** Programa completo del carro *******************/
static void car_main(void){
	bool end_map = false;

	// Suponemos que siempre se inicia en la coordenada 00 mirando hacia arriba
	int current_x = 0;
	int current_y = 0;
	int absolute_direction = 1; // 0: Izquierda | 1: Arriba | 2: Derecha | 3:Abajo

	// Ciclo hasta terminar el laberinto
	while (!end_map) {
		// Iniciamos nuestros arreglos de información
		bool walls[4] = {0, 0, 0, 0};  // Paredes encontradas [ izq, arriba, der, abajo ]
		char color[4] = {"NNNN"};  // Obstáculos (colores) en dirección absoluta (al final)
		
		// Adquirimos nuestros datos de la coordenada en dirección relativa
		pwm_cntrl_orden_write(5);
		walls[0] = read_us();
		color[0] = read_camera();

		pwm_cntrl_orden_write(6);
		walls[2] = read_us();
		color[2] = read_camera();
		
		pwm_cntrl_orden_write(4);
		walls[1] = read_us();
		color[1] = read_camera();

		// Reservamos nuestra referencia de qué camino está libre para después
		int free_index = 0;
		for (int i = 0; i < 4; i++) {
			if (walls[i] == 0) {
				free_index = i;
				break;
			}
		}

		// Rotamos nuestros arreglos para obtener la información en direcciones
		// absolutas y las transmitimos
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

		// Con nuestra información relativa al carro, decidimos como avanzar
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

		//Actualiza nuestras coordeneadas para el siguiente ciclo.
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
	}
}


static void display_test(void)
{
	int i;
	signed char defill = 0;	//1->left, -1->right, 0->.
	
	char txtToDisplay[40] = {0, DISPLAY_0, DISPLAY_1,DISPLAY_2,DISPLAY_3,DISPLAY_4,DISPLAY_5,DISPLAY_6,DISPLAY_7,DISPLAY_8,DISPLAY_9,DISPLAY_A,DISPLAY_B,DISPLAY_C,DISPLAY_D,DISPLAY_E,DISPLAY_F,DISPLAY_G,DISPLAY_H,DISPLAY_I,DISPLAY_J,DISPLAY_K,DISPLAY_L,DISPLAY_M,DISPLAY_N,DISPLAY_O,DISPLAY_P,DISPLAY_Q,DISPLAY_R,DISPLAY_S,DISPLAY_T,DISPLAY_U,DISPLAY_V,DISPLAY_W,DISPLAY_X,DISPLAY_Y,DISPLAY_Z,DISPLAY_DP,DISPLAY_TR,DISPLAY_UR};
	
	printf("Test del los display de 7 segmentos... se interrumpe con el botton 1\n");

	while(!(buttons_in_read()&1)) {
		display(txtToDisplay);
		if(buttons_in_read()&(1<<1)) defill = ((defill<=-1) ? -1 : defill-1);
		if(buttons_in_read()&(1<<2)) defill = ((defill>=1) ? 1 : defill+1);
		if (defill > 0) {
			char tmp = txtToDisplay[0];
			for(i=0; i<sizeof(txtToDisplay)/sizeof(txtToDisplay[i]); i++) {
				txtToDisplay[i] = ((i==sizeof(txtToDisplay)/sizeof(txtToDisplay[i])-1) ? tmp : txtToDisplay[i+1]);
			}
		}
		else if(defill < 0) {
			char tmp = txtToDisplay[sizeof(txtToDisplay)/sizeof(txtToDisplay[0])-1];
			for(i=sizeof(txtToDisplay)/sizeof(txtToDisplay[i])-1; i>=0; i--) {
				txtToDisplay[i] = ((i==0) ? tmp : txtToDisplay[i-1]);
			}
		}
		delay_ms(500);
	}

}

static void led_test(void)
{
	unsigned int i;
	printf("Test del los leds... se interrumpe con el botton 1\n");
	while(!(buttons_in_read()&1)) {

	for(i=1; i<65536; i=i*2) {
		leds_out_write(i);
		delay_ms(50);
	}
	for(i=32768;i>1; i=i/2) {
		leds_out_write(i);
		delay_ms(50);
	}
	}
	
}


static void switch_test(void)
{
	unsigned short temp2 =0;
	printf("Test del los interruptores... se interrumpe con el botton 1\n");
	while(!(buttons_in_read()&1)) {
		unsigned short temp = switchs_in_read();
		if (temp2 != temp){
			printf("switch bus : %i\n", temp);
			leds_out_write(temp);
			temp2 = temp;
		}
	}
}

static void rgbled_test(void)
{
	unsigned int T = 128;
	
	ledRGB_1_r_period_write(T);
	ledRGB_1_g_period_write(T);
	ledRGB_1_b_period_write(T);

	ledRGB_1_r_enable_write(1);
	ledRGB_1_g_enable_write(1);
	ledRGB_1_b_enable_write(1);

	
	ledRGB_2_r_period_write(T);
	ledRGB_2_g_period_write(T);
	ledRGB_2_b_period_write(T);
	
	
	ledRGB_2_r_enable_write(1);
	ledRGB_2_g_enable_write(1);
	ledRGB_2_b_enable_write(1);

	for (unsigned int j=0; j<100; j++){
		ledRGB_1_g_width_write(j);
		for (unsigned int i=0; i<100; i++){
			ledRGB_1_r_width_write(100-i);
			ledRGB_1_b_width_write(i);	
			delay_ms(20);
		}	
	}
	


}


static void vga_test(void)
{
	int x,y;
	
	for(y=0; y<480; y++) {
		for(x=0; x<640; x++) {
			vga_cntrl_mem_we_write(0);
			vga_cntrl_mem_adr_write(y*640+x);
			if(x<640/3)	
				vga_cntrl_mem_data_w_write(((int)(x/10)%2^(int)(y/10)%2)*15);
			else if(x<2*640/3) 
				vga_cntrl_mem_data_w_write((((int)(x/10)%2^(int)(y/10)%2)*15)<<4);
			else 
				vga_cntrl_mem_data_w_write((((int)(x/10)%2^(int)(y/10)%2)*15)<<8);
			vga_cntrl_mem_we_write(1);
		}
	}

}

static void console_service(void)
{
	char *str;
	char *token;

	str = readstr();
	if(str == NULL) return;
	token = get_token(&str);
	if(strcmp(token, "help") == 0)
		help();
	else if(strcmp(token, "reboot") == 0)
		reboot();
	else if(strcmp(token, "led") == 0)
		led_test();
	else if(strcmp(token, "switch") == 0)
		switch_test();
	else if(strcmp(token, "display") == 0)
		display_test();
	else if(strcmp(token, "rgbled") == 0)
		rgbled_test();
	else if(strcmp(token, "vga") == 0)
		vga_test();
	else if(strcmp(token, "av") == 0)
		forward();
	else if(strcmp(token, "car") == 0)
		car_main();
	else if(strcmp(token, "pwm") == 0)
		test_pwm();
	else if(strcmp(token, "ir") == 0)
		test_ir();
	else if(strcmp(token, "us") == 0)
		test_us();
	else if(strcmp(token, "uart") == 0)
		uart1_test();
	else if(strcmp(token, "motor") == 0)
		motor_test();
	prompt();
}

int main(void)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();
	camara_init();
	/*Se adiciona un serial mas */
	uart1_init();


	puts("\nSoC - RiscV project UNAL 2020-2-- CPU testing software  uart add "__DATE__" "__TIME__"\n");
	help();
	prompt();

	while(1) {
	//	uart1_test();
		console_service();
	}

	return 0;
}
