/* Copyright 2025 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * @file
 *
 * Board definition for a Texas Instuments
 * <a href="https://www.ti.com/tool/LP-EM-CC1354P10">LP-EM-CC1354P10 LaunchPad development kit</a>
 */
#ifndef BOARD_LP_EM_CC1354P10_1_BOARD_H_
#define BOARD_LP_EM_CC1354P10_1_BOARD_H_

/* XDS110 VCOM Serial Port (UART0 on XDS110 back-channel) */
/* Connects to PC via USB for debugging and testing */
#define BOARD_UART_RX_PIN              12 /* DIO12 UART0 RX (PC → Device) */
#define BOARD_UART_TX_PIN              13 /* DIO13 UART0 TX (Device → PC) */

#define BOARD_UART_WAKEUP_NOT_IN_USE   /* UART_WAKEUP not needed on LL-only devices, thus not implemented for CC1354P10 */

/* Additional UART instances (customize as needed) */
#define BOARD_UART1_RX_PIN             45 /* DIO45 XDS_GPIO1 UART1 RX */
#define BOARD_UART1_TX_PIN             46 /* DIO46 XDS_GPIO2 UART1 TX */
#define BOARD_UART2_RX_PIN             IOID_UNUSED /* DIOxx UART2 RX */
#define BOARD_UART2_TX_PIN             IOID_UNUSED /* DIOxx UART2 TX */
#define BOARD_UART3_RX_PIN             IOID_UNUSED /* DIOxx UART3 RX */
#define BOARD_UART3_TX_PIN             IOID_UNUSED /* DIOxx UART3 TX */

// SPI for External Flash Memory IOC_PORT_MCU_SPI0_*
#define BOARD_SPI_EXTFLASH_MOSI_PIN    36 /* DIO36 Flash MOSI */
#define BOARD_SPI_EXTFLASH_MISO_PIN    37 /* DIO37 Flash MISO */
#define BOARD_SPI_EXTFLASH_CS_PIN      38 /* DIO38 Flash CS */
#define BOARD_SPI_EXTFLASH_SCLK_PIN    39 /* DIO39 Flash SCLK */
#define BOARD_SPI_EXTFLASH_DEV         SPI0_BASE
#define BOARD_SPI_EXTFLASH_PERIPH      PRCM_PERIPH_SPI0

// RF switch
#define BOARD_RF_SWITCH_2_4_GHZ        34 /* 2.4 GHz RF signal (RX and TX) */
#define BOARD_RF_SWITCH_SUB_GHZ        35 /* Sub-GHz RF signal (RX and TX) */
#define BOARD_RF_SWITCH_20_DBM_PA      3  /* 2.4 GHz and Sub-GHz high-power RF signal (TX only) */

// List of GPIO pins
#define BOARD_GPIO_PIN_LIST            { 6,  /* DIO06 Red LED */\
                                         7,  /* DIO07 Green LED */\
                                         15, /* DIO15 BTN1 */\
                                         14, /* DIO14 BTN2 */\
                                       }

// User friendly name for GPIOs (IDs mapped to the BOARD_GPIO_PIN_LIST table)
#define BOARD_GPIO_ID_LED0             0  // mapped to pin DIO6 Red LED
#define BOARD_GPIO_ID_LED1             1  // mapped to pin DIO7 Green LED
#define BOARD_GPIO_ID_BUTTON0          2  // mapped to pin DIO15 BTN1
#define BOARD_GPIO_ID_BUTTON1          3  // mapped to pin DIO14 BTN2

// List of LED IDs
#define BOARD_LED_ID_LIST              {BOARD_GPIO_ID_LED0, BOARD_GPIO_ID_LED1}

// Active high polarity for LEDs
#define BOARD_LED_ACTIVE_LOW           false

// List of button IDs
#define BOARD_BUTTON_ID_LIST           {BOARD_GPIO_ID_BUTTON0, BOARD_GPIO_ID_BUTTON1}

// Active low polarity for buttons
#define BOARD_BUTTON_ACTIVE_LOW        true

// Board doesn't have external pull-up for buttons
#define BOARD_BUTTON_INTERNAL_PULL     true



#endif /* BOARD_LP_EM_CC1354P10_1_BOARD_H_ */
