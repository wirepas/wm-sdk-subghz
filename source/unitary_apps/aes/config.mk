# Boards compatible with this app 
TARGET_BOARDS := efr32_template lp-em-cc1354p10-1 silabs_brd4158a silabs_brd4210a 
# Define a specific application area_id
app_specific_area_id=0x8cad1b

# App version
app_major=1
app_minor=0
app_maintenance=0
app_development=0

# Enable multi-instance UART driver (set to yes for multi-UART support)
uart_multi_instance?=no

usart_multi_tx_buffer_size?=256
usart_multi_rx_buffer_size?=256
usart_multi_max_instances?=1
