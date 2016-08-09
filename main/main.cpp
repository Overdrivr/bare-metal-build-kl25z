#include "stdint.h"
#include "../platform/MKL25Z4/include/MKL25Z4.h"

static void delay()
{
  volatile int i;
  for(i = 0 ; i < 8192 ; i++)
  {

  }
}

int main(void)
{
  int counter = 0;
  /* Turn on clock to PortB module */
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

  /* Set the PTB18 pin multiplexer to GPIO mode */
  PORTB_PCR18 = PORT_PCR_MUX(1);

  /* Set the initial output state to high */
  GPIOB_PSOR |= 1 << 18;

  /* Set the pins direction to output */
  GPIOB_PDDR |= 1 << 18;

  for(;;)
  {
    counter++;
    delay();
    if((counter % 64) == 0)
    {
      GPIOB_PTOR = 1 << 18;
    }
  }
  return 0;
}
