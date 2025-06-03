/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Wirepas SDK", "index.html", [
    [ "Single-MCU Operation Overview", "index.html", "index" ],
    [ "SDK Environment setup", "dd/d6a/sdk_environment.html", [
      [ "Installation of SDK Environment", "dd/d6a/sdk_environment.html#installation_of_sdk_environment", null ],
      [ "Flashing devices", "dd/d6a/sdk_environment.html#flashing_guideline", null ],
      [ "Resources on EFR32", "dd/d6a/sdk_environment.html#efr32_resources", [
        [ "Flash Memory available for application on EFR32", "dd/d6a/sdk_environment.html#flash_memory_efr32", null ],
        [ "RAM Memory available for application on EFR32", "dd/d6a/sdk_environment.html#ram_memory_efr32", null ],
        [ "Peripherals accessible by stack only", "dd/d6a/sdk_environment.html#peripherals_accessible_by_stack_only_efr32", null ],
        [ "Peripherals shared between the stack and the application", "dd/d6a/sdk_environment.html#peripherals_shared_between_the_stack_and_the_application_efr32", null ],
        [ "Peripherals available for the application", "dd/d6a/sdk_environment.html#peripherals_available_for_the_application_efr32", null ]
      ] ],
      [ "Related Material", "dd/d6a/sdk_environment.html#autotoc_md0", null ]
    ] ],
    [ "How to Develop Application with SDK", "de/d7a/how_to_develop.html", [
      [ "Pre-requirements", "de/d7a/how_to_develop.html#licensing", [
        [ "Debugging", "de/d7a/how_to_develop.html#debugging", null ],
        [ "Installing firmware image", "de/d7a/how_to_develop.html#firmware_linking", null ]
      ] ],
      [ "Create new application", "de/d7a/how_to_develop.html#development_of_a_new_application", [
        [ "Copy of an application example", "de/d7a/how_to_develop.html#copy_of_an_application_example", null ],
        [ "Change default network", "de/d7a/how_to_develop.html#change_default_network_address_and_channel", null ],
        [ "Change of app_area_id", "de/d7a/how_to_develop.html#change_of_app_area_id", null ],
        [ "Configuration of a node", "de/d7a/how_to_develop.html#configuration_of_a_node", null ]
      ] ],
      [ "Build application", "de/d7a/how_to_develop.html#build_application", null ],
      [ "Test application", "de/d7a/how_to_develop.html#test_application", null ],
      [ "Flashing device", "de/d7a/how_to_develop.html#flashing_device", null ],
      [ "Using OTAP", "de/d7a/how_to_develop.html#using_otap", null ],
      [ "Define custom board definition", "de/d7a/how_to_develop.html#define_custom_board", null ],
      [ "Application startup", "de/d7a/how_to_develop.html#app_init", null ],
      [ "Adding new source files", "de/d7a/how_to_develop.html#adding_new_source_files_to_the_application", null ],
      [ "Recommendations", "de/d7a/how_to_develop.html#recommendations", [
        [ "Optimization of throughput", "de/d7a/how_to_develop.html#optimization_of_network_throughput", null ],
        [ "Free resources", "de/d7a/how_to_develop.html#free_resources", null ],
        [ "Power consumption", "de/d7a/how_to_develop.html#power_consumption", null ],
        [ "How to store data in persistent memory", "de/d7a/how_to_develop.html#persistent_memory", null ],
        [ "Using storage library", "de/d7a/how_to_develop.html#storage_library", null ],
        [ "Using platform specific storage", "de/d7a/how_to_develop.html#platform_specific_storage", null ],
        [ "Using a dedicated area in flash", "de/d7a/how_to_develop.html#dedicated_area", null ]
      ] ],
      [ "Related Material", "de/d7a/how_to_develop.html#autotoc_md1", null ]
    ] ],
    [ "Single-MCU API Operation Principle", "d7/d28/application_operation.html", [
      [ "Build process", "d7/d28/application_operation.html#build", null ],
      [ "Memory Partitioning", "d7/d28/application_operation.html#memory_partitioning", null ],
      [ "Application Detection", "d7/d28/application_operation.html#application_detection", null ],
      [ "Cooperative MCU Access", "d7/d28/application_operation.html#cooperative_mcu_access", [
        [ "Periodic Application Callback Function", "d7/d28/application_operation.html#periodic_application", null ],
        [ "Asynchronous Application Callback Functions", "d7/d28/application_operation.html#asynchronous_application_callback_functions", null ],
        [ "Application Interrupt", "d7/d28/application_operation.html#application_interrupt", null ],
        [ "Deferred interrupt", "d7/d28/application_operation.html#deferred_interrupt", null ],
        [ "Fast interrupt", "d7/d28/application_operation.html#fast_interrupt", null ],
        [ "Which kind of interrupt for which purpose", "d7/d28/application_operation.html#which_kind_of_interrupt_for_which_purpose", null ],
        [ "Execution time limits", "d7/d28/application_operation.html#execution_time_limits", null ]
      ] ]
    ] ],
    [ "API", "dd/d9c/programming_interface.html", [
      [ "Stack Libraries", "dd/d9c/programming_interface.html#stack_api", [
        [ "Application and Library Versioning", "dd/d9c/programming_interface.html#application_and_library_versioning", null ]
      ] ],
      [ "SDK Libraries", "dd/d9c/programming_interface.html#libraries_api", null ],
      [ "Board definitions", "dd/d9c/programming_interface.html#board_api", [
        [ "DCDC converter configuration", "dd/d9c/programming_interface.html#DCDC_converter", null ]
      ] ],
      [ "Bootloader configuration", "dd/d9c/programming_interface.html#bootloader_api", null ],
      [ "Low level hardware services (HAL)", "dd/d9c/programming_interface.html#mcu_api", [
        [ "mcu/common Common MCU files", "dd/d9c/programming_interface.html#mcu_common", null ],
        [ "start.c", "dd/d9c/programming_interface.html#mcu_common_start_c", null ],
        [ "mcu/hal_api Low level hardware API", "dd/d9c/programming_interface.html#mcu_hal_api", null ],
        [ "mcu/<hardware>", "dd/d9c/programming_interface.html#mcu_specific_files", null ],
        [ "Linker file", "dd/d9c/programming_interface.html#linker_file", null ],
        [ "Flash Memory", "dd/d9c/programming_interface.html#flash_memory", null ],
        [ "RAM memory", "dd/d9c/programming_interface.html#ram_memory", null ]
      ] ],
      [ "Utility and helper services", "dd/d9c/programming_interface.html#util_api", null ]
    ] ],
    [ "Application Examples and Tools", "d5/d7b/application_examples.html", [
      [ "Application examples", "d5/d7b/application_examples.html#source_folder", [
        [ "app.c", "d5/d7b/application_examples.html#source_app_c", null ],
        [ "config.mk", "d5/d7b/application_examples.html#source_config_mk", null ],
        [ "app_specific_area_id", "d5/d7b/application_examples.html#app_specific_area_id", null ],
        [ "Application version", "d5/d7b/application_examples.html#app_version", null ],
        [ "TARGET_BOARDS", "d5/d7b/application_examples.html#app_target_boards", null ],
        [ "makefile", "d5/d7b/application_examples.html#source_makefile", null ],
        [ "APP_PRINTING", "d5/d7b/application_examples.html#source_makefile_app_printing", null ],
        [ "APP_SCHEDULER", "d5/d7b/application_examples.html#source_makefile_app_scheduler", null ],
        [ "CFLAGS", "d5/d7b/application_examples.html#source_makefile_cflags", null ],
        [ "HAL_GPIO", "d5/d7b/application_examples.html#source_makefile_hal_gpio", null ],
        [ "HAL_BUTTON", "d5/d7b/application_examples.html#source_makefile_hal_button", null ],
        [ "HAL_HW_DELAY", "d5/d7b/application_examples.html#source_makefile_hal_hw_delay", null ],
        [ "HAL_I2C", "d5/d7b/application_examples.html#source_makefile_hal_i2c", null ],
        [ "HAL_LED", "d5/d7b/application_examples.html#source_makefile_hal_led", null ],
        [ "HAL_PERSISTENT_MEMORY", "d5/d7b/application_examples.html#source_makefile_hal_persistent_memory", null ],
        [ "HAL_SPI", "d5/d7b/application_examples.html#source_makefile_hal_spi", null ],
        [ "HAL_UART", "d5/d7b/application_examples.html#source_makefile_hal_uart", null ],
        [ "INCLUDES", "d5/d7b/application_examples.html#source_makefile_includes", null ],
        [ "LDFLAGS", "d5/d7b/application_examples.html#source_makefile_ldflags", null ],
        [ "LIBS", "d5/d7b/application_examples.html#source_makefile_libs", null ],
        [ "PROVISIONING", "d5/d7b/application_examples.html#source_makefile_provisioning", null ],
        [ "PROVISIONING_PROXY", "d5/d7b/application_examples.html#source_makefile_provisioning_proxy", null ],
        [ "SHARED_DATA", "d5/d7b/application_examples.html#source_makefile_shared_data", null ],
        [ "SRCS", "d5/d7b/application_examples.html#source_makefile_srcs", null ],
        [ "SW_AES", "d5/d7b/application_examples.html#source_makefile_sw_aes", null ]
      ] ],
      [ "Tools", "d5/d7b/application_examples.html#tools_folder", [
        [ "genscratchpad.py", "d5/d7b/application_examples.html#genscratchpad_py", null ],
        [ "INI_FILE", "d5/d7b/application_examples.html#config_mk_ini_file", null ]
      ] ]
    ] ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", "functions_vars" ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", "globals_eval" ],
        [ "Macros", "globals_defs.html", "globals_defs" ]
      ] ]
    ] ],
    [ "Example applications", "examples.html", "examples" ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"d2/d9e/cbor_8h.html#a7d1bf6025bf79637dd52806ed0503331",
"d4/d27/wms__state_8h.html#a317f50bd4ec70608fcab95aa001455fa",
"d5/d63/dualmcu__lib_8h.html#aa267ba0cb0c8fa9778930ad837301e36a42014a979fdce9f769b4c82ae000621d",
"d6/d50/wms__data_8h.html#ab53b6510f8ecb4a1621c905c15855164abc148375192b9cd13d5a3733def48cf6",
"d7/dc3/tlv_8h.html#a849e9473d3954c57c1a4f976aedd019da0a29e48ca5fe6df9cd0463a0c99f4fd8",
"d8/d21/msap__frames_8h.html#ac9e42bc3f331a9213430e7c1b1d1bc98a7606ebce7e30a9715879e112be846a22",
"da/d1b/function__codes_8h.html#a5d932513e8009c2280045fe336ff4138a19878661f1de522fc8100573b862a973",
"db/d41/timing_8h.html#a884497f3269a8ae82bcd0a4814997702",
"dd/d2c/bl__interface_8h.html#a9a1d6078d92f4d46b59e7f80ef16b898a1a28228a2c91c43fd8ae40e7c992a15f",
"de/d7a/how_to_develop.html#flashing_device",
"df/d99/wms__otap_8h.html#a471f1fe9b71534a54c285db244a4210a",
"index.html#bootloader_extension"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';