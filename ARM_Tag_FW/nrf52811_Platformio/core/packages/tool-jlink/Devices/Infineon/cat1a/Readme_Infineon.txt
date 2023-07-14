The Infineon devices support comes without any warranty and support from
SEGGER Microcontroller GmbH. Support is provided via Infineon only.
For more support, please contact: 
https://www.infineon.com/cms/en/about-infineon/company/contacts/support/
===============================================================================

  Infineon devices support in SEGGER Software and Documentation Pack:
  - PSoC6A-1M/2M/512K/256K (CY8C6xxx)
  - PSoC64-2M/1M/512K devices (CYB064xA, CYS064xA, CYB064x7, and CYB064x5)
  
  Contents:
  |- Alias (targets)
  |- Tips and tricks
  |  |- Serial Memory Interface (SMIF) for the external QSPI Flash
  |  |- Electronic fuses (eFuse)
  |  |- ETM Trace configuration
  |- References

===============================================================================
> Alias (targets)
===============================================================================

* SEGGER’s database has no records for each PSoC 6 MPN. Instead, we are using
  device family aliases containing the list and characteristics of the memory
  banks, paths to the Flash Loaders (*.FLM), and the path to the script file
  (*.JLinkScript) with additional initialization methods. Following are the
  naming pattern for the aliases and usage recommendations:

  CY<FW>6xx<MEMORY>_<CORE>[_sect256KB][_tm] , where:
  CY        Static prefix
  FW        Firmware:
            8C - Standard
            B0 - Secure Boot v1
            S0 - Std. Secure - AWS
  6xx       PSoC 6 Architecture, any line and speed
  MEMORY    Memory Size (Flash/SRAM):
            4 - 256K/128K
            5 - 512K/256K
            6 - 512K/128K
            7 - 1024K/512K
            A - 2048K/1024K
  CORE      Cortex Core to connect:
            CM0p - Cortex M0+
            CM4  - Cortex M4
  sect256KB Erase Sector Size set to 256 KB. Use such aliases for better
            performance if the granularity is not essential. Do not use the
            aliases with '_sect256KB' suffix if you need better granularity:
            for example, to program two independent images into two sequential
            flash rows (512 bytes each).
  tm        Test Mode acquisition/connection method. It is recommended
            to use the aliases with '_tm' suffix to avoid potential issues with
            the device connection by the debugger. This is also the only
            reliable method to connect to the device under specific
            circumstances: for example, if the application is using the debug
            pins as GPIO, disables the debug access port, or doing other system
            changes that can prevent debugger access. Note: this method works
            only with the SWD communication protocol and doesn't work with JTAG.

  -----------------------------------------------------------------------------
  Supported aliases           | Flash | Cortex | Erase  | Acquisition (Reset)
                              | (KB)  | Core   | Sector | Method
  -----------------------------------------------------------------------------
  CY8C6xx4_CM0p               | 256   | M0+    | 512 B  | Default
  CY8C6xx4_CM0p_sect128KB     | 256   | M0+    | 128 KB | Default
  CY8C6xx4_CM0p_tm            | 256   | M0+    | 512 B  | Test Mode
  CY8C6xx4_CM0p_sect128KB_tm  | 256   | M0+    | 128 KB | Test Mode
  CY8C6xx4_CM4                | 256   | M4     | 512 B  | Default
  CY8C6xx4_CM4_sect128KB      | 256   | M4     | 128 KB | Default
  CY8C6xx5_CM0p               | 512   | M0+    | 512 B  | Default
  CY8C6xx5_CM0p_sect256KB     | 512   | M0+    | 256 KB | Default
  CY8C6xx5_CM0p_tm            | 512   | M0+    | 512 B  | Test Mode
  CY8C6xx5_CM0p_sect256KB_tm  | 512   | M0+    | 256 KB | Test Mode
  CY8C6xx5_CM4                | 512   | M4     | 512 B  | Default
  CY8C6xx5_CM4_sect256KB      | 512   | M4     | 256 KB | Default
  CY8C6xx6_CM0p               | 512   | M0+    | 512 B  | Default
  CY8C6xx6_CM0p_sect256KB     | 512   | M0+    | 256 KB | Default
  CY8C6xx6_CM0p_tm            | 512   | M0+    | 512 B  | Test Mode
  CY8C6xx6_CM0p_sect256KB_tm  | 512   | M0+    | 256 KB | Test Mode
  CY8C6xx6_CM4                | 512   | M4     | 512 B  | Default
  CY8C6xx6_CM4_sect256KB      | 512   | M4     | 256 KB | Default
  CY8C6xx7_CM0p               | 1024  | M0+    | 512 B  | Default
  CY8C6xx7_CM0p_sect256KB     | 1024  | M0+    | 256 KB | Default
  CY8C6xx7_CM0p_tm            | 1024  | M0+    | 512 B  | Test Mode
  CY8C6xx7_CM0p_sect256KB_tm  | 1024  | M0+    | 256 KB | Test Mode
  CY8C6xx7_CM4                | 1024  | M4     | 512 B  | Default
  CY8C6xx7_CM4_sect256KB      | 1024  | M4     | 256 KB | Default
  CY8C6xxA_CM0p               | 2048  | M0+    | 512 B  | Default
  CY8C6xxA_CM0p_sect256KB     | 2048  | M0+    | 256 KB | Default
  CY8C6xxA_CM0p_tm            | 2048  | M0+    | 512 B  | Test Mode
  CY8C6xxA_CM0p_sect256KB_tm  | 2048  | M0+    | 256 KB | Test Mode
  CY8C6xxA_CM4                | 2048  | M4     | 512 B  | Default
  CY8C6xxA_CM4_sect256KB      | 2048  | M4     | 256 KB | Default
  CYB06xx7_CM0p               | 1024  | M0+    | 512 B  | Test Mode
  CYB06xx7_CM0p_tm            | 1024  | M0+    | 512 B  | Test Mode
  CYB06xx7_CM4                | 1024  | M4     | 512 B  | Test Mode
  CYB06xx7_CM4_tm             | 1024  | M4     | 512 B  | Test Mode
  CYB06xxA_CM0p               | 2048  | M0+    | 512 B  | Test Mode
  CYB06xxA_CM0p_tm            | 2048  | M0+    | 512 B  | Test Mode
  CYB06xxA_CM4                | 2048  | M4     | 512 B  | Test Mode
  CYB06xxA_CM4_tm             | 2048  | M4     | 512 B  | Test Mode
  CYS06xxA_CM0p               | 2048  | M0+    | 512 B  | Test Mode
  CYS06xxA_CM0p_tm            | 2048  | M0+    | 512 B  | Test Mode
  CYS06xxA_CM4                | 2048  | M4     | 512 B  | Test Mode
  CYS06xxA_CM4_tm             | 2048  | M4     | 512 B  | Test Mode
  CYB06xx5_CM0p               | 512   | M0+    | 512 B  | Test Mode
  CYB06xx5_CM0p_tm            | 512   | M0+    | 512 B  | Test Mode
  CYB06xx5_CM4                | 512   | M4     | 512 B  | Test Mode
  CYB06xx5_CM4_tm             | 512   | M4     | 512 B  | Test Mode
  -----------------------------------------------------------------------------

===============================================================================
> Tips and tricks
===============================================================================

* Serial Memory Interface (SMIF) for the external QSPI Flash
  Programming of the external flash memory devices via the SMIF IP block is
  supported using a flash loader - application compiled for a target CPU that
  implements programming algorithms and executed from MCUs SRAM. Flash loaders
  developed by Infineon have the following requirements:
  - An external flash device must be write-enabled and mapped to the eXecute in
    Place (XIP) region in (within the address range 0x18000000 - 0x1FFFFFFF).
  - Configuration data for the SMIF driver (generated by QSPI Configurator
    tool) must be in the application flash. The Loader uses this data to
    determine the design-specific settings of the external memory devices and
    to initialize the SMIF driver for further read/write operations.
  - The Table of Contents Part2 (TOC2) must contain the pointer to the location
    in flash where the configuration data for the SMIF driver is placed. TOC2
    is the predefined 512-byte-wide data structure, located at address
    0x1600 6C00 in the Sflash region.
  - Data for the external memory device in the input binary file (hex, elf,
    etc.) must be allocated in the exact range of XIP address space, where the
    memory device is mapped.
  These requirements necessitate a specific order of memory bank operations:
  - For program operation, the application and supervisory flash banks must be
    programmed first, before the SMIF bank is programmed.
  - For erase operation, SMIF bank must be erased first, before the application
    and supervisory flash banks are erased.

* Electronic fuses (eFuse)
  Because eFuses stores the critical data for the device functionality, blowing
  an eFuse is an irreversible process that requires the associated I/O supply
  to be at a specific level, programming is recommended only in mass production
  programming under controlled factory conditions and not prototyping stages.
  For more details, refer to the programming specifications and technical
  reference manual (TRM) documents.
  If you know what you are doing and still want to program the eFuse data using
  the SEGGER tools, you must enable the "EFUSE" bank in JLinkDevices.xml file
  for the target alias.

* ETM Trace configuration
  By default, the TRACECLK and TRACEDATA pins are initialized for the standard
  signals routing on common demo boards as in the following example:
                                     | TCLK | TD0  | TD1  | TD2  | TD3
  ---------------------------------------------------------------------
  A) CY8CKIT-062-WIFI-BT Pioneer Kit | P7_0 | P9_3 | P7_6 | P7_5 | P7_4
  B) PSoC 62S2 Wi-Fi BT Pioneer Kit  | P7_0 | P9_3 | P9_2 | P9_1 | P9_0
  If you are using the custom mapping, follow the comments in OnTraceStart
  function of CY8C6xxx_CM4.JLinkScript file to change the pins assignment.
  
===============================================================================
> Known issues and limitations
===============================================================================

* ID: PROGTOOLS-98
  Issue: Segger J-Link Commander and J-Flash Lite tools cannot read and do not
  write eFuse bits correctly. These tools do not fill in gaps in the hex file
  with the correct values for eFuse bits and use a different read technique
  than the Segger J-Flash tool.
  Workaround: Use the Segger J-Flash tool. To program eFuse bits with J-Link
  Commander and J-Flash Lite tools, manually edit the hex file so that the
  eFuse region (0x90700000-0x907003FF) is filled in with data. Fuses, which
  must not be touched, should be filled with the 0xFF (ignore) value in that
  hex file.

* ID: PROGTOOLS-2274
  Issue: Incorrect behavior (invalid RAM size, erase operation failures) might
  be observed in J-Flash tool while using PSoC 64 MCU devices after changing
  the target device from CM0+ to CM4. It is caused by the fact that J-Flash
  continues to work via CM0+ core but uses RAM region which is dedicated to CM4
  core. No reconnection happens automatically after changing the core.
  Workaround: Perform a disconnecting before changing the MCU core.

* ID: None
  The recommended targets (aliases) for PSoC 6 devices in SEGGER tools have the
  '_tm' suffix (for example CY8C6xA_CM0P_tm) and use the Test Mode
  connection method. Please use them for programming to avoid potential issues
  with the device connection by debugger when using SEGGER's default
  acquisition method (aliases without '_tm' suffix). Use targets without the
  '_tm' suffix when using the JTAG and not SWD protocol.

===============================================================================
> References
===============================================================================
  [1] J-Link / J-Trace User Guide (https://www.segger.com/downloads/jlink/UM08001)
  [2] PSoC 6 MCU Programming Specifications (Document Number: 002-15554 https://www.infineon.com/dgdl/Infineon-PSoC_6_Programming_Specifications-Programming+Specifications-v12_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f66d9bf5627)
  [3] PSoC 64 MCU Programming Specifications (Document Number: 002-31353 http://www.cypress.com/file/521461)
