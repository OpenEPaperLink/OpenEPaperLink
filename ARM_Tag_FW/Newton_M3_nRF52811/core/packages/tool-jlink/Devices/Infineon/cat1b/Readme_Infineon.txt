The Infineon devices support comes without any warranty and support from
SEGGER Microcontroller GmbH. Support is provided via Infineon only.
For more support, please contact: 
https://www.infineon.com/cms/en/about-infineon/company/contacts/support/
===============================================================================

  Infineon devices support in SEGGER Software and Documentation Pack:
  - AIROC™ CYW20829 Bluetooth® LE system on chip
  
  Contents:
  |- Alias (targets)
  |- Tips and tricks
  |  |- Serial Memory Interface (SMIF) for the external QSPI Flash
  |  |- SWO Trace configuration
  |- References

===============================================================================
> Alias (targets)
===============================================================================

* SEGGER’s database has no records for each CYW20xx MPN. Instead, we are using
  device family aliases containing the list and characteristics of the memory
  banks, paths to the Flash Loaders (*.FLM), and the path to the script file
  (*.JLinkScript) with additional initialization methods. Following are the
  naming pattern for the aliases and usage recommendations: CYW208xx[_tm],
  where '_tm' is the Test Mode acquisition/connection method. It is
  recommended to use the aliases with the '_tm' suffix to avoid potential
  issues with the device connection by the debugger. This is also the only
  reliable method to connect to the device under specific circumstances: for
  example, if the application is using the debug pins as GPIO, disables the
  debug access port, or doing other system changes that can prevent debugger
  access. Note: this method works only with the SWD communication protocol and
  doesn't work with JTAG.

  -----------------------------------------------------------
  Supported aliases  | Flash | Cortex | Acquisition (Reset)
                     | (KB)  | Core   | Method
  -----------------------------------------------------------
  CYW208xx           | -     | M33    | Default
  CYW208xx_tm        | -     | M33    | Test Mode and
                     |       |        | alternate methods
  -----------------------------------------------------------

===============================================================================
> Tips and tricks
===============================================================================

* Serial Memory Interface (SMIF) for the external QSPI Flash
  Programming of the external flash memory devices via the SMIF IP block is
  supported using a flash loader - application compiled for a target CPU that
  implements programming algorithms and executed from MCUs SRAM. Flash loaders
  developed by Infineon have the following requirements:
  - An external flash device must be write-enabled and mapped to the eXecute in
    Place (XIP) region in (within the address range 0x60000000 - 0x67FFFFFF).
  - Data for the external memory device in the input binary file (hex, elf,
    etc.) must be allocated in the exact range of XIP address space, where the
    memory device is mapped.
  - Use the QSPI Configurator tool to patch the flash loader with the SMIF
    configuration data if your project requires a custom connection or the
    flash memory device does not support SFDP. By default, flash loader uses
    a common SMIF configuration, where the flash device is used as the Slave#0
    and mapped to the beginning of the XIP region.

* SWO Trace configuration
  24.5-bit divider [0] is used by default for the trace clock setup. If you use
  this divider in your application or need to change the division value, you
  have to set the TRACE_div_* variables in the JLinkScript file associated with
  the target alias (check in JLinkDevices.xml).

===============================================================================
> Known issues and limitations
===============================================================================

* ID: None
  The recommended targets (aliases) for AIROC™ CYW20829 Bluetooth® LE system on
  chip in SEGGER tools have the '_tm' suffix (for example CYW208xx_tm) and use
  the Test Mode connection method. Please use them for programming to
  avoid potential issues with the device connection by debugger when using
  SEGGER's default connection method (aliases without '_tm' suffix).

* ID: PROGTOOLS-2529
  Issue: Cannot read, save, or verify the data from the external QSPI memory in
  SEGGER J-Link Commander. This tool does not support the indirect reading of
  the memory regions that are not mapped by default.
  Workaround: Use the SEGGER J-Flash tool.

* ID: PROGTOOLS-3367
  Issue: With JTAG protocol and the target device set to "CYW20829", flash
  operations may fail under specific circumstances: for example, when the
  debugger listen window is set to "0 ms".
  Workaround: Infineon recommends using the SWD instead as a faster and more
  robust protocol for programming and debugging. If your design requires JTAG,
  please set the target device to "CYW20829_tm". If you are debugging several
  devices in one JTAG chain, you must specify the chain configuration in the
  JTAG_CHAIN_CONF_STR constant in the CYW208xx_tm.JLinkScript file.

===============================================================================
> References
===============================================================================
  [1] J-Link / J-Trace User Guide (https://www.segger.com/downloads/jlink/UM08001)
  [2] AIROC(TM) CYW20829 Bluetooth(R) LE system on chip Programming specification (Document Number: 002-32463)
