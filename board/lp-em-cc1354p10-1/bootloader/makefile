SRCS += bootloader/external_flash_cfi.c
SRCS += board/lp-em-cc1354p10-1/bootloader/external_flash_mx25r1635f.c
SRCS += $(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/spi.c

# TI DriverLib vendor files have unused parameters - suppress warnings
$(BUILDPREFIX_APP)bootloader/$(VENDOR_PREFIX)/cc13x4_cc26x4/driverlib/ioc.o: CFLAGS += -Wno-unused-parameter
