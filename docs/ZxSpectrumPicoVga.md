### ZxSpectrumPicoVga

<img src="pico-demo-base-9_1500x1500.png" width="300"/>



It supports the following:
* USB keyboard
* USB joysticks
* 50/60Hz  video
* I2S audio
* SPI SD card
* Ringo's low resolution mode

#### Firmware

| Diplay | Firmware |
| - | - |
| 720x576x50Hz | [ZxSpectrumPicoVga_720x576x50Hz.uf2](/uf2/ZxSpectrumPicoVga_720x576x50Hz.uf2) |
| 640x480x60Hz | [ZxSpectrumPicoVga_640x480x60Hz.uf2](/uf2/ZxSpectrumPicoVga_640x480x60Hz.uf2) |

#### Build
Building firmware for the *Pimoroni Pico VGA Demo Base* needs a different cmake command:

```sh
cd build
cmake -DPICO_COPY_TO_RAM=0 -DPICO_BOARD=vgaboard ..
make -j4 ZxSpectrumPicoVga_720x576x50Hz ZxSpectrumPicoVga_640x480x60Hz
```

#### Parts
[Pimoroni](https://shop.pimoroni.com/products/pimoroni-pico-vga-demo-base)