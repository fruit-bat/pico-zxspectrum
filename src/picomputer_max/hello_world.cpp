#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pzx_keyscan.h"
#include "tft_t_dma.h"

#define VREG_VSEL VREG_VOLTAGE_1_10
#define LED_PIN 25


int main(){
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(100);
	set_sys_clock_khz(240000, true);

  //Initialise I/O
  stdio_init_all(); 
  
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  // Initialise the keyboard scan
  pzx_keyscan_init();
  
  sleep_ms(10);
  TFT_T_DMA tft;
  
  tft.begin();
  
  while(1){
    

    printf("RP2040 Pico Pi at %3.1fMhz  ", (float)clock_get_hz(clk_sys) / 1000000.0);
    sleep_ms(500);
    pzx_keyscan_row();
    tft.fillScreenNoDma(time_us_32() & 0xffff);
  }
}
