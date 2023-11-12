#define SSD1619 0
#define UCVARIANT029 1
#define UCVARIANT043 2
#define UC8159 3
#define UC8179 4

#if EPD_DRIVER == SSD1619
#include "epd_driver/ssd1619.cpp"
#endif

#if EPD_DRIVER == UCVARIANT029
#include "epd_driver/uc_variant_029.cpp"
#endif

#if EPD_DRIVER == UCVARIANT043
#include "epd_driver/uc_variant_043.cpp"
#endif

#if EPD_DRIVER == UC8159
#include "epd_driver/uc8159.cpp"
#endif

#if EPD_DRIVER == UC8179
#include "epd_driver/uc8179.cpp"
#endif
