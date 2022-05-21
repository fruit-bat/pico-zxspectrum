#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "vga.h"
#include "pzx_prepare_vga332_scanline.h"
#include "pzx_keyscan.h"

#define VREG_VSEL VREG_VOLTAGE_1_10
#include "PicoCharRendererVga.h"
#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "PicoPen.h"
#include "PicoTextField.h"
#include "PicoWinHidKeyboard.h"

struct semaphore dvi_start_sem;
static const sVmode* vmode = NULL;



static uint8_t screen[6144];
static uint8_t attr[768];

void __not_in_flash_func(core1_main)() {
  sem_acquire_blocking(&dvi_start_sem);
  printf("Core 1 running...\n");

  // TODO fetch the resolution from the mode ?
  VgaInit(vmode,640,480);

  while (1) {

    VgaLineBuf *linebuf = get_vga_line();
    uint32_t* buf = (uint32_t*)&(linebuf->line);
    uint32_t y = linebuf->row;
    if (true) {
      pcw_prepare_vga332_scanline_80(
        buf,
        y,
        linebuf->frame);
    }
    else {
      pzx_prepare_vga332_scanline(
        buf, 
        y, 
        linebuf->frame,
        screen,
        attr,
        1);
    }
      
    pzx_keyscan_row();
  }
  __builtin_unreachable();
}

int main(){
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(10);
  vmode = Video(DEV_VGA, RES_HVGA);
  sleep_ms(100);

  //Initialise I/O
  stdio_init_all(); 

  for(unsigned int i = 0; i < sizeof(screen) ; ++i) {
    screen[i] = (0xff & time_us_32());
  }
  for(unsigned int i = 0; i < sizeof(attr); ++i) {
    attr[i] = (0xff & i);
  }
  
  pcw_init_renderer();
  
  pzx_keyscan_init();
  
  sleep_ms(10);
  
  printf("Core 0 VCO %d\n", Vmode.vco);

  printf("Core 1 start\n");
  sem_init(&dvi_start_sem, 0, 1);
  multicore_launch_core1(core1_main);
    
  PicoWin picoRootWin(10, 10, 60, 10);
  PicoDisplay picoDisplay(pcw_screen(), &picoRootWin);
  PicoWinHidKeyboard picoWinHidKeyboard(&picoDisplay);  
  
  PicoTextField textField(24, 5, 16, 18);
  picoRootWin.addChild(&textField, true);
  picoRootWin.onPaint([=](PicoPen *pen){
    pen->printAtF(24, 4, false,"Hello World!");
  });
  
  printf("Core 0 VCO %d\n", Vmode.vco);

  sem_release(&dvi_start_sem);


  //Main Loop 
  while(1){

   // printf("Hello ");
    sleep_ms(1); 
  
    hid_keyboard_report_t const *curr;
    hid_keyboard_report_t const *prev;
    pzx_keyscan_get_hid_reports(&curr, &prev);
    
    for(int ri = 0; ri < 6; ++ri) {
      uint32_t r = pzx_keyscan_get_row(ri);
      printf("keyrow %d %2.2x\n", ri, r);
    }
    for(int ri = 0; ri < 6; ++ri) {
      uint32_t cc = curr->keycode[ri];
      uint32_t cp = prev->keycode[ri];
      printf("hid key %d %2.2x %2.2x\n", ri, cc, cp);
    }
    printf("\n");       
    
    picoWinHidKeyboard.processHidReport(curr, prev);
    
    picoDisplay.refresh();
      
   // for(unsigned int i = 0; i < sizeof(screen); ++i) {
   //   screen[i] = (0xff & time_us_32());
   // }
   // printf("world!\n");
    sleep_ms(100); 
  }
}
