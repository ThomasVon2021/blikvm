TEST_PATH=$(PWD)
PLAT=$(PLATFORM)

INCLUDE_PATH += ./
INCLUDE_PATH += ./extern
INCLUDE_PATH += ./include
INCLUDE_PATH += ../third_lib

ifeq ($(PLAT),x86)
SOURCE_FILES += ./extern/softPwm_mock.c
SOURCE_FILES += ./extern/wiringPi_mock.c
endif

ifeq ($(RPI),1)
CFLAGS += -DRPI
endif

ifeq ($(H616),1)
CFLAGS += -DH616
CFLAGS += -DVER4
ifeq ($(TEST_HARDWARE),1)
INCLUDE_PATH += ../test/hardware/
SOURCE_FILES += ../test/hardware/blikvm_test_v4.c
CFLAGS += -DTEST_HARDWARE
endif
endif

ifeq ($(SSD1306),1)
INCLUDE_PATH += ../third_lib/ssd1306/examples/
INCLUDE_PATH += ../third_lib/ssd1306/lib/Config/
INCLUDE_PATH += ../third_lib/ssd1306/lib/Fonts/
INCLUDE_PATH += ../third_lib/ssd1306/lib/GUI/
INCLUDE_PATH += ../third_lib/ssd1306/lib/OLED/

SOURCE_FILES += ../third_lib/ssd1306/examples/OLED_0in91_test.c
SOURCE_FILES += ../third_lib/ssd1306/examples/OLED_0in96_test.c
SOURCE_FILES += ../third_lib/ssd1306/lib/Config/*.c
SOURCE_FILES += ../third_lib/ssd1306/lib/Fonts/font12.c
SOURCE_FILES += ../third_lib/ssd1306/lib/Fonts/font8.c
SOURCE_FILES += ../third_lib/ssd1306/lib/GUI/*.c
SOURCE_FILES += ../third_lib/ssd1306/lib/OLED/*.c
CFLAGS += -DSSD1306=$(SSD1306)
endif

ifeq ($(ST7789),1)
INCLUDE_PATH += ../third_lib/SPI_LCD/
SOURCE_FILES += ../third_lib/SPI_LCD/spi_lcd.c
SOURCE_FILES += ../third_lib/SPI_LCD/oled_run.c
SOURCE_FILES += ../third_lib/SPI_LCD/fonts.c
SOURCE_FILES += ../third_lib/SPI_LCD/Fonts/*.c
SOURCE_FILES += ../third_lib/SPI_LCD/GUI/*.c
CFLAGS += -DST7789=$(ST7789)
endif


SOURCE_FILES += ./common/blikvm_log/blikvm_log.c
SOURCE_FILES += ./common/blikvm_uart/blikvm_uart.c
SOURCE_FILES += ./common/blikvm_util/blikvm_util.c
SOURCE_FILES += ./common/blikvm_util/cJSON.c
SOURCE_FILES += ./blikvm_server.c
SOURCE_FILES += ./kvmd/blikvm_fan/blikvm_fan.c
SOURCE_FILES += ./kvmd/blikvm_atx/blikvm_atx.c
SOURCE_FILES += ./kvmd/blikvm_switch/blikvm_switch.c
SOURCE_FILES += ../third_lib/GPIO/armbianio.c
SOURCE_FILES += ../third_lib/GPIO/softPwm.c
SOURCE_FILES += ./kvmd/blikvm_oled/blikvm_oled.c
SOURCE_FILES += ./kvmd/blikvm_gpio/blikvm_gpio.c
SOURCE_FILES += ./kvmd/blikvm_rtc/blikvm_rtc.c
SOURCE_FILES += ./config/blikvm_config.c

CC = gcc


OFLAGS += -g -Wall 

LIBS    +=  -lpthread -lm

# LIBPATH += $(foreach path,$(DEPS_LIB_PATH),-L$(path))
# LIBPATH += -L${SDK_ROOT_PATH}

IFLAGS += $(foreach path,$(INCLUDE_PATH),-I$(path))

CFLAGS += ${IFLAGS} ${OFLAGS}

.PHONY:clean

all:clean kvmd-main
# $(SOURCE_FILES)
kvmd-main:demo.c $(SOURCE_FILES)
	$(CC) -D_GNU_SOURCE ${CFLAGS} $^ -o $@ ${LIBS}
clean:
	@rm -f kvmd-main