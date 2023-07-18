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
 
#ifndef MERGEHEX_H
#define MERGEHEX_H

//@TODO: Have the build system update this.
#define major_version (9) 
#define minor_version (7) 
#define micro_version (2) 

enum MergehexErrorCodesType {
    
    Success                                 = 0,                // Requested operation (operations) were successfully completed.
    
    /* mergehex.exe or PC errors */
    MergehexError                           = 1,                // An error condition that should not occur has happened. 
                                                                // It is most probably a bug in mergehex.exe. 
    MemoryAllocationError                   = 3,                // Memory allocation for mergehex failed.
        
    /* Command line input errors */
    InvalidArgumentError                    = 11,               // Invalid arguments passed to the application.
    InsufficientArgumentsError              = 12,               // Needed arguments not passed to the application.
    IncompatibleArgumentsError              = 13,               // Incompatible arguments passed to the application.
    DuplicatedArgumentsError                = 14,               // The same argument has been provided twice.
    NoOperationError                        = 15,               // The arguments passed do not perform a valid operation.

    /* General errors */
    FileNotFoundError                       = 51,               // Unable to find the given file.
    InvalidHexFileError                     = 52,               // File specified does not seem a valid hex file.
    NoWritePermissionError                  = 56,               // megehex application has no write permission in the given path.
    ConflictError                           = 60,               // There is a conflict in the hex files to merge.
};



#endif //MERGEHEX_H