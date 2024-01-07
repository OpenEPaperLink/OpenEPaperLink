/********************************************************************************************************
 * @file    MassStorageClassCommon.h
 *
 * @brief   This is the header file for MassStorageClassCommon
 *
 * @author  Driver & Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

#pragma once

/* Includes: */
#include "types.h"
#include "StdDescriptors.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#pragma pack(1)

/* Macros: */
/** Magic signature for a Command Block Wrapper used in the Mass Storage Bulk-Only transport protocol. */
#define MS_CBW_SIGNATURE                               0x43425355UL

/** Magic signature for a Command Status Wrapper used in the Mass Storage Bulk-Only transport protocol. */
#define MS_CSW_SIGNATURE                               0x53425355UL

/** Mask for a Command Block Wrapper's flags attribute to specify a command with data sent from host-to-device. */
#define MS_COMMAND_DIR_DATA_OUT                        (0 << 7)

/** Mask for a Command Block Wrapper's flags attribute to specify a command with data sent from device-to-host. */
#define MS_COMMAND_DIR_DATA_IN                         (1 << 7)

/** \name SCSI Commands*/
//@{
/** SCSI Command Code for an INQUIRY command. */
#define SCSI_CMD_INQUIRY                               0x12

/** SCSI Command Code for a REQUEST SENSE command. */
#define SCSI_CMD_REQUEST_SENSE                         0x03

/** SCSI Command Code for a TEST UNIT READY command. */
#define SCSI_CMD_TEST_UNIT_READY                       0x00

/** SCSI Command Code for a READ CAPACITY (10) command. */
#define SCSI_CMD_READ_CAPACITY_10                      0x25

/** SCSI Command Code for a SEND DIAGNOSTIC command. */
#define SCSI_CMD_SEND_DIAGNOSTIC                       0x1D

/** SCSI Command Code for a PREVENT ALLOW MEDIUM REMOVAL command. */
#define SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL          0x1E

/** SCSI Command Code for a WRITE (10) command. */
#define SCSI_CMD_WRITE_10                              0x2A

/** SCSI Command Code for a READ (10) command. */
#define SCSI_CMD_READ_10                               0x28

/** SCSI Command Code for a WRITE (6) command. */
#define SCSI_CMD_WRITE_6                               0x0A

/** SCSI Command Code for a READ (6) command. */
#define SCSI_CMD_READ_6                                0x08

/** SCSI Command Code for a VERIFY (10) command. */
#define SCSI_CMD_VERIFY_10                             0x2F

/** SCSI Command Code for a MODE SENSE (6) command. */
#define SCSI_CMD_MODE_SENSE_6                          0x1A

/** SCSI Command Code for a MODE SENSE (10) command. */
#define SCSI_CMD_MODE_SENSE_10                         0x5A
//@}

/** \name SCSI Sense Key Values */
//@{
/** SCSI Sense Code to indicate no error has occurred. */
#define SCSI_SENSE_KEY_GOOD                            0x00

/** SCSI Sense Code to indicate that the device has recovered from an error. */
#define SCSI_SENSE_KEY_RECOVERED_ERROR                 0x01

/** SCSI Sense Code to indicate that the device is not ready for a new command. */
#define SCSI_SENSE_KEY_NOT_READY                       0x02

/** SCSI Sense Code to indicate an error whilst accessing the medium. */
#define SCSI_SENSE_KEY_MEDIUM_ERROR                    0x03

/** SCSI Sense Code to indicate a hardware error has occurred. */
#define SCSI_SENSE_KEY_HARDWARE_ERROR                  0x04

/** SCSI Sense Code to indicate that an illegal request has been issued. */
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST                 0x05

/** SCSI Sense Code to indicate that the unit requires attention from the host to indicate
 *  a reset event, medium removal or other condition.
 */
#define SCSI_SENSE_KEY_UNIT_ATTENTION                  0x06

/** SCSI Sense Code to indicate that a write attempt on a protected block has been made. */
#define SCSI_SENSE_KEY_DATA_PROTECT                    0x07

/** SCSI Sense Code to indicate an error while trying to write to a write-once medium. */
#define SCSI_SENSE_KEY_BLANK_CHECK                     0x08

/** SCSI Sense Code to indicate a vendor specific error has occurred. */
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC                 0x09

/** SCSI Sense Code to indicate that an EXTENDED COPY command has aborted due to an error. */
#define SCSI_SENSE_KEY_COPY_ABORTED                    0x0A

/** SCSI Sense Code to indicate that the device has aborted the issued command. */
#define SCSI_SENSE_KEY_ABORTED_COMMAND                 0x0B

/** SCSI Sense Code to indicate an attempt to write past the end of a partition has been made. */
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW                 0x0D

/** SCSI Sense Code to indicate that the source data did not match the data read from the medium. */
#define SCSI_SENSE_KEY_MISCOMPARE                      0x0E
//@}

/** \name SCSI Additional Sense Codes */
//@{
/** SCSI Additional Sense Code to indicate no additional sense information is available. */
#define SCSI_ASENSE_NO_ADDITIONAL_INFORMATION          0x00

/** SCSI Additional Sense Code to indicate that the logical unit (LUN) addressed is not ready. */
#define SCSI_ASENSE_LOGICAL_UNIT_NOT_READY             0x04

/** SCSI Additional Sense Code to indicate an invalid field was encountered while processing the issued command. */
#define SCSI_ASENSE_INVALID_FIELD_IN_CDB               0x24

/** SCSI Additional Sense Code to indicate that a medium that was previously indicated as not ready has now
 *  become ready for use.
 */
#define SCSI_ASENSE_NOT_READY_TO_READY_CHANGE          0x28

/** SCSI Additional Sense Code to indicate that an attempt to write to a protected area was made. */
#define SCSI_ASENSE_WRITE_PROTECTED                    0x27

/** SCSI Additional Sense Code to indicate an error whilst formatting the device medium. */
#define SCSI_ASENSE_FORMAT_ERROR                       0x31

/** SCSI Additional Sense Code to indicate an invalid command was issued. */
#define SCSI_ASENSE_INVALID_COMMAND                    0x20

/** SCSI Additional Sense Code to indicate a write to a block out outside of the medium's range was issued. */
#define SCSI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE 0x21

/** SCSI Additional Sense Code to indicate that no removable medium is inserted into the device. */
#define SCSI_ASENSE_MEDIUM_NOT_PRESENT                 0x3A
//@}

/** \name SCSI Additional Sense Key Code Qualifiers */
//@{
/** SCSI Additional Sense Qualifier Code to indicate no additional sense qualifier information is available. */
#define SCSI_ASENSEQ_NO_QUALIFIER                      0x00

/** SCSI Additional Sense Qualifier Code to indicate that a medium format command failed to complete. */
#define SCSI_ASENSEQ_FORMAT_COMMAND_FAILED             0x01

/** SCSI Additional Sense Qualifier Code to indicate that an initializing command must be issued before the issued
 *  command can be executed.
 */
#define SCSI_ASENSEQ_INITIALIZING_COMMAND_REQUIRED     0x02

/** SCSI Additional Sense Qualifier Code to indicate that an operation is currently in progress. */
#define SCSI_ASENSEQ_OPERATION_IN_PROGRESS             0x07
//@}

/* Enums: */
/** Enum for possible Class, Subclass and Protocol values of device and interface descriptors relating to the Mass
 *  Storage device class.
 */
enum MS_Descriptor_ClassSubclassProtocol_t
{
	MS_CSCP_MassStorageClass          = 0x08, /**< Descriptor Class value indicating that the device or interface
	                                           *   belongs to the Mass Storage class.
	                                           */
	MS_CSCP_SCSITransparentSubclass   = 0x06, /**< Descriptor Subclass value indicating that the device or interface
	                                           *   belongs to the SCSI Transparent Command Set subclass of the Mass
	                                           *   storage class.
	                                           */
	MS_CSCP_BulkOnlyTransportProtocol = 0x50, /**< Descriptor Protocol value indicating that the device or interface
	                                           *   belongs to the Bulk Only Transport protocol of the Mass Storage class.
	                                           */
};

/** Enum for the Mass Storage class specific control requests that can be issued by the USB bus host. */
enum MS_ClassRequests_t
{
	MS_REQ_GetMaxLUN                  = 0xFE, /**< Mass Storage class-specific request to retrieve the total number of Logical
	                                           *   Units (drives) in the SCSI device.
	                                           */
	MS_REQ_MassStorageReset           = 0xFF, /**< Mass Storage class-specific request to reset the Mass Storage interface,
	                                           *   ready for the next command.
                                               */
};

/** Enum for the possible command status wrapper return status codes. */
enum MS_CommandStatusCodes_t
{
	MS_SCSI_COMMAND_Pass              = 0, /**< Command completed with no error */
	MS_SCSI_COMMAND_Fail              = 1, /**< Command failed to complete - host may check the exact error via a
	                                        *   SCSI REQUEST SENSE command.
	                                        */
	MS_SCSI_COMMAND_PhaseError        = 2, /**< Command failed due to being invalid in the current phase. */
};

/* Type Defines: */
/** \brief Mass Storage Class Command Block Wrapper.
 *
 *  Type define for a Command Block Wrapper, used in the Mass Storage Bulk-Only Transport protocol.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u32 Signature; /**< Command block signature, must be \ref MS_CBW_SIGNATURE to indicate a valid Command Block. */
	u32 Tag; /**< Unique command ID value, to associate a command block wrapper with its command status wrapper. */
	u32 DataTransferLength; /**< Length of the optional data portion of the issued command, in bytes. */
	u8  Flags; /**< Command block flags, indicating command data direction. */
	u8  LUN; /**< Logical Unit number this command is issued to. */
	u8  SCSICommandLength; /**< Length of the issued SCSI command within the SCSI command data array. */
	u8  SCSICommandData[16]; /**< Issued SCSI command in the Command Block. */
}  MS_CommandBlockWrapper_t;

/** \brief Mass Storage Class Command Status Wrapper.
 *
 *  Type define for a Command Status Wrapper, used in the Mass Storage Bulk-Only Transport protocol.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u32 Signature; /**< Status block signature, must be \ref MS_CSW_SIGNATURE to indicate a valid Command Status. */
	u32 Tag; /**< Unique command ID value, to associate a command block wrapper with its command status wrapper. */
	u32 DataTransferResidue; /**< Number of bytes of data not processed in the SCSI command. */
	u8  Status; /**< Status code of the issued command - a value from the \ref MS_CommandStatusCodes_t enum. */
}  MS_CommandStatusWrapper_t;

/** \brief Mass Storage Class SCSI Sense Structure
 *
 *  Type define for a SCSI Sense structure. Structures of this type are filled out by the
 *  device via the \ref MS_Host_RequestSense() function, indicating the current sense data of the
 *  device (giving explicit error codes for the last issued command). For details of the
 *  structure contents, refer to the SCSI specifications.
 */
typedef struct
{
	u8  ResponseCode;

	u8  SegmentNumber;

	u8 SenseKey            : 4;
	u8 Reserved            : 1;
	u8 ILI                 : 1;
	u8 EOM                 : 1;
	u8 FileMark            : 1;

	u8  Information[4];
	u8  AdditionalLength;
	u8  CmdSpecificInformation[4];
	u8  AdditionalSenseCode;
	u8  AdditionalSenseQualifier;
	u8  FieldReplaceableUnitCode;
	u8  SenseKeySpecific[3];
} SCSI_Request_Sense_Response_t;

/** \brief Mass Storage Class SCSI Inquiry Structure.
 *
 *  Type define for a SCSI Inquiry structure. Structures of this type are filled out by the
 *  device via the \ref MS_Host_GetInquiryData() function, retrieving the attached device's
 *  information.
 *
 *  For details of the structure contents, refer to the SCSI specifications.
 */
typedef struct
{
	u8 DeviceType          : 5;
	u8 PeripheralQualifier : 3;

	u8 Reserved            : 7;
	u8 Removable           : 1;

	u8  Version;

	u8 ResponseDataFormat  : 4;
	u8 Reserved2           : 1;
	u8 NormACA             : 1;
	u8 TrmTsk              : 1;
	u8 AERC                : 1;

	u8  AdditionalLength;
	u8  Reserved3[2];

	u8 SoftReset           : 1;
	u8 CmdQue              : 1;
	u8 Reserved4           : 1;
	u8 Linked              : 1;
	u8 Sync                : 1;
	u8 WideBus16Bit        : 1;
	u8 WideBus32Bit        : 1;
	u8 RelAddr             : 1;

	u8  VendorID[8];
	u8  ProductID[16];
	u8  RevisionID[4];
} SCSI_Inquiry_Response_t;

#pragma pack()

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif



