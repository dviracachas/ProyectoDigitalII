#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camara.h"

#include <uart.h>
#include <irq.h>
#include <generated/csr.h>

void camara_isr(void)
{

	
}
void camara_init(void)
{
/*	camara_cntrl_ev_pending_write(camara_cntrl_ev_pending_read());
	camara_cntrl_ev_enable_write(1);
	irq_setmask(irq_getmask() | (1 << CAMARA_CNTRL_INTERRUPT));*/
}



