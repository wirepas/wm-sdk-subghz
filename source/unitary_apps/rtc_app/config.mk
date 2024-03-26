# Boards compatible with this app 
TARGET_BOARDS := efr32_template radientum_wp_v1_0 silabs_brd4158a silabs_brd4210a 
#
# Network default settings configuration
#

# If this section is removed, node has to be configured in
# a different way
default_network_address ?= 0x239492
default_network_channel ?= 9

# !HIGHLY RECOMMENDED! : To enable security keys please un-comment the lines below and fill with a
#                        randomly generated authentication & encryption keys (exactly 16 bytes)
#default_network_cipher_key ?= 0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??
#default_network_authen_key ?= 0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??,0x??

#
# App specific configuration
#

# Define a specific application area_id
app_specific_area_id=0x80597A

# App version
app_major=$(sdk_major)
app_minor=$(sdk_minor)
app_maintenance=$(sdk_maintenance)
app_development=1