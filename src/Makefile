TEST_PATH=$(PWD)
PLAT=$(PLATFORM)

INCLUDE_PATH += ./
INCLUDE_PATH += ./include
INCLUDE_PATH += ../third_lib

ifeq ($(PLAT),x86)
SOURCE_FILES += ./extern/softPwm_mock.c
SOURCE_FILES += ./extern/wiringPi_mock.c
endif

ifeq ($(RPI),1)
OFLAGS += -DPRI
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
SOURCE_FILES += ../third_lib/ssd1306/lib/GUI/*.c
SOURCE_FILES += ../third_lib/ssd1306/lib/OLED/*.c
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

CC = gcc


OFLAGS += -g -Wall 

LIBS    +=  -lpthread -lm

# LIBPATH += $(foreach path,$(DEPS_LIB_PATH),-L$(path))
# LIBPATH += -L${SDK_ROOT_PATH}

IFLAGS += $(foreach path,$(INCLUDE_PATH),-I$(path))

CFLAGS = ${IFLAGS} ${OFLAGS}

.PHONY:clean

all:clean kvmd-main
# $(SOURCE_FILES)
kvmd-main:demo.c $(SOURCE_FILES)
	$(CC) -DSSD1306=$(SSD1306) ${CFLAGS} $^ -o $@ ${LIBS}
clean:
	@rm -f kvmd-main