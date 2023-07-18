/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 */
 
#ifndef NRFJPROG_H
#define NRFJPROG_H

#define major_version (9) 
#define minor_version (7) 
#define micro_version (2) 


 enum NrfjprogErrorCodesType {
    
	Success                                     = 0,				// Requested operation (operations) were successfully completed.
    
	/* nrfjprog.exe or PC errors */
	NrfjprogError								= 1,		        // An error condition that should not occur has happened. 
                                                                    // It is most probably a bug in nrfjprog.exe or nrfjprog.dll. 
	NrfjprogOutdatedError						= 2,				// Nrfjprog version is too old for the device 
	MemoryAllocationError						= 3,				// Memory allocation for nrfjprog failed failed.
	
	
	/* Command line input errors */
    InvalidArgumentError						= 11,				// Invalid arguments passed to the application.
    InsufficientArgumentsError					= 12,				// Needed arguments not passed to the application.
    IncompatibleArgumentsError					= 13,				// Incompatible arguments passed to the application.
    DuplicatedArgumentsError					= 14,				// The same argument has been provided twice.
	NoOperationError							= 15,				// The arguments passed do not perform a valid operation.
	UnavailableOperationBecauseProtectionError	= 16,				// The operation attempted can not be performed because either the main-ap or the ctrl-ap is not available.
	UnavailableOperationInFamilyError			= 17,				// The operation attempted can not be performed in the device because the feature is lacking in your device.
	WrongFamilyForDeviceError					= 18,				// The --family option given with the command (or the default from nrfjprog.ini) does not match the device connected.
    UnavailableOperationBecauseMpuConfiguration = 19,               // For nRF51, --eraseuicr is unavailable unless the device came with an ANT softdevice programmed at Nordic factory.
    

	/* nrfjprog.dll errors */
    NrfjprogDllNotFoundError					= 20,				// Unable to find nrfjprog.dll in the installation folder. Reinstall nrfjprog.
    NrfjprogDllLoadFailedError					= 21,				// Failed to Load nrfjprog.dll
    NrfjprogDllFunctionLoadFailedError			= 22,				// Failed to Load the functions from nrfjprog.dll
	NrfjprogDllNotImplementedError				= 23,				// Dll still does not implement this function for your device.

    /* nrfjprog.ini errors */
    NrfjprogIniSyntaxError                      = 24,               // Syntax error in nrfjprog.ini file
    NrfjprogIniNotFoundError                    = 25,               // Unable to find nrfjprog.ini in the installation folder. Reinstall nrfjprog.
    NrfjprogIniCannotBeOpenedError              = 26,               // Opening the nrfjprog.ini file for read failed.
    NrfjprogIniFamilyMissingError               = 27,               // Family parameter cannot be parsed from ini file. Line might be deleted or invalid format.
    NrfjprogIniClockspeedMissingError           = 28,               // Clockspeed parameter cannot be parsed from ini file. Line might be deleted or invalid format.
    
	/* JLinkARM.dll errors */
    JLinkARMDllNotFoundError					= 30,				// Unable to find install path for JLink software
	JLinkARMDllInvalidError						= 31,				// Dll found does not seem a valid dll.
	JLinkARMDllFailedToOpenError				= 32,				// Dll could not be opened.
	JLinkARMDllError							= 33,				// Dll reported error.
	JLinkARMDllTooOldError						= 34,				// Dll is too old for functionality. Install a newer version of JLinkARM.dll
 
	/* Emulator errors */
    InvalidSerialNumberError					= 40,				// Serial number provided is not among those connected.
    NoDebuggersError							= 41,				// There are no debuggers connected to the PC.
 	NotPossibleToConnectError					= 42,				// Not possible to connect to the NRF device.
	LowVoltageError								= 43,				// Low voltage detected at target device.
	
	/* General errors */
    FileNotFoundError							= 51,				// Unable to find the given file.
	InvalidHexFileError							= 52,				// File specified does not seem a valid hex file.
	FicrReadError								= 53,				// FICR read failed.
	WrongArgumentError							= 54,				// One of the arguments is wrong. Path does not exist, memory access is not aligned...
	VerifyError									= 55,				// The write verify operation failed.
	NoWritePermissionError						= 56,				// Unable to create file in the current working directory.
	NVMCOperationError							= 57,				// The flash operation in the device failed.
	FlashNotErasedError							= 58,				// A program operation failed because the area to write was not erased.
	RamIsOffError								= 59,				// The RAM area to read or write is unpowered.
	NoReadPermissionError                       = 60,               // Unable to open file for read.
    NoExternalMemoryConfiguredError             = 61,               // A QSPI operation is attempted without an external memory configured.
    RecoverFailed                               = 62,               // Recovery failed.

    /* QSPI ini parsing. */
    NrfjprogQspiIniNotFoundError                = 70,               // Unable to find QspiDefault.ini file in the installation directory or given with option --qspiini.
    NrfjprogQspiIniCannotBeOpenedError          = 71,               // Opening the QSPI ini file for read failed.
    NrfjprogQspiSyntaxError                     = 72,               // The QSPI ini file has some syntax error.
    NrfjprogQspiIniParsingError                 = 73,               // QSPI ini file parsed has one or more missing keys.
    NrfjprogQspiIniCustomMissingError           = 74,               // QSPI ini file parsed has no custom instructions specified, but option --qspicustominit option was given.

	/* Warning. Will not be returned by nrfjprog but used to generate warnings. */
	FicrOperationWarning						= 100,				// FICR operation. It is important to be certain of what you do.
	UnalignedPageEraseWarning					= 101,				// Address provided with page erase is not aligned to first address of page.
	NoLogWarning								= 102,				// No log is possible because the program has no write permission in the current directory.
	UicrWriteOperationWithoutEraseWarning		= 103,				// A UICR write operation is requested but there has not been a UICR erase.
    VeryLongOperationWarning                    = 104               // An operation that might take up to several minutes is been execued. Please wait.
};

#endif //NRFJPROG_H
