
#ifndef _SDK_ENVIRONMENT_H_
#define _SDK_ENVIRONMENT_H_

/**
@page sdk_environment SDK Environment setup

As a prerequisite for this guide, you must be able to successfully build
one of the provided application example from the SDK.

This page contains following sections:
- @subpage installation_of_sdk_environment
- @subpage flashing_guideline
- @subpage efr32_resources
- @subpage ti_resources_subg


@section installation_of_sdk_environment Installation of SDK Environment

To ease the management of SDK environement, Wirepas maintains a docker image with
all the required dependencies installed.

For more information to use it, please read guidance from <a href="https://developer.wirepas.com/support/solutions/articles/77000435375">
Wirepas Helpdesk.</a>

It is also possible to install requirement in your native environement but it is not described here.
Requirement are listed in Github SDK main page under Requirement section.

@section flashing_guideline Flashing devices

Checkout flashing guidance from <a href="https://developer.wirepas.com/support/solutions/articles/77000465762">
Wirepas Helpdesk.</a>



@section efr32_resources Resources on EFR32

Following chip variants (at SubG only) are supported:
-   EFR32MG13P733F512GM48
-   EFR32FG23B020F512IM48-C
-   EFR32FG23B020F512IM40-C
-   EFR32FG23A020F512GM48-C
-   EFR32FG23A020F512GM40-C
-   EFR32ZG23B020F512IM48-C
-   EFR32ZG23B020F512IM40-C
-   EFR32ZG23A020F512GM48-C
-   EFR32ZG23A020F512GM40-C

This page contains following sections:
- @subpage flash_memory_efr32
- @subpage ram_memory_efr32
- @subpage peripherals_accessible_by_stack_only_efr32
- @subpage peripherals_shared_between_the_stack_and_the_application_efr32
- @subpage peripherals_available_for_the_application_efr32

@subsection flash_memory_efr32 Flash Memory available for application on EFR32

As stated in [description of memory partitioning](@ref memory_partitioning), the
available flash memory for application is limited by size of the memory area
that is used commonly for application and also scratchpad image. If application
size is too large, there is possibility that large scratchpad image will
override application image. The default maximum size of the application has been
set so that it is always safe to use scratchpad image that will contain both
firmware and application.

The _recommended_ maximum size of [flash memory](@ref flash_memory) for an
application, according to processor type is following:


<table>
<tr><td>efr32xg13pxxxf512</td><td>40kB</td></tr>
<tr><td>efr32xg23xxxxf512</td><td>40kB</td></tr>
</table>

@subsection ram_memory_efr32 RAM Memory available for application on EFR32

Allocated [RAM memory](@ref ram_memory) for application, by the processor is
following:


<table>
<tr><td>efr32xg13pxxxf512</td><td>16kB</td></tr>
<tr><td>efr32xg23xxxxf512</td><td>12kB</td></tr>
</table>

@subsection peripherals_accessible_by_stack_only_efr32 Peripherals accessible by stack only

Some peripherals are used by the Wirepas Mesh stack and cannot be used by the application.

<table>
<tr><th>Peripheral</th><th>Associated interrupt (from chip vendor files)</th></tr>
<tr><td><code>TIMER0</code></td><td><code>TIMER0_IRQn</code></td></tr>
<tr><td><code>RTCC</code></td><td><code>RTCC_IRQn</code></td></tr>
<tr><td><code>WDOG0</code></td><td><code>WDOG0_IRQn</code></td></tr>
<tr><td><code>CMU</code></td><td><code>CMU_IRQn</code></td></tr>
<tr><td><code>CRYPTO0</code></td><td><code>CRYPTO0_IRQn</code></td></tr>
</table>

All the internal interrupt of cortex M are handled by the stack directly (NMI, HardFault,...)

@subsection peripherals_shared_between_the_stack_and_the_application_efr32 Peripherals shared between the stack and the application

True Random Number Generator TRNG is available for application to use within App_init function.
After App_init returns, this peripheral is reserved for Wirepas Mesh stack and all initializations done in App_init may be overwritten.
Application may also take the control of RNG/TRNG by initializing the peripheral in scheduled task after App_init has returned and after
Wirepas Mesh stack has started. Do note that initialization must not take place within interrupt context as interrupt could be served
before this peripheral is released from Wirepas Mesh stack usage.

@subsection peripherals_available_for_the_application_efr32 Peripherals available for the application

All the other peripherals not listed above are free to be used by the application.

@section ti_resources_subg Resources on TI CC13x4

The following chip variants are supported:
-   CC1354P10
-   CC1354R10
-   CC1314R10

This page contains the following sections:
- @subpage flash_ti_subg
- @subpage ram_ti_subg
- @subpage peripherals_accessible_by_stack_only_ti_subg
- @subpage peripherals_available_for_the_application_ti_subg

@subsection flash_ti_subg Flash memory available for application on TI CC13x4

As stated in [description of memory partitioning](@ref memory_partitioning), the
available flash memory for application is limited by size of the memory area
that is used commonly for application and also scratchpad image. If application
size is too large, there is possibility that large scratchpad image will
override application image. The default maximum size of the application has been
set so that it is always safe to use scratchpad image that will contain both
firmware and application.

The _recommended_ maximum size of [flash memory](@ref flash_memory) for an
application, according to device type:
<table>
<tr><th>Device</th><th>Flash memory</th></tr>
<tr><td>CC1354P10</td><td>256 kB</td></tr>
<tr><td>CC1354R10</td><td>256 kB</td></tr>
<tr><td>CC1314R10</td><td>256 kB</td></tr>
</table>

@subsection ram_ti_subg RAM available for application on TI CC13x4

Allocated [RAM](@ref ram_memory) for application, by device type:
<table>
<tr><th>Device</th><th>RAM</th></tr>
<tr><td>CC1354P10</td><td>128 kB</td></tr>
<tr><td>CC1354R10</td><td>128 kB</td></tr>
<tr><td>CC1314R10</td><td>128 kB</td></tr>
</table>

@subsection peripherals_accessible_by_stack_only_ti_subg Peripherals accessible by stack only

Some peripherals are used by the Wirepas Mesh stack and cannot be used by the application.

<table>
<tr><th>Peripheral</th><th>Associated interrupts (from chip vendor files)</th></tr>
<tr><td><code>Radio</code></td><td><code>RFC_CPE_0_IRQn, RFC_CPE_1_IRQn, RFC_HW_COMB_IRQn, RFC_CMD_ACK_IRQn</code></td></tr>
<tr><td><code>PKA</code></td><td><code>PKA_IRQ_IRQn</code></td></tr>
<tr><td><code>AON RTC</code></td><td><code>AON_RTC_COMB_IRQn</code></td></tr>
<tr><td><code>Watchdog</code></td><td><code>WDT_IRQ_IRQn</code></td></tr>
<tr><td><code>GPT0</code></td><td><code>GPT0A_IRQn, GPT0B_IRQn</code></td></tr>
<tr><td><code>CRYPTO</code></td><td><code>CRYPTO_RESULT_AVAIL_IRQ_IRQn</code></td></tr>
<tr><td><code>TRNG</code></td><td><code>TRNG_IRQ_IRQn</code></td></tr>
<tr><td><code>AUX combined event</code></td><td><code>AUX_COMB_IRQn</code></td></tr>
<tr><td><code>Combined event from oscillator control</code></td><td><code>OSC_COMB_IRQn</code></td></tr>
</table>

All the internal interrupts of Cortex-M are handled by the stack directly (NMI, Hard Fault...)

@subsection peripherals_available_for_the_application_ti_subg Peripherals available for the application

All the other peripherals not listed above are free to be used by the application.


Related Material
================

@anchor relmat3 [3] WP-RM-108 - OTAP Reference Manual

@anchor relmat4 [4] https://github.com/wirepas/wm-sdk/blob/master/source/reference_apps/dualmcu_app/api/DualMcuAPI.md

*/


#endif /* API_DOC_SDK_ENVIRONMENT_H_ */
