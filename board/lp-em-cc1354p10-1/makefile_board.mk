INCLUDES += -Imcu/ti/cc13/hal/radio

SRCS += board/$(target_board)/board_custom_init.c \
        board/$(target_board)/fem_rf_switch.c

# TI DriverLib dependencies for RF switch
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/ioc.c

# TI DriverLib vendor files have unused parameters - suppress warnings
$(BUILDPREFIX_APP)$(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/ioc.o: CFLAGS += -Wno-unused-parameter
