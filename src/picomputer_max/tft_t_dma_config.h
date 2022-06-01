// From : https://github.com/Jean-MarcHarvengt/MCUME/blob/master/MCUME_pico/tft_t/tft_t_dma_config.h

// TFT
#define TFT_SPIREG      spi0
#define TFT_SPIDREQ     DREQ_SPI0_TX
#define TFT_SCLK        18
#define TFT_MOSI        19
#define TFT_MISO        255 // Not required, used for DC... 
#define TFT_DC          16
#define TFT_RST         255
#define TFT_CS          21
#define TFT_BACKLIGHT   20



#define ST7789         1
#define ST7789_POL     1
#define INVX           1
#define HAS_SND        1
#define PICOMPUTER     1


#define TFT_STATICFB   1
#define TFT_LINEARINT  1
#define LINEARINT_HACK 1


