// From: https://github.com/Jean-MarcHarvengt/MCUME/blob/master/MCUME_pico/tft_t/tft_t_dma.cpp

#include "tft_t_dma.h"

#if defined(ILI9341) || defined(ST7789)

#include "tft_font8x8.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include <string.h>


#define digitalWrite(pin, val) gpio_put(pin, val)

#define SPICLOCK 60000000
#ifdef ST7789
#ifdef ST7789_POL
#define SPI_MODE SPI_CPOL_0
#else
#define SPI_MODE SPI_CPOL_1
#endif
#endif
#ifdef ILI9341
#define SPI_MODE SPI_CPOL_0
#endif


static void SPItransfer(uint8_t val)
{
  uint8_t dat8=val;
  spi_write_blocking(TFT_SPIREG, &dat8, 1);
}

static void SPItransfer16(uint16_t val)
{
  uint8_t dat8[2];
  dat8[0] = val>>8;
  dat8[1] = val&0xff;
  spi_write_blocking(TFT_SPIREG, dat8, 2);
}

static const uint8_t init_commands[] = {
#ifdef ILI9341
  4, 0xEF, 0x03, 0x80, 0x02,
  4, 0xCF, 0x00, 0XC1, 0X30,
  5, 0xED, 0x64, 0x03, 0X12, 0X81,
  4, 0xE8, 0x85, 0x00, 0x78,
  6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
  2, 0xF7, 0x20,
  3, 0xEA, 0x00, 0x00,
  2, ILI9341_PWCTR1, 0x23, // Power control
  2, ILI9341_PWCTR2, 0x10, // Power control
  3, ILI9341_VMCTR1, 0x3e, 0x28, // VCM control
  2, ILI9341_VMCTR2, 0x86, // VCM control2
  2, ILI9341_MADCTL, 0x48, // Memory Access Control
  2, ILI9341_PIXFMT, 0x55,
  3, ILI9341_FRMCTR1, 0x00, 0x18,
  4, ILI9341_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control
  2, 0xF2, 0x00, // Gamma Function Disable
  2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
  16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
  0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
  16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
  0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
//  3, 0xb1, 0x00, 0x1f, // FrameRate Control 61Hz
  3, 0xb1, 0x00, 0x10, // FrameRate Control 119Hz
  2, ILI9341_MADCTL, ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR,
  0
#endif
#ifdef ST7789
#define DELAY 0x80
    9,                       // 9 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      150,                     //    150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x55,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  4: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_CASET  , 4      ,  //  5: Column addr set, 4 args, no delay:
      0x00,
      0x00,                   //     XSTART = 0
      0x00,
      240,                    //      XEND = 240
    ST7735_RASET  , 4      ,  // 6: Row addr set, 4 args, no delay:
      0x00,
      0x00,                   //     YSTART = 0
      320>>8,
      320 & 0xFF,             //      YEND = 320
    ST7735_INVON ,   DELAY,   // 7: hack
      10,
    ST7735_NORON  ,   DELAY,  // 8: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 9: Main screen turn on, no args, w/delay
    255
#endif
};


TFT_T_DMA::TFT_T_DMA()
{
  _cs   = TFT_CS;
  _dc   = TFT_DC;
  _rst  = TFT_RST;
  _mosi = TFT_MOSI;
  _sclk = TFT_SCLK;
  //_miso = TFT_MISO;
  _bkl = TFT_BACKLIGHT;
  gpio_init(_dc);
  gpio_set_dir(_dc, GPIO_OUT);
  gpio_init(_cs);
  gpio_set_dir(_cs, GPIO_OUT);
  digitalWrite(_cs, 1);
  digitalWrite(_dc, 1);
  if (_bkl != 0xff) {
    gpio_init(_bkl);
    gpio_set_dir(_bkl, GPIO_OUT);
    digitalWrite(_bkl, 1);
  }
}


void TFT_T_DMA::setArea(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2) {
  int dx=0;
  int dy=0;
#ifdef ST7789
  if (TFT_REALWIDTH == TFT_REALHEIGHT)
  {
#ifdef ROTATE_SCREEN
    if (!flipped) {
      dy += 80;
    }
#else
    if (flipped) {
      dx += 80;
    }
#endif
  }
#endif

  digitalWrite(_dc, 0);
  SPItransfer(TFT_CASET);
  digitalWrite(_dc, 1);
  SPItransfer16(x1+dx);
  digitalWrite(_dc, 1);
  SPItransfer16(x2+dx);
  digitalWrite(_dc, 0);
  SPItransfer(TFT_PASET);
  digitalWrite(_dc, 1);
  SPItransfer16(y1+dy);
  digitalWrite(_dc, 1);
  SPItransfer16(y2+dy);

  digitalWrite(_dc, 0);
  SPItransfer(TFT_RAMWR);
  digitalWrite(_dc, 1);

  return;
}


void TFT_T_DMA::begin(void) {
  spi_init(TFT_SPIREG, SPICLOCK);
  spi_set_format(TFT_SPIREG, 8, SPI_MODE, SPI_CPHA_0, SPI_MSB_FIRST);
  gpio_set_function(_sclk , GPIO_FUNC_SPI);
  gpio_set_function(_mosi , GPIO_FUNC_SPI);
  //gpio_set_function(_miso, GPIO_FUNC_SPI);

  // Initialize display
  if (_rst != 0xff) {
    gpio_init(_rst);
    gpio_set_dir(_rst, GPIO_OUT);
    digitalWrite(_rst, 1);
    sleep_ms(100);
    digitalWrite(_rst, 0);
    sleep_ms(100);
    digitalWrite(_rst, 1);
    sleep_ms(200);
  }

  const uint8_t *addr = init_commands;
  digitalWrite(_cs, 0);
#ifdef ILI9341
  while (1) {
    uint8_t count = *addr++;
    if (count-- == 0) break;

    digitalWrite(_dc, 0);
    SPItransfer(*addr++);

    while (count-- > 0) {
      digitalWrite(_dc, 1);
      SPItransfer(*addr++);
    }
  }

  digitalWrite(_dc, 0);
  SPItransfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
  digitalWrite(_cs, 1);

  digitalWrite(_dc, 1);
  digitalWrite(_cs, 1);

  digitalWrite(_dc, 0);
  digitalWrite(_cs, 0);
  SPItransfer(ILI9341_DISPON);
  digitalWrite(_dc, 1);
  digitalWrite(_cs, 1);
#endif
#ifdef ST7789
  uint8_t  numCommands, numArgs;
  uint16_t ms;
  numCommands = *addr++;    // Number of commands to follow
  while(numCommands--) {        // For each command...
    digitalWrite(_dc, 0);
    SPItransfer(*addr++);
    numArgs  = *addr++; //   Number of args to follow
    ms       = numArgs & DELAY;   //   If hibit set, delay follows args
    numArgs &= ~DELAY;      //   Mask out delay bit
    while(numArgs > 1) {      //   For each argument...
      digitalWrite(_dc, 1);
      SPItransfer(*addr++);
      numArgs--;
    }

    if (numArgs)  {
      digitalWrite(_dc, 1);
      SPItransfer(*addr++);
    }
    if(ms) {
      ms = *addr++; // Read post-command delay time (ms)
      if(ms == 255) ms = 500;   // If 255, delay for 500 ms
      digitalWrite(_cs, 1);
      //SPI.endTransaction();
      sleep_ms(ms);
      //SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
      digitalWrite(_cs, 0);
    }
  }
  digitalWrite(_cs, 1);
#endif
  setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);

#ifdef FLIP_SCREEN
  flipscreen(true);
#else
  flipscreen(false);
#endif
#ifdef ST7789
  if (TFT_REALWIDTH != TFT_REALHEIGHT)
  {
    //flipscreen(true);
  }
#endif
};

void TFT_T_DMA::flipscreen(bool flip)
{
  digitalWrite(_dc, 0);
  digitalWrite(_cs, 0);
  SPItransfer(TFT_MADCTL);
  digitalWrite(_dc, 1);
  if (flip) {
    flipped=true;
#ifdef ILI9341
    SPItransfer(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
#endif
#ifdef ST7789
#ifdef ROTATE_SCREEN
    SPItransfer(ST77XX_MADCTL_RGB);
#else
    SPItransfer(ST77XX_MADCTL_MY | ST77XX_MADCTL_MV |ST77XX_MADCTL_RGB);
#endif
#endif
  }
  else {
    flipped=false;
#ifdef ILI9341
    SPItransfer(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
#endif
#ifdef ST7789
#ifdef ROTATE_SCREEN
    SPItransfer(ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB);
#else
    SPItransfer(ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB);
#endif
#endif
  }
  digitalWrite(_cs, 1);
}

bool TFT_T_DMA::isflipped(void)
{
  return(flipped);
}

void  TFT_T_DMA::waitSync() {
}

/***********************************************************************************************
    No DMA functions
 ***********************************************************************************************/
void TFT_T_DMA::fillScreenNoDma(uint16_t color) {
  digitalWrite(_cs, 0);
  setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);
  //digitalWrite(_dc, 0);
  //SPItransfer(TFT_RAMWR);
  int i,j;
  for (j=0; j<TFT_REALHEIGHT; j++)
  {
    for (i=0; i<TFT_REALWIDTH; i++) {
      //digitalWrite(_dc, 1);
      SPItransfer16(color);
    }
  }
#ifdef ILI9341
  digitalWrite(_dc, 0);
  SPItransfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
#endif
  digitalWrite(_cs, 1);
  setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));
}


void TFT_T_DMA::drawRectNoDma(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  //SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);
  setArea(x,y,x+w-1,y+h-1);
  int i;
  for (i=0; i<(w*h); i++)
  {
    //digitalWrite(_dc, 1);
    SPItransfer16(color);
  }
#ifdef ILI9341
  digitalWrite(_dc, 0);
  SPItransfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
#endif
  digitalWrite(_cs, 1);
  setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));
  //SPI.endTransaction();
}


void TFT_T_DMA::writeScreenNoDma(const uint16_t *pcolors) {
  //SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);
  setArea(0, 0, TFT_WIDTH-1, TFT_HEIGHT-1);
  int i,j;
  for (j=0; j<240; j++)
  {
    for (i=0; i<TFT_WIDTH; i++) {
      SPItransfer16(*pcolors++);
    }
  }
#ifdef ILI9341
  digitalWrite(_dc, 0);
  SPItransfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
#endif
  digitalWrite(_cs, 1);
  setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));
  //SPI.endTransaction();
}

void TFT_T_DMA::drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap) {
    drawSpriteNoDma(x,y,bitmap, 0,0,0,0);
}

void TFT_T_DMA::drawSpriteNoDma(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t arx, uint16_t ary, uint16_t arw, uint16_t arh)
{
  int bmp_offx = 0;
  int bmp_offy = 0;
  uint16_t *bmp_ptr;

  int w =*bitmap++;
  int h = *bitmap++;

  if ( (arw == 0) || (arh == 0) ) {
    // no crop window
    arx = x;
    ary = y;
    arw = w;
    arh = h;
  }
  else {
    if ( (x>(arx+arw)) || ((x+w)<arx) || (y>(ary+arh)) || ((y+h)<ary)   ) {
      return;
    }

    // crop area
    if ( (x > arx) && (x<(arx+arw)) ) {
      arw = arw - (x-arx);
      arx = arx + (x-arx);
    } else {
      bmp_offx = arx;
    }
    if ( ((x+w) > arx) && ((x+w)<(arx+arw)) ) {
      arw -= (arx+arw-x-w);
    }
    if ( (y > ary) && (y<(ary+arh)) ) {
      arh = arh - (y-ary);
      ary = ary + (y-ary);
    } else {
      bmp_offy = ary;
    }
    if ( ((y+h) > ary) && ((y+h)<(ary+arh)) ) {
      arh -= (ary+arh-y-h);
    }
  }


  //SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);
  setArea(arx, ary, arx+arw-1, ary+arh-1);

  bitmap = bitmap + bmp_offy*w + bmp_offx;
  for (int row=0;row<arh; row++)
  {
    bmp_ptr = (uint16_t*)bitmap;
    for (int col=0;col<arw; col++)
    {
        uint16_t color = *bmp_ptr++;
        SPItransfer16(color);
    }
    bitmap +=  w;
  }
#ifdef ILI9341
  digitalWrite(_dc, 0);
  SPItransfer(ILI9341_SLPOUT);
  digitalWrite(_dc, 1);
#endif
  setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);
  digitalWrite(_cs, 1);
  //SPI.endTransaction();
}

void TFT_T_DMA::drawTextNoDma(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize) {
  uint16_t c;
  while ((c = *text++)) {
    const unsigned char * charpt=&font8x8[c][0];

    //SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
    digitalWrite(_cs, 0);
    setArea(x,y,x+7,y+(doublesize?15:7));
    for (int i=0;i<8;i++)
    {
      unsigned char bits;
      if (doublesize) {
        bits = *charpt;
        //digitalWrite(_dc, 1);
        if (bits&0x01) SPItransfer16(fgcolor);
        else SPItransfer16(bgcolor);
        bits = bits >> 1;
        if (bits&0x01) SPItransfer16(fgcolor);
        else SPItransfer16(bgcolor);
        bits = bits >> 1;
        if (bits&0x01) SPItransfer16(fgcolor);
        else SPItransfer16(bgcolor);
        bits = bits >> 1;
        if (bits&0x01) SPItransfer16(fgcolor);
        else SPItransfer16(bgcolor);
        bits = bits >> 1;
        if (bits&0x01) SPItransfer16(fgcolor);
        else SPItransfer16(bgcolor);
        bits = bits >> 1;
        if (bits&0x01) SPItransfer16(fgcolor);
        else SPItransfer16(bgcolor);
        bits = bits >> 1;
        if (bits&0x01) SPItransfer16(fgcolor);
        else SPItransfer16(bgcolor);
        bits = bits >> 1;
        if (bits&0x01) SPItransfer16(fgcolor);
        else SPItransfer16(bgcolor);
      }
      bits = *charpt++;
      //digitalWrite(_dc, 1);
      if (bits&0x01) SPItransfer16(fgcolor);
      else SPItransfer16(bgcolor);
      bits = bits >> 1;
      if (bits&0x01) SPItransfer16(fgcolor);
      else SPItransfer16(bgcolor);
      bits = bits >> 1;
      if (bits&0x01) SPItransfer16(fgcolor);
      else SPItransfer16(bgcolor);
      bits = bits >> 1;
      if (bits&0x01) SPItransfer16(fgcolor);
      else SPItransfer16(bgcolor);
      bits = bits >> 1;
      if (bits&0x01) SPItransfer16(fgcolor);
      else SPItransfer16(bgcolor);
      bits = bits >> 1;
      if (bits&0x01) SPItransfer16(fgcolor);
      else SPItransfer16(bgcolor);
      bits = bits >> 1;
      if (bits&0x01) SPItransfer16(fgcolor);
      else SPItransfer16(bgcolor);
      bits = bits >> 1;
      if (bits&0x01) SPItransfer16(fgcolor);
      else SPItransfer16(bgcolor);
    }
    x +=8;
#ifdef ILI9341
    digitalWrite(_dc, 0);
    SPItransfer(ILI9341_SLPOUT);
    digitalWrite(_dc, 1);
#endif
    digitalWrite(_cs, 1);
    //SPI.endTransaction();
  }

  //SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
  digitalWrite(_cs, 0);
  setArea(0, 0, (TFT_REALWIDTH-1), (TFT_REALHEIGHT-1));
  digitalWrite(_cs, 1);
  //SPI.endTransaction();
}


/***********************************************************************************************
    DMA functions
 ***********************************************************************************************/
#ifndef USE_VGA

#ifdef TFT_STATICFB
static uint16_t fb0[LINES_PER_BLOCK*TFT_WIDTH]; //__attribute__ ((aligned(2048)));
static uint16_t fb1[LINES_PER_BLOCK*TFT_WIDTH]; //__attribute__ ((aligned(2048)));
static uint16_t fb2[LINES_PER_BLOCK*TFT_WIDTH]; //__attribute__ ((aligned(2048)));
static uint16_t fb3[(TFT_HEIGHT-3*LINES_PER_BLOCK)*TFT_WIDTH];// __attribute__ ((aligned(2048)));
static uint16_t * blocks[NR_OF_BLOCK]={fb0,fb1,fb2,fb3};
static uint16_t blocklens[NR_OF_BLOCK];
#else
static uint16_t * blocks[NR_OF_BLOCK];
static uint16_t blocklens[NR_OF_BLOCK];
#endif


static dma_channel_config dmaconfig;
const uint dma_tx = dma_claim_unused_channel(true);
static volatile uint8_t rstop = 0;
static volatile bool cancelled = false;
static volatile uint8_t curTransfer = 0;
static uint8_t nbTransfer = 0;

static void dma_isr() {
  irq_clear(DMA_IRQ_0);
  dma_hw->ints0 = 1u << dma_tx;
  curTransfer++;
  if (curTransfer >= nbTransfer) {
    curTransfer = 0;
  }
  if (cancelled) {
    rstop = 1;
  }
  else
  {
    dma_channel_transfer_from_buffer_now ( dma_tx, blocks[curTransfer], blocklens[curTransfer]);
  }
}

static void setDmaStruct() {
  uint32_t remaining = TFT_HEIGHT*TFT_WIDTH*2;
  int i=0;
  nbTransfer = 0;
  uint16_t col=RGBVAL16(0x00,0x00,0x00);;
  while (remaining > 0) {
    uint16_t * fb = blocks[i];
    int32_t len = (remaining >= (LINES_PER_BLOCK*TFT_WIDTH*2)?LINES_PER_BLOCK*TFT_WIDTH*2:remaining);
#ifdef TFT_DEBUG
    printf("%d\n",(unsigned long)blocks[i]);
    printf("%d\n",remaining);
#endif
    switch (i) {
      case 0:
        if (fb == 0) fb = (uint16_t*)((int)malloc(len+64)&0xffffffe0);
        //fb=&fb0[0];
#ifdef TFT_DEBUG
        col = RGBVAL16(0x00,0xff,0x00);
#endif
        break;
      case 1:
        if (fb == 0) fb = (uint16_t*)((int)malloc(len+64)&0xffffffe0);
        //fb=&fb1[0];
#ifdef TFT_DEBUG
        col = RGBVAL16(0x00,0xff,0xff);
#endif
        break;
      case 2:
        if (fb == 0) fb = (uint16_t*)((int)malloc(len+64)&0xffffffe0);
        //fb=&fb2[0];
#ifdef TFT_DEBUG
        col = RGBVAL16(0x00,0x00,0xff);
#endif
        break;
      case 3:
        if (fb == 0) fb = (uint16_t*)((int)malloc(len+64)&0xffffffe0);
        //fb=&fb3[0];
#ifdef TFT_DEBUG
        col = RGBVAL16(0xff,0x00,0xff);
#endif
        break;
    }
    blocks[i] = fb;
    blocklens[i] = len/2;
    if (blocks[i] == 0) {
      printf("LI9341 allocaltion failed for block %d\n",i);
      sleep_ms(10000);
    }
    nbTransfer++;
    remaining -= len;
    i++;
  }


  // Setup the control channel
  dmaconfig = dma_channel_get_default_config(dma_tx);
  channel_config_set_transfer_data_size(&dmaconfig, DMA_SIZE_16);
  channel_config_set_dreq(&dmaconfig, TFT_SPIDREQ);
  //channel_config_set_read_increment(&dmaconfig, true); // read incrementing
  //channel_config_set_write_increment(&dmaconfig, false); // no write incrementing

  dma_channel_configure(
      dma_tx,
      &dmaconfig,
      &spi_get_hw(TFT_SPIREG)->dr, // write address
      blocks[0],
      blocklens[0],
      false
  );

  irq_set_exclusive_handler(DMA_IRQ_0, dma_isr);
  dma_channel_set_irq0_enabled(dma_tx, true);
  irq_set_enabled(DMA_IRQ_0, true);
  dma_hw->ints0 = 1u << dma_tx;
}


void TFT_T_DMA::startDMA(void) {
  curTransfer = 0;
  rstop = 0;
  digitalWrite(_cs, 1);
  setDmaStruct();
  fillScreen(RGBVAL16(0x00,0x00,0x00));

  digitalWrite(_cs, 0);
  setArea((TFT_REALWIDTH-TFT_WIDTH)/2, (TFT_REALHEIGHT-TFT_HEIGHT)/2, (TFT_REALWIDTH-TFT_WIDTH)/2 + TFT_WIDTH-1, (TFT_REALHEIGHT-TFT_HEIGHT)/2+TFT_HEIGHT-1);
  // we switch to 16bit mode!!
  spi_set_format(TFT_SPIREG, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
  dma_start_channel_mask(1u << dma_tx);
}


void TFT_T_DMA::stopDMA(void) {
  rstop = 0;
  wait();
  sleep_ms(100);
  cancelled = false;
  //dmatx.detachInterrupt();
  fillScreen(RGBVAL16(0x00,0x00,0x00));
  digitalWrite(_cs, 1);
  // we switch to 8bit mode!!
  //spi_set_format(TFT_SPIREG, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
//#ifdef ST7789
  begin();
//#endif
//#ifdef ILI9341
//  SPI.begin();
//  digitalWrite(_cs, 0);
//  SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE));
//  SPI.endTransaction();
//  digitalWrite(_cs, 1);
//  digitalWrite(_dc, 1);
//#endif
  setArea(0, 0, TFT_REALWIDTH-1, TFT_REALHEIGHT-1);
}

void TFT_T_DMA::wait(void) {
  rstop = 1;
  unsigned long m = time_us_32()*1000;
  cancelled = true;
  while (!rstop)  {
    if ((time_us_32()*1000 - m) > 100) break;
    sleep_ms(100);
    asm volatile("wfi");
  };
  rstop = 0;
}


int TFT_T_DMA::get_frame_buffer_size(int *width, int *height){
  if (width != nullptr) *width = TFT_REALWIDTH;
  if (height != nullptr) *height = TFT_REALHEIGHT;
  return TFT_REALWIDTH;
}


uint16_t * TFT_T_DMA::getLineBuffer(int j)
{
  uint16_t * block=blocks[j>>6];
  return(&block[(j&0x3F)*TFT_REALWIDTH]);
}

void TFT_T_DMA::writeScreen(int width, int height, int stride, uint8_t *buf, uint16_t *palette16) {
  uint8_t *buffer=buf;
  uint8_t *src;

  int i,j,y=0;
  if (width*2 <= TFT_REALWIDTH) {
    for (j=0; j<height; j++)
    {
      uint16_t * block=blocks[y>>6];
      uint16_t * dst=&block[(y&0x3F)*TFT_WIDTH];
      src=buffer;
      for (i=0; i<width; i++)
      {
        uint16_t val = palette16[*src++];
        *dst++ = val;
        *dst++ = val;
      }
      y++;
      if (height*2 <= TFT_HEIGHT) {
        block=blocks[y>>6];
        dst=&block[(y&0x3F)*TFT_WIDTH];
        src=buffer;
        for (i=0; i<width; i++)
        {
          uint16_t val = palette16[*src++];
          *dst++ = val;
          *dst++ = val;
        }
        y++;
      }
      buffer += stride;
    }
  }
  else if (width <= TFT_REALWIDTH) {
    //dst += (TFT_WIDTH-width)/2;
    for (j=0; j<height; j++)
    {
      uint16_t * block=blocks[y>>6];
      uint16_t * dst=&block[(y&0x3F)*TFT_WIDTH+(TFT_WIDTH-width)/2];
      src=buffer;
      for (i=0; i<width; i++)
      {
        uint16_t val = palette16[*src++];
        *dst++ = val;
      }
      y++;
      if (height*2 <= TFT_HEIGHT) {
        block=blocks[y>>6];
        dst=&block[(y&0x3F)*TFT_WIDTH+(TFT_WIDTH-width)/2];
        src=buffer;
        for (i=0; i<width; i++)
        {
          uint16_t val = palette16[*src++];
          *dst++ = val;
        }
        y++;
      }
      buffer += stride;
    }
  }
}

void TFT_T_DMA::writeLine(int width, int height, int y, uint8_t *buf, uint16_t *palette16) {
  uint16_t * block=blocks[y>>6];
  uint16_t * dst=&block[(y&0x3F)*TFT_WIDTH];
  if (width > TFT_WIDTH) {
#ifdef TFT_LINEARINT
    int delta = (width/(width-TFT_WIDTH))-1;
    int pos = delta;
    for (int i=0; i<TFT_WIDTH; i++)
    {
      uint16_t val = palette16[*buf++];
      pos--;
      if (pos == 0) {
#ifdef LINEARINT_HACK
        val  = ((uint32_t)palette16[*buf++] + val)/2;
#else
        uint16_t val2 = *buf++;
        val = RGBVAL16((R16(val)+R16(val2))/2,(G16(val)+G16(val2))/2,(B16(val)+B16(val2))/2);
#endif
        pos = delta;
      }
      *dst++=val;
    }
#else
    int step = ((width << 8)/TFT_WIDTH);
    int pos = 0;
    for (int i=0; i<TFT_WIDTH; i++)
    {
      *dst++=palette16[buf[pos >> 8]];
      pos +=step;
    }
#endif
  }
  else if ((width*2) == TFT_WIDTH) {
    for (int i=0; i<width; i++)
    {
      *dst++=palette16[*buf];
      *dst++=palette16[*buf++];
    }
  }
  else {
    if (width <= TFT_WIDTH) {
      dst += (TFT_WIDTH-width)/2;
    }
    for (int i=0; i<width; i++)
    {
      *dst++=palette16[*buf++];
    }
  }
}

void TFT_T_DMA::writeLine(int width, int height, int y, uint16_t *buf) {
  uint16_t * block=blocks[y>>6];
  uint16_t * dst=&block[(y&0x3F)*TFT_WIDTH];
  if (width > TFT_WIDTH) {
#ifdef TFT_LINEARINT
    int delta = (width/(width-TFT_WIDTH))-1;
    int pos = delta;
    for (int i=0; i<TFT_WIDTH; i++)
    {
      uint16_t val = *buf++;
      pos--;
      if (pos == 0) {
#ifdef LINEARINT_HACK
        val  = ((uint32_t)*buf++ + val)/2;
#else
        uint16_t val2 = *buf++;
        val = RGBVAL16((R16(val)+R16(val2))/2,(G16(val)+G16(val2))/2,(B16(val)+B16(val2))/2);
#endif
        pos = delta;
      }
      *dst++=val;
    }
#else
    int step = ((width << 8)/TFT_WIDTH);
    int pos = 0;
    for (int i=0; i<TFT_WIDTH; i++)
    {
      *dst++=buf[pos >> 8];
      pos +=step;
    }
#endif
  }
  else if ((width*2) == TFT_WIDTH) {
    for (int i=0; i<width; i++)
    {
      *dst++=*buf;
      *dst++=*buf++;
    }
  }
  else {
    if (width <= TFT_WIDTH) {
      dst += (TFT_WIDTH-width)/2;
    }
    for (int i=0; i<width; i++)
    {
      *dst++=*buf++;
    }
  }
}

void TFT_T_DMA::fillScreen(uint16_t color) {
  int i,j;
  for (j=0; j<TFT_HEIGHT; j++)
  {
    uint16_t * block=blocks[j>>6];
    uint16_t * dst=&block[(j&0x3F)*TFT_WIDTH];
    for (i=0; i<TFT_WIDTH; i++)
    {
      *dst++ = color;
    }
  }
}

void TFT_T_DMA::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  int i,j,l=y;
  color=color;
  for (j=0; j<h; j++)
  {
    uint16_t * block=blocks[l>>6];
    uint16_t * dst=&block[(l&0x3F)*TFT_WIDTH+x];
    for (i=0; i<w; i++)
    {
      *dst++ = color;
    }
    l++;
  }
}

void TFT_T_DMA::drawText(int16_t x, int16_t y, const char * text, uint16_t fgcolor, uint16_t bgcolor, bool doublesize) {
  uint16_t c;
  uint16_t * block;
  uint16_t * dst;
  fgcolor = fgcolor;
  bgcolor = bgcolor;

  while ((c = *text++)) {
    const unsigned char * charpt=&font8x8[c][0];

    int l=y;
    for (int i=0;i<8;i++)
    {
      unsigned char bits;
      if (doublesize) {
        block=blocks[l>>6];
        dst=&block[(l&0x3F)*TFT_WIDTH+x];
        bits = *charpt;
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        bits = bits >> 1;
        if (bits&0x01) *dst++=fgcolor;
        else *dst++=bgcolor;
        l++;
      }
      block=blocks[l>>6];
      dst=&block[(l&0x3F)*TFT_WIDTH+x];
      bits = *charpt++;
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      bits = bits >> 1;
      if (bits&0x01) *dst++=fgcolor;
      else *dst++=bgcolor;
      l++;
    }
    x +=8;
  }
}

void TFT_T_DMA::drawSprite(int16_t x, int16_t y, const uint16_t *bitmap) {
    drawSprite(x,y,bitmap, 0,0,0,0);
}

void TFT_T_DMA::drawSprite(int16_t x, int16_t y, const uint16_t *bitmap, uint16_t arx, uint16_t ary, uint16_t arw, uint16_t arh)
{
  int bmp_offx = 0;
  int bmp_offy = 0;
  uint16_t *bmp_ptr;

  int w =*bitmap++;
  int h = *bitmap++;


  if ( (arw == 0) || (arh == 0) ) {
    // no crop window
    arx = x;
    ary = y;
    arw = w;
    arh = h;
  }
  else {
    if ( (x>(arx+arw)) || ((x+w)<arx) || (y>(ary+arh)) || ((y+h)<ary)   ) {
      return;
    }

    // crop area
    if ( (x > arx) && (x<(arx+arw)) ) {
      arw = arw - (x-arx);
      arx = arx + (x-arx);
    } else {
      bmp_offx = arx;
    }
    if ( ((x+w) > arx) && ((x+w)<(arx+arw)) ) {
      arw -= (arx+arw-x-w);
    }
    if ( (y > ary) && (y<(ary+arh)) ) {
      arh = arh - (y-ary);
      ary = ary + (y-ary);
    } else {
      bmp_offy = ary;
    }
    if ( ((y+h) > ary) && ((y+h)<(ary+arh)) ) {
      arh -= (ary+arh-y-h);
    }
  }


  int l=ary;
  bitmap = bitmap + bmp_offy*w + bmp_offx;
  for (int row=0;row<arh; row++)
  {
    uint16_t * block=blocks[l>>6];
    uint16_t * dst=&block[(l&0x3F)*TFT_WIDTH+arx];
    bmp_ptr = (uint16_t*)bitmap;
    for (int col=0;col<arw; col++)
    {
        *dst++ = *bmp_ptr++;
    }
    bitmap +=  w;
    l++;
  }
}

#endif

#endif
