# Boards compatible with this app 
TARGET_BOARDS := efr32_template radientum_wp_v1_0 silabs_brd4158a silabs_brd4210a 
#
# Network default settings configuration
#

# If this section is removed, node has to be configured in
# a different way
default_network_address ?= 0xBADBAD
default_network_channel ?= 1
# !HIGHLY RECOMMENDED! : To enable security keys please un-comment the lines below and fill with a
#                        randomly generated authentication & encryption keys (exactly 16 bytes)
default_network_cipher_key ?= 0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33
default_network_authen_key ?= 0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33

#
# App specific configuration
#

# Define a specific application area_id
app_specific_area_id=0x38530C

# App version
app_major=1
app_minor=0
app_maintenance=0
app_development=0

led_notification_enabled=yes
