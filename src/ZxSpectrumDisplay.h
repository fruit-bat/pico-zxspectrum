#pragma once

#ifndef DISPLAY_WIDTH_PIXELS
#define DISPLAY_WIDTH_PIXELS 640
#endif
#ifndef DISPLAY_HEIGHT_PIXELS
#define DISPLAY_HEIGHT_PIXELS 480
#endif
// The number of blank lines to add, taking into account line doubling
#define DISPLAY_BLANK_LINES ((DISPLAY_HEIGHT_PIXELS/2) - 240)
