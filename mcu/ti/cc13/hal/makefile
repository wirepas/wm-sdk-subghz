CC13_HAL_PREFIX := $(MCU_PREFIX)/hal

# Add include path for cc13 specific HAL header files
INCLUDES += -I$(CC13_HAL_PREFIX)

SRCS += $(CC13_HAL_PREFIX)/power.c
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/pwr_ctrl.c

# Build cc1354 specific HAL drivers
ifeq ($(HAL_UART), yes)

# UART configuration from config.mk (lowercase) -> CFLAGS (uppercase)
# User must define usart_multi_max_instances in config.mk (1-4 for CC1354P10)
ifneq ($(usart_multi_max_instances),)
CFLAGS += -DUSART_MULTI_MAX_INSTANCES=$(usart_multi_max_instances)
endif
ifneq ($(usart_multi_tx_buffer_size),)
CFLAGS += -DUSART_MULTI_TX_BUFFER_SIZE=$(usart_multi_tx_buffer_size)
endif
ifneq ($(usart_multi_rx_buffer_size),)
CFLAGS += -DUSART_MULTI_RX_BUFFER_SIZE=$(usart_multi_rx_buffer_size)
endif

# DMA driver sources (shared by UART and other peripherals)
SRCS += $(CC13_HAL_PREFIX)/dma/dma.c

# UART driver sources (core multi-instance driver)
SRCS += $(CC13_HAL_PREFIX)/uart/uart.c
SRCS += $(CC13_HAL_PREFIX)/uart/uart_dma.c
SRCS += $(CC13_HAL_PREFIX)/uart/uart_ints.c
SRCS += $(CC13_HAL_PREFIX)/uart/uart_hw_config.c

# Backward compatibility wrapper for legacy usart.h API
# Included by default, set usart_exclude_legacy_api = yes to exclude
ifneq ($(usart_exclude_legacy_api), yes)
SRCS += $(MCU_COMMON_SRCS_PATH)hal/uart/usart_compat.c
endif

# TI DriverLib dependencies for UART
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/uart.c
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/udma.c
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/ioc.c
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/prcm.c

# TI DriverLib vendor files have unused parameters - suppress warnings
$(BUILDPREFIX_APP)$(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/uart.o: CFLAGS += -Wno-unused-parameter
$(BUILDPREFIX_APP)$(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/udma.o: CFLAGS += -Wno-unused-parameter
$(BUILDPREFIX_APP)$(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/ioc.o: CFLAGS += -Wno-unused-parameter
$(BUILDPREFIX_APP)$(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/prcm.o: CFLAGS += -Wno-unused-parameter

# uart_internal.h wraps vendor headers with pragma directives to suppress warnings

# Add include paths
INCLUDES += -I$(CC13_HAL_PREFIX)/dma
INCLUDES += -I$(CC13_HAL_PREFIX)/uart
endif

ifeq ($(HAL_GPIO), yes)
SRCS += $(CC13_HAL_PREFIX)/gpio.c

# TI DriverLib dependencies for GPIO
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/ioc.c
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/prcm.c

# TI DriverLib vendor files have unused parameters - suppress warnings
$(BUILDPREFIX_APP)$(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/ioc.o: CFLAGS += -Wno-unused-parameter
$(BUILDPREFIX_APP)$(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/prcm.o: CFLAGS += -Wno-unused-parameter
endif

ifeq ($(HAL_SPI), yes)
SRCS += $(CC13_HAL_PREFIX)/spi.c

# TI DriverLib dependencies for SPI
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/ioc.c
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/prcm.c
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/spi.c

# TI DriverLib vendor files have unused parameters - suppress warnings
$(BUILDPREFIX_APP)$(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/ioc.o: CFLAGS += -Wno-unused-parameter
endif
