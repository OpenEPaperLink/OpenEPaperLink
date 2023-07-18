#ifndef JLINKARM_UNKNOWN_NRFJPROGDLL_H
#define JLINKARM_UNKNOWN_NRFJPROGDLL_H


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
 * @brief   Opens the JLinkARM DLL and sets the log callback. Prepares the dll for work with an nRF52 device.
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
 * @post    After the execution of this function, the device will not be in debug interface mode. To enter debug interface mode, see NRFJPROG_connect_to_device() function.
 * @post    After the execution of this function, the PC will be disconnected from an emulator. To connect to an emulator, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
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
 * @post    After the execution of this function, the device will not be in debug interface mode. To enter debug interface mode, see NRFJPROG_connect_to_device() function.
 * @post    After the execution of this function, the PC will be disconnected from an emulator. To connect to an emulator, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_disconnect_from_emu(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_recover(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_connected_to_device(bool * is_emu_connected_to_device);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_device(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_disconnect_from_device(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_readback_protect(readback_protection_status_t desired_protection);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_readback_status(readback_protection_status_t * status);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_region_0_size_and_source(uint32_t * size, region_0_source_t * source);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_debug_reset(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_sys_reset(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_pin_reset(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_disable_bprot(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_erase_all(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_erase_page(uint32_t addr);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_erase_uicr(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_write_u32(uint32_t addr, uint32_t data, bool nvmc_control);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_u32(uint32_t addr, uint32_t * data);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_write(uint32_t addr, const uint8_t * data, uint32_t data_len, bool nvmc_control);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read(uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_halted(bool * is_device_halted);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_halt(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_run(uint32_t pc, uint32_t sp);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_go(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_step(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_count(uint32_t * ram_sections_count);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_size(uint32_t * ram_sections_size, uint32_t ram_sections_size_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_power_status(ram_section_power_status_t * ram_sections_power_status, uint32_t ram_sections_power_status_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_ram_powered(ram_section_power_status_t * ram_sections_power_status, uint32_t ram_sections_power_status_array_size, uint32_t * ram_sections_number, uint32_t * ram_sections_size);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_power_ram_all(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_unpower_ram_section(uint32_t section_index);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_cpu_register(cpu_registers_t register_name, uint32_t * register_value);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_write_cpu_register(cpu_registers_t register_name, uint32_t register_value);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_device_version(device_version_t * version);


/**
 * @brief   Reads the family of the device connected to the emulator.
 *
 * @details Reads the family of the device connected to the emulator by the use of NRFJPROG_read_access_port_register() function.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see the appropriate family header file for functions that can take the device out of debug interface mode.
 *
 * @param   family                              Pointer of the location to store the device family.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The family parameter is null.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error. 
 */
nrfjprogdll_err_t NRFJPROG_read_device_family(device_family_t * family);


/**
 * @brief   Reads a debug port register.
 *
 * @details Reads into data pointer a debug port register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @param   reg_addr                            Register address to read, either in debug port or access port.
 * @param   data                                Pointer of the location to store the value read.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
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
 *
 * @param   reg_addr                            Register address to write, either in debug port or access port.
 * @param   data                                Data to write into the register.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
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
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset(), NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @param   ap_index                            Access port index for read if ap_access.
 * @param   reg_addr                            Register address to read, either in debug port or access port.
 * @param   data                                Pointer of the location to store the value read.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
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
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset(), NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @param   ap_index                            Access port index for write if ap_access.
 * @param   reg_addr                            Register address to write, either in debug port or access port.
 * @param   data                                Data to write into the register.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_write_access_port_register(uint8_t ap_index, uint8_t reg_addr, uint32_t data);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_rtt_started(bool * started);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_set_control_block_address(uint32_t address);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_start(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_is_control_block_found(bool * is_control_block_found);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_stop(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read(uint32_t up_channel_index, char * data, uint32_t data_len, uint32_t * data_read);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_write(uint32_t down_channel_index, const char * data, uint32_t data_len, uint32_t * data_written);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_count(uint32_t * down_channel_number, uint32_t * up_channel_number);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_info(uint32_t channel_index, rtt_direction_t dir, char * channel_name, uint32_t channel_name_len, uint32_t * channel_size);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_qspi_init(bool * initialized);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_init(bool retain_ram, const qspi_init_params_t * init_params);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_uninit(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_read(uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_write(uint32_t addr, const uint8_t * data, uint32_t data_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_erase(uint32_t addr, qspi_erase_len_t length);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_custom(uint8_t instruction_code, uint8_t instruction_length, const uint8_t * data_in, uint8_t * data_out);

#if defined(__cplusplus)
}
#endif

#endif  /* JLINKARM_UNKNOWN_NRFJPROGDLL_H */
