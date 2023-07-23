#ifndef JLINKARM_NRF51_NRFJPROGDLL_H
#define JLINKARM_NRF51_NRFJPROGDLL_H


#include <stdint.h>
#include "DllCommonDefinitions.h"


#if defined(__cplusplus)
extern "C" {
#endif

/* Expected log function prototype for logging operations. */
typedef void msg_callback(const char * msg_str);


/**
 * @brief   Returns the JLinkARM DLL version.
 *
 * @details Returns the JLinkARM DLL version that has been opened in with the NRFJPROG_open_dll() function.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @param   major                               Pointer for storing of dll major version.
 * @param   minor                               Pointer for storing of dll minor version.
 * @param   revision                            Pointer for storing of dll revision.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_PARAMETER                   The major parameter is NULL.
 *                                              The minor parameter is NULL.
 *                                              The revision parameter is NULL.
 */
nrfjprogdll_err_t NRFJPROG_dll_version(uint32_t * major, uint32_t * minor, char * revision);


/**
 * @brief   Checks if the JLinkARM DLL is open.
 *
 * @details Checks if the NRFJPROG_open_dll() function has been called since the last call to NRFJPROG_close_dll() or since the loading of this dll.
 *
 * @param   opened                              Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The opened parameter is NULL.
 */
nrfjprogdll_err_t NRFJPROG_is_dll_open(bool * opened);


/**
 * @brief   Opens the JLinkARM DLL and sets the log callback. Prepares the dll for work with an nRF51 device.
 *
 * @details This function opens the JLinkARM DLL using the received path. The path should include the name of the DLL
 *          itself (i.e. "JLinkARM.dll"). Only JLinkARM DLLs whose versions are greater than a minimum version will be
 *          accepted. The minimum version for the JLinkARM DLL is defined in MIN_JLINK_MAJOR_VERSION and
 *          MIN_JLINK_MINOR_VERSION defines. The log callback may be NULL. In that case no logging mechanism is provided.
 *          The msg_callback typedef is defined elsewhere in this file. To close the dll, see NRFJPROG_close_dll() function.
 *
 * @pre     Before the execution of this function, the dll must not be open. To close the dll, see NRFJPROG_close_dll() function.
 *
 * @post    After the execution of this function, the JLINKARM DLL pointers will be loaded and some memory reserved. To unload the pointers and free the memory, see NRFJPROG_close_dll() function.
 *
 * @param   jlink_path                          Path to the JLinkARM DLL.
 * @param   cb                                  Callback for reporting informational and error messages.
 * @param   family                              Defines the device family the next commands are going to be called to.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has already been called.
 * @retval  INVALID_PARAMETER                   The jlink_path is NULL.
 *                                              The provided device family is not supported by this DLL.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 */
nrfjprogdll_err_t NRFJPROG_open_dll(const char * jlink_path, msg_callback * cb, device_family_t family);


/**
 * @brief   Closes and frees the JLinkARM DLL.
 *
 * @details Closes and frees the JLinkARM DLL. This function needs to be called before exiting if NRFJPROG_open_dll() has been called.
 *          After the execution of this function, the device CPU will not change its state from running or halted.
 *
 * @post    After the execution of this function, the JLINKARM DLL function pointers will be unloaded and the reserved memory freed. To open the dll, see NRFJPROG_open_dll() function.
 * @post    After the execution of this function, the PC will be disconnected from an emulator. To connect to an emulator, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @post    After the execution of this function, the emulator will be disconnected from the device. To connect to the device, see NRFJPROG_connect_to_device() function.
 */
void NRFJPROG_close_dll(void);


/**
 * @brief   Enumerates the serial numbers of connected USB J-Link emulators.
 *
 * @details This function asks the JLinkARM DLL how many USB J-Link emulators are connected to
 *          the PC, and writes that value into the num_available parameter. It also copies
 *          up to serial_numbers_len serial numbers into the serial_numbers array parameter.
 *          The function can be called with serial_numbers set to NULL and serial_numbers_len
 *          set to zero to obtain the number of connected emulators.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @param   serial_numbers                      Array in which to store the enumerated serial numbers.
 * @param   serial_numbers_len                  Number of uint32_t values that can be stored in the
 *                                              serial_numbers array (may be zero).
 * @param   num_available                       The number of serial numbers that were enumerated.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  INVALID_PARAMETER                   The serial_numbers parameter is NULL but serial_numbers_len is > 0.
 *                                              The num_available parameter is NULL.
 * @retval  OUT_OF_MEMORY                       Could not allocate buffer for reading serial numbers.
 */
nrfjprogdll_err_t NRFJPROG_enum_emu_snr(uint32_t serial_numbers[], uint32_t serial_numbers_len, uint32_t * num_available);


/**
 * @brief   Checks if the emulator has an established connection with Segger emulator/debugger.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @param   is_pc_connected_to_emu              Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_PARAMETER                   The is_connected_to_emu pointer is NULL.
 */
nrfjprogdll_err_t NRFJPROG_is_connected_to_emu(bool * is_pc_connected_to_emu);


/**
 * @brief   Connects to a given emulator/debugger.
 *
 * @details This function connects to serial_number emulator and sets the SWD communication speed at clock_speed_in_khz.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must not be established. To disconnect from an emulator, see NRFJPROG_disconnect_from_emu() function.
 * @pre     Before the execution of this function, the emulator must be physically connected to a powered board.
 *
 * @during  During the execution of this function, if the device was not in debug interface mode an unavoidable pin reset will occur. The pin reset will not occur if the debugger used is the Nordic's JLink On-Board-Debugger.
 *
 * @post    After the execution of this function, the PC will be connected to an emulator. To disconnect to the emulator, see NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @param   serial_number                       Serial number of the emulator to connect to.
 * @param   clock_speed_in_khz                  Speed for the SWD communication. It must be between JLINKARM_SWD_MIN_SPEED_KHZ
 *                                              and JLINKARM_SWD_MAX_SPEED_KHZ defined in DllCommonDefinitions.h. If the emulator
 *                                              does not support the input clock_speed, the emulators maximum supported speed
 *                                              will be used.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() has already been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  LOW_VOLTAGE                         Low voltage was detected at the target device.
 * @retval  INVALID_PARAMETER                   The clock_speed_in_khz parameter is not within limits.
 * @retval  EMULATOR_NOT_CONNECTED              The serial_number emulator is not connected to the PC.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_emu_with_snr(uint32_t serial_number, uint32_t clock_speed_in_khz);


/**
 * @brief   Connects to an emulator/debugger.
 *
 * @details This function connects to an available emulator and sets the SWD communication speed at clock_speed_in_khz.
 *          If more than one emulator is available, a pop-up window will appear to make a selection.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must not be established. To disconnect from an emulator, see NRFJPROG_disconnect_from_emu() function.
 * @pre     Before the execution of this function, the emulator must be physically connected to a powered board.
 *
 * @during  During the execution of this function, if the device was not in debug interface mode an unavoidable pin reset will occur. The pin reset will not occur if the debugger used is the Nordic's JLink On-Board-Debugger.
 *
 * @post    After the execution of this function, the PC will be connected to an emulator. To disconnect to the emulator, see NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @param   clock_speed_in_khz                  Speed for the SWD communication. It must be between JLINKARM_SWD_MIN_SPEED_KHZ
 *                                              and JLINKARM_SWD_MAX_SPEED_KHZ defined in DllCommonDefinitions.h. If the emulator
 *                                              does not support the input clock_speed, the emulators maximum supported speed
 *                                              will be used.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() has already been called.
 * @retval  NO_EMULATOR_CONNECTED               There is no emulator connected to the PC.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  LOW_VOLTAGE                         Low voltage was detected at the target device.
 * @retval  INVALID_PARAMETER                   The clock_speed_in_khz parameter is not within limits.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_emu_without_snr(uint32_t clock_speed_in_khz);


/**
 * @brief   Reads the serial number of the emulator connected to.
 *
 * @details Reads the serial number of the emulator connected to.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The serial_number pointer is NULL.
 */
nrfjprogdll_err_t NRFJPROG_read_connected_emu_snr(uint32_t * serial_number);


/**
 * @brief   Reads the firmware identification string of the emulator connected to.
 *
 * @details This function reads the firmware identification string of the emulator connected to into the
 *          given buffer. The function will read a maximum of buffer_size-1 characters into the buffer, and 0-terminate it. 
 *          Any excess characters are not read.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @param   buffer                              Pointer to buffer to contain the firmware string.
 * @param   buffer_size                         Size of the buffer. The user is responsible for ensuring a big enough buffer. A 255 byte buffer is suggested.
 *                                              Maximum buffer_size value is INT_MAX (2147483647).
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   NRFJPROG_open_dll() function has not been called.
 *                                              NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() has not been called.
 * @retval  INVALID_PARAMETER                   The character buffer pointer is a NULL-pointer.
 */
nrfjprogdll_err_t NRFJPROG_read_connected_emu_fwstr(char * buffer, uint32_t buffer_size);


/**
 * @brief   Disconnects from an emulator.
 *
 * @details This function disconnects from a connected emulator. This also disconnects from a connected device if connected. Will
 *          not fail if we have never connected to an emulator. After the execution of this function, the device CPU will not change
 *          its state from running or halted.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @post    After the execution of this function, the PC will be disconnected from an emulator. To connect to an emulator, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @post    After the execution of this function, the emulator will be disconnected from the device. To connect to the device, see NRFJPROG_connect_to_device() function.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 */
nrfjprogdll_err_t NRFJPROG_disconnect_from_emu(void);


/**
 * @brief   Recovers the device.
 *
 * @details This operation attempts to recover the device and leave it as it was when it left Nordic factory. It will attempt to
 *          connect, erase all user available flash, halt and eliminate any protection. Note that this operation may take up to 30 s.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 * @post    After the execution of this function, all device RAM will be powered. To unpower the device RAM, see NRFJPROG_unpower_ram_section() function.
 * @post    After the execution of this function, the device user available code and UICR flash will be erased.
 * @post    After the execution of this function, all peripherals will be reset.
 * @post    After the execution of this function, if the device was readback protected, the device will no longer be readback protected.
 * @post    After the execution of this function, the POWER.RESETREAS register will be cleared. Due to PAN-41, Lockup reset may be present for your device.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  RECOVER_FAILED                      Recover failed for any reason, including that the wrong family was used in the NRFJPROG_open_dll() function.
 */
nrfjprogdll_err_t NRFJPROG_recover(void);


/**
 * @brief   Checks if the connected emulator has an established connection with an nRF device.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @param   is_emu_connected_to_device          Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The is_emu_connected_to_device pointer is NULL.
 */
nrfjprogdll_err_t NRFJPROG_is_connected_to_device(bool * is_emu_connected_to_device);


/**
 * @brief   Connects to the nRF device.
 *
 * @details This function connects the emulator to the nRF device, powering the debug resources and entering debug interface mode.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, the emulator must not be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              The NRFJPROG_connect_to_device() function has already been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_device(void);


/**
 * @brief   Disconnects from the nRF device.
 *
 * @details Disconnects the emulator from the nRF device, by executing the exit sequence. If the emulator was not connected to the device, no operation is performed.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the emulator will be disconnected from the device. To connect to the device, see NRFJPROG_connect_to_device() function.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  EMULATOR_NOT_CONNECTED              The emulator with which a connection was established is no longer connected to the PC.
 */
nrfjprogdll_err_t NRFJPROG_disconnect_from_device(void);


/**
 * @brief   Protects the device against read or debug.
 *
 * @details Protects the device against read or debug by writing into UICR.RBPCONF and resetting. The function will reset
 *          with a sys reset after execution.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   desired_protection                  Desired protection level of readback protection.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The desired_protection is NONE.
 *                                              The desired_protection cannot be encoded in readback_protection_status_t.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_readback_protect(readback_protection_status_t desired_protection);


/**
 * @brief   Returns the status of the readback protection.
 *
 * @details Returns the status of the readback protection.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   status                              Pointer for storing of readback status.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The status pointer is NULL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_readback_status(readback_protection_status_t * status);


/**
 * @brief   Returns the region 0 size and source of protection if any.
 *
 * @details Returns the region 0 size and source of protection if any.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   size                                Pointer for storing of region 0 protection size.
 * @param   source                              Pointer for storing of region 0 protection source.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The size pointer is NULL.
 *                                              The source pointer is NULL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_read_region_0_size_and_source(uint32_t * size, region_0_source_t * source);


/**
 * @brief   Operation not available for devices of NRF51 family.
 *
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_debug_reset(void);


/**
 * @brief   Executes a system reset request.
 *
 * @details Executes a cortex-M standard sys reset request by the use of SCB.AIRCR register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_sys_reset(void);


/**
 * @brief   Executes a pin reset.
 *
 * @details Executes a pin reset by toggling SWIO and SWCLK lines as explained in the nRF51 user manual. NRFJPROG_pin_reset()
 *          function will not be available if the device is an NRF51xxx_xxAA_REV1 or NRF51xxx_xxAA_REV2 and is readback protected by PALL.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @during  During the execution of this function, the device CPU is halted. If the function execution fails, the device CPU might be left halted.
 * @during  During the execution of this function, the emulator mode is changed to JTAG. If the execution fails, the emulator might be left in JTAG mode. If the execution fails, execute NRFJPROG_close_dll() and start anew.
 * @during  During the execution of this function, a system reset will be executed.
 *
 * @post    After the execution of this function, the device will not be in debug interface mode. To enter debug interface mode, see NRFJPROG_connect_to_device() function.
 * @post    After the execution of this function, the emulator will be disconnected from the device. To connect to the device, see NRFJPROG_connect_to_device() function.
 * @post    After the execution of this function, the device CPU will be running. To halt the device CPU, see NRFJPROG_halt() function.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The device is one of the early versions and is readback protected by PALL.
 */
nrfjprogdll_err_t NRFJPROG_pin_reset(void);


/**
 * @brief   Disables NVM protection blocks.
 *
 * @details Disables NVM protection blocks by writing into register MPU.DISABLEINDEBUG.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @during  During the execution of this function, a system reset will be executed.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.

 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_disable_bprot(void);


/**
 * @brief   Erases all code and UICR flash.
 *
 * @details Erases all code and UICR flash regions. If the device is programmed at the Nordic factory with a SoftDevice, only Region 1 and UICR will be erased.
 *          Note that erase_all() will not disable NVM protection blocks if enabled while in debug interface mode. See NRFJPROG_disable_bprot()
 *          function to disable NVM protection blocks while in debug interface mode.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  NOT_AVAILABLE_BECAUSE_MPU_CONFIG    The operation is not available due to the MPU configuration. The operation is not available due to the presence of Pre-Programmed Factory Code (PPFC). 
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_erase_all(void);


/**
 * @brief   Erases a page of code flash.
 *
 * @details Erases a page of code flash containing the addr. Only available if the page addr is not configured in
 *          region 0, see NRFJPROG_read_region_0_size_and_source() function.  Note that NRFJPROG_erase_page() will not
 *          disable NVM protection blocks if enabled while in debug interface mode. See NRFJPROG_disable_bprot() funtion
 *          to disable NVM protection blocks while in debug interface mode.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   addr                                Address of the code flash page to erase.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The page is not erasable because it is configured in region 0.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_erase_page(uint32_t addr);


/**
 * @brief   Erases UICR info page.
 *
 * @details Erases UICR info page. Only available if the device is programmed at Nordic factory with a SoftDevice.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  NOT_AVAILABLE_BECAUSE_MPU_CONFIG    The operation is not available due to the MPU configuration. The operation is not available due to the absence of Pre-Programmed Factory Code (PPFC). 
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_erase_uicr(void);


/**
 * @brief   Writes one uint32_t data into the given address.
 *
 * @details Writes one uint32_t data to the given addr without verifying that the address is accessible or even exists.
 *          If nvmc_control is true, it will control the NVMC in order to write into flash. Writes need to be 32-bit aligned.
 *          Note that if the target address is in unpowered RAM, the operation will fail.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   addr                                Address to write to.
 * @param   data                                Value to write.
 * @param   nvmc_control                        If the target address needs NVMC control.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The addr parameter is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 *                                              The address to write is in unpowered RAM.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_write_u32(uint32_t addr, uint32_t data, bool nvmc_control);


/**
 * @brief   Reads one uint32_t from the given address.
 *
 * @details Reads one uint32_t data from the given addr without verifying that the address is accessible or even exists.
 *          Reads need to be 32-bit aligned. Note that if the target address is in unpowered RAM, the operation will fail.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   addr                                Address to read from.
 * @param   data                                Pointer of the location to store the value.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The addr parameter is not 32-bit aligned.
 *                                              The data parameter is null.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 *                                              The address to read is in unpowered RAM.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_read_u32(uint32_t addr, uint32_t * data);


/**
 * @brief   Writes data from the array into the device starting at the given address.
 *
 * @details Writes data_len bytes from the data array into the device starting at the given addr without verifying that the address is accessible
 *          or even exists. If nvmc_control is true, it will control the NVMC in order to write into flash. Writes need
 *          to be 32-bit aligned. Note that if the target address is in unpowered RAM, the operation will fail.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   addr                                Start address of the region to write to.
 * @param   data                                Pointer to an array with the data to write.
 * @param   data_len                            Length of the data array.
 * @param   nvmc_control                        If the target address needs NVMC control.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The addr parameter is not 32-bit aligned.
 *                                              The data_len parameter is 0.
 *                                              The data parameter is NULL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 *                                              The address to write is in unpowered RAM.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_write(uint32_t addr, const uint8_t * data, uint32_t data_len, bool nvmc_control);


/**
 * @brief   Reads data_len bytes from the device starting at the given address.
 *
 * @details Reads data_len bytes from the device starting at the given addr without verifying that the addresses are accessible or even
 *          exist. Note that if the target address is in unpowered RAM, the operation will fail.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   addr                                Address to read from.
 * @param   data                                Pointer of the location to store the value.
 * @param   data_len                            Number of bytes to read.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The data_len parameter is 0.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 *                                              The address to write is in unpowered RAM.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_read(uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief   Checks if the device CPU is halted.
 *
 * @details Checks if the device CPU is halted.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 *
 * @param   is_device_halted                    Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  INVALID_PARAMETER                   The is_device_halted pointer is NULL.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_is_halted(bool * is_device_halted);


/**
 * @brief   Halts the device CPU.
 *
 * @details Halts the device CPU.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_halt(void);


/**
 * @brief   Starts the device CPU with the given pc and sp.
 *
 * @details Starts the device CPU with the given pc and sp.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @during  During the execution of this function, the device CPU is halted. If the function execution fails, the device CPU might be left halted.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be running. To halt the device CPU, see NRFJPROG_halt() function.
 *
 * @param   pc                                  Program Counter to start running from.
 * @param   sp                                  Stack Pointer to use when running.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_run(uint32_t pc, uint32_t sp);


/**
 * @brief   Starts the device CPU.
 *
 * @details Starts the device CPU.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be running. To halt the device CPU, see NRFJPROG_halt() function.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_go(void);


/**
 * @brief   Runs the device CPU for one instruction.
 *
 * @details Runs the device CPU for one instruction and halts.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, the device CPU must be halted. To halt the device CPU, see NRFJPROG_halt() function.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              The device is not halted.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_step(void);


/**
 * @brief   Reads the number of RAM sections in the device.
 *
 * @details Reads the number of RAM sections in device and returns the result in input pointer.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset(), NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 *
 * @param   ram_sections_count                  Pointer of the location to store the number of RAM section in the device.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The ram_sections_count pointer is NULL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_count(uint32_t * ram_sections_count);


/**
 * @brief   Reads the size in bytes of the RAM sections in device.
 *
 * @details Reads the size of the RAM sections in device and returns the result in the input array in bytes. If input array size is larger
 *          than the number of RAM sections, the rest of the array will be left untouched.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset(), NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 *
 * @param   ram_sections_size                   Array to store the results.
 * @param   ram_sections_size_len               Size of ram_sections_size array.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The ram_sections_size pointer is NULL.
 *                                              The ram_sections_size_len is less than the number of RAM sections in device.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_size(uint32_t * ram_sections_size, uint32_t ram_sections_size_len);


/**
 * @brief   Reads the RAM sections power status.
 *
 * @details Reads the RAM sections power status and returns the result, on or off for each section, in ram_sections_power_status array.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, readback protection must not be enabled with PALL. To disable readback protection, see NRFJPROG_recover() function.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset(), NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 *
 * @param   ram_sections_power_status           Array to store the results.
 * @param   ram_sections_power_status_len       Size of ram_sections_power_status array.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The ram_sections_power_status pointer is NULL.
 *                                              The ram_sections_power_status_len is less than the number of RAM sections in device.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to readback protection.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_power_status(ram_section_power_status_t * ram_sections_power_status, uint32_t ram_sections_power_status_len);


/**
 * @brief   DEPRECATED. Please use NRFJPROG_read_ram_sections_power_status(), NRFJPROG_read_ram_sections_size() or NRFJPROG_read_ram_sections_count() functions instead.
 *
 * @details This function is DEPRECATED and replaced by NRFJPROG_read_ram_sections_power_status(), NRFJPROG_read_ram_sections_count() and NRFJPROG_read_ram_sections_size() functions.
 *          The function is functional, but should not be used as it will be removed in a future major release of the DLL.
 *
 *          Reads the RAM power status. Will read the status of the RAM sections and return if they are either on or off.
 *          Parameter ram_sections_number is used by the dll to indicate how many of the ram_sections_power_status are
 *          used to store results. Parameter ram_sections_size is used to report the size of those RAM sections. As a special
 *          use case, if ram_sections_power_status_array_size is 0, the dll will report the size and number of RAM power
 *          sections, but will not populate ram_sections_power_status array. If the device is readback protected by PALL,
 *          RAM power registers are not available for read, so status for all RAM blocks is reported as off.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, readback protection must not be enabled with PALL. To disable readback protection, see NRFJPROG_recover() function.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   ram_sections_power_status               Array to store the results.
 * @param   ram_sections_power_status_array_size    Size of ram_sections_power_status array.
 * @param   ram_sections_number                     Pointer of the location to store the number of RAM sections in the device.
 * @param   ram_sections_size                       Pointer of the location to store the size of RAM sections in the device.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The ram_sections_power_status pointer is NULL and ram_sections_power_status_array_size is different than 0.
 *                                              The ram_sections_size pointer is NULL.
 *                                              The ram_sections_number pointer is NULL.
 *                                              The ram_sections_power_status_array_size is not 0 but is less than the number of RAM power sections in the device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to readback protection.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_is_ram_powered(ram_section_power_status_t * ram_sections_power_status, uint32_t ram_sections_power_status_array_size, uint32_t * ram_sections_number, uint32_t * ram_sections_size);


/**
 * @brief   Powers up all RAM sections of the device.
 *
 * @details Powers up all the RAM of the device in ON state. Will not affect the RAM retention.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, readback protection must not be enabled with PALL. To disable readback protection, see NRFJPROG_recover() function.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to readback protection.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_power_ram_all(void);


/**
 * @brief   Powers down a RAM section of the device.
 *
 * @details Powers down a RAM section of the device in ON state. Will not affect the RAM retention.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, readback protection must not be enabled with PALL. To disable readback protection, see NRFJPROG_recover() function.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   section_index                       Section of RAM to power down.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The section section_index does not exist in the device.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to readback protection.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_unpower_ram_section(uint32_t section_index);


/**
 * @brief   Reads a CPU register.
 *
 * @details Reads a CPU register. Valid registers are R0-R15, XPSR, MSP and PSP.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   register_name                       Register name to read. See cpu_registers_t definition for valid values.
 * @param   register_value                      Pointer of the location to store the read register.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The register_data parameter is null.
                                                The register_name parameter is not a valid register.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_read_cpu_register(cpu_registers_t register_name, uint32_t * register_value);


/**
 * @brief   Writes a CPU register.
 *
 * @details Writes a CPU register. Valid registers are R0-R15, XPSR, MSP and PSP.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   register_name                       CPU register to write. See cpu_registers_t definition for valid values.
 * @param   register_value                      Value to write into the CPU register.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The register_name parameter is not a valid register.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 */
nrfjprogdll_err_t NRFJPROG_write_cpu_register(cpu_registers_t register_name, uint32_t register_value);


/**
 * @brief   Reads the version of the device connected to the emulator.
 *
 * @details Reads the version of the device connected to the emulator. If the device is not an NRF51 device version parameter will be set to UNKNOWN and WRONG_FAMILY_FOR_DEVICE error returned. 
 *          If the device is a newer revision of an NRF51, but this revision is not known at compile time of this dll, version parameter will be set to UNKNOWN and SUCCESS returned.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_pin_reset(), NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @param   version                             Pointer of the location to store the device type.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The version parameter is null.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_read_device_version(device_version_t * version);


/**
 * @brief   Operation not available for this dll.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed in this dll.
 */
nrfjprogdll_err_t NRFJPROG_read_device_family(device_family_t * family);


/**
 * @brief   Reads a debug port register.
 *
 * @details Reads into data pointer a debug port register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, the emulator must not be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 *
 * @param   reg_addr                            Register address to read, either in debug port or access port.
 * @param   data                                Pointer of the location to store the value read.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              The NRFJPROG_connect_to_device() function has already been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_read_debug_port_register(uint8_t reg_addr, uint32_t * data);


/**
 * @brief   Writes a debug port register.
 *
 * @details Writes data parameter into a debug port register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, the emulator must not be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 *
 * @param   reg_addr                            Register address to write, either in debug port or access port.
 * @param   data                                Data to write into the register.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              The NRFJPROG_connect_to_device() function has already been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_write_debug_port_register(uint8_t reg_addr, uint32_t data);


/**
 * @brief   Reads a debugger access port register.
 *
 * @details Reads into data pointer a debugger access port register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, the emulator must not be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 *
 * @param   ap_index                            Access port index for read if ap_access.
 * @param   reg_addr                            Register address to read, either in debug port or access port.
 * @param   data                                Pointer of the location to store the value read.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              The NRFJPROG_connect_to_device() function has already been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_read_access_port_register(uint8_t ap_index, uint8_t reg_addr, uint32_t * data);


/**
 * @brief   Writes a debugger access port register.
 *
 * @details Writes data parameter into a debugger access port register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, the emulator must not be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 *
 * @param   ap_index                            Access port index for write if ap_access.
 * @param   reg_addr                            Register address to write, either in debug port or access port.
 * @param   data                                Data to write into the register.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              The NRFJPROG_connect_to_device() function has already been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_write_access_port_register(uint8_t ap_index, uint8_t reg_addr, uint32_t data);


/**
 * @brief   Checks if the RTT is started.
 *
 * @details Checks if the NRFJPROG_rtt_start() function has been called since the last call to NRFJPROG_rtt_stop() or since the loading of this dll.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @param   started                             Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_PARAMETER                   The started parameter is null.
 */
nrfjprogdll_err_t NRFJPROG_is_rtt_started(bool * started);


/**
 * @brief   Indicates to the dll the location of the RTT control block in the device memory.
 *
 * @details Indicates to the dll the location of the RTT control block in the device memory, to accelerate the discovery of the RTT control block.
 *          If the RTT control block is not located at the address given, NRFJPROG_rtt_start() will never locate the RTT control block.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, the RTT must not be started. To stop the RTT, see NRFJPROG_rtt_stop() function.
 *
 * @param   address                             RTT control block address in device memory.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_rtt_start() function has been called.
 */
nrfjprogdll_err_t NRFJPROG_rtt_set_control_block_address(uint32_t address);


/**
 * @brief   Starts RTT.
 *
 * @details Starts RTT, initiating the search for the RTT control block and preparing the DLL for RTT operations. If NRFJPROG_rtt_set_control_block_address() has been called,
 *          the address provided will be used to locate the control block. Since NRFJPROG_rtt_start() function is not blocking, to find out if NRFJPROG_rtt_start() has located
 *          the RTT control block and is ready to receive commands, see NRFJPROG_rtt_is_control_block_found() function. After the execution of NRFJPROG_rtt_start(), please
 *          ensure that NRFJPROG_rtt_stop() function will be executed.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, the RTT must not be started. To stop the RTT, see NRFJPROG_rtt_stop() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, the RTT control block must be present in RAM. Normally it is the firmware execution that writes the control block into RAM. To run the firmware, see NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset() function.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              The NRFJPROG_rtt_start() function has already been called.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF51.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_start(void);


/**
 * @brief   Checks if RTT control block has been found.
 *
 * @details Checks if RTT control block has been found. Can be used to make sure RTT is ready to be used before calling NRFJPROG_rtt_read() and NRFJPROG_rtt_write() functions.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   is_control_block_found              Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The is_found parameter is null.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_is_control_block_found(bool * is_control_block_found);


/**
 * @brief   Stops RTT.
 *
 * @details Stops RTT and clears the RTT Control Buffer.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @post    After the execution of this function, the RTT control block in RAM will have been erased. Therefore, before another NRFJPROG_rtt_start() can be issued, the control block must be rewritten to RAM. See NRFJPROG_rtt_start() function for details.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_stop(void);


/**
 * @brief   Reads from an RTT channel.
 *
 * @details Reads up to data_len characters from the up_channel_index RTT channel.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   up_channel_index                    RTT channel index to read from.
 * @param   data                                Pointer of the location to store the value.
 * @param   data_len                            Number of bytes to read.
 * @param   data_read                           Pointer to the location to store the actual number of read characters.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The data_read parameter is null.
 *                                              There is no channel in the device with the given up_channel_index index.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read(uint32_t up_channel_index, char * data, uint32_t data_len, uint32_t * data_read);


/**
 * @brief   Writes to an RTT channel.
 *
 * @details Writes the given data_len length data string into the down_channel_index RTT channel.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   down_channel_index                  RTT channel index to write to.
 * @param   data                                Pointer to a string with the string to write.
 * @param   data_len                            Length of the string.
 * @param   data_written                        Pointer to the location to store the actual number of written characters.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The data_read parameter is null.
 *                                              There is no channel in the device with the given down_channel_index index.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_write(uint32_t down_channel_index, const char * data, uint32_t data_len, uint32_t * data_written);


/**
 * @brief   Gets the number of RTT channels.
 *
 * @details Gets the number of down and up channels in the device.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   down_channel_number                 Pointer to store the number of down channels.
 * @param   up_channel_number                   Pointer to store the number of up channels.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The down_channel_number parameter is null.
 *                                              The up_channel_number parameter is null.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_count(uint32_t * down_channel_number, uint32_t * up_channel_number);


/**
 * @brief   Reads the info from one RTT channel.
 *
 * @details Reads the info from one RTT channel of index channel_index of direction dir. Reads the rtt channel size into channel_size and name into channel_name pointers.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   channel_index                       Channel index to read the info from.
 * @param   dir                                 Channel direction of channel to read the info from.
 * @param   channel_name                        Pointer to store the channel name, must be able to store at least 32 characters.
 * @param   channel_name_len                    Size of channel_name storage size.
 * @param   channel_size                        Pointer to store the channel size.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The channel_name parameter is null.
 *                                              The channel_size parameter is null.
 *                                              The channel_size_len parameter is less than 32.
 *                                              The dir parameter is neither UP_DIRECTION or DOWN_DIRECTION defined in rtt_direction_t enum in DllCommonDefinitions.h
 *                                              The channel with index channel_index does not exist in the device.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_info(uint32_t channel_index, rtt_direction_t dir, char * channel_name, uint32_t channel_name_len, uint32_t * channel_size);


/**
 * @brief   Since NRF51 devices do not include a QSPI peripheral, returns false.
 *
 * @param   initialized                         Pointer of the location to store the result. 
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_PARAMETER                   The initialized parameter is null. 
 */
nrfjprogdll_err_t NRFJPROG_is_qspi_init(bool * initialized);



/**
 * @brief   Operation not available for devices of NRF51 family.
 *
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_qspi_init(bool retain_ram, const qspi_init_params_t * init_params);


/**
 * @brief   Operation not available for devices of NRF51 family.
 *
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_qspi_uninit(void);


/**
 * @brief   Operation not available for devices of NRF51 family.
 *
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_qspi_read(uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief   Operation not available for devices of NRF51 family.
 *
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_qspi_write(uint32_t addr, const uint8_t * data, uint32_t data_len);


/**
 * @brief   Operation not available for devices of NRF51 family.
 *
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_qspi_erase(uint32_t addr, qspi_erase_len_t length);


/**
 * @brief   Operation not available for devices of NRF51 family.
 *
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_qspi_custom(uint8_t instruction_code, uint8_t instruction_length, const uint8_t * data_in, uint8_t * data_out);


#if defined(__cplusplus)
}
#endif

#endif  /* JLINKARM_NRF51_NRFJPROGDLL_H */
