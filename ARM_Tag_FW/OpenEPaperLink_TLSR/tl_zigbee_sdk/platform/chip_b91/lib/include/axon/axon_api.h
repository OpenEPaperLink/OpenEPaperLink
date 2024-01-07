/********************************************************************************************************
 * @file	axon_api.h
 *
 * @brief	This is the header file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include <stdint.h>
#include <stdbool.h>

/**
 * axon driver functions return either a positive success value
 * or a negative error code.
 */
typedef enum {
  kAxonResultFailureInvalidAsyncMode = -16,/**< returned by AxonApiExecuteOps and AxonApiQueueOpsList if the other is busy (these are mutually exclusive) */
  kAxonResultFailureInputOutOfRange = -15, /**< Input parameter value out of allowed range */
  kAxonResultFailureMissingNullCoef = -14, /**< FIR requires last filter coefficient to be 0. */
  kAxonResultFailureNullBuffer = -13,      /**< one or more required buffers is NULL */
  kAxonResultFailureInvalidRounding = -12, /**< Rounding value specified is out of range */
  kAxonResultFailureIBadOpHandle = -10,    /**< All internal buffers are in use. Free some operations or allocate more buffers. */
  kAxonResultFailureNoMoreBuffers = -9,    /**< All internal buffers are in use. Free some operations or allocate more buffers. */
  kAxonResultFailureInvalidDataWidth = -8, /**< requested data_width is not supported by the function */
  kAxonResultFailureUnalignedBuffer = -7,  /**< One or more buffers do not meet alignment requirements for the requested data_width  */
  kAxonResultFailureUnsupportedHw = -6,    /**< axon driverhardware version is unsupported */
  kAxonResultFailureHwError = -5,          /**< error within axon hardware */
  kAxonResultFailureBadHandle = -4,        /**< Unable to acquire the mutex to access the Axon */
  kAxonResultFailureMutexFailed = -3,      /**< Unable to acquire the mutex to access the Axon */
  kAxonResultFailureInvalidLength = -2,    /**< length provided in the input struct was invalid */
  kAxonResultFailure = -1,                 /**< generic failure code */
  kAxonResultSuccess = 0,                  /**< success */
  kAxonResultNotFinished = 1,              /**< An async operation hasn't completed */
  kAxonResultFailureOverflow = 2,          /**< HW overflow occurs, give a warning */
} AxonResultEnum;

typedef enum {
  kAxonBoolFalse,
  kAxonBoolTrue
} AxonBoolEnum;

/*
 * Enum for asynchronous modes of operation that is supplied to the discrete and
 * batch operation APIs.
 * The two synchronous modes are kAxonAsyncModeSynchronous and kAxonAsyncModeSyncWithWfi. In either of these
 * two modes the function will return after the operation has completed (ie, blocking mode).
 * kAxonAsyncModeSynchronous:
 * use the CPU to poll status bits
 */
typedef enum {
  kAxonAsyncModeSynchronous, /**< Synchronous mode w/ software polling of hardware status without sleeping. Axon interrupts are disabled. */
  kAxonAsyncModeAsynchronous, /**< Asynchronous mode. Axon interrupts are enabled. */
  kAxonAsyncModeSyncWithWfi, /**< Synchronous mode, but driver invokes AxonHostWfi() to allow processor to sleep. Axon interrupts are enabled.*/
} AxonAsyncModeEnum;



/*
 * Data width enum and packing
 * Packing results in data elements being stored in less memory, but also impacts buffer alignment.
 * See comments for each value.
 */
typedef enum {
  kAxonDataWidth24 = 0, /**< data elements are 24bits wide, aligned to 32bits regardless of packing */
  kAxonDataWidth16,     /**< data elements are 16bits wide, buffer must be aligned to 8 bytes and data elements aligned to 16bits with packing enabled */
  kAxonDataWidth12,     /**< data elements are 12bits wide,  buffer must be aligned to 8 bytes and data elements aligned to 16bits with packing enabled */
  kAxonDataWidth8,      /**< data elements are 8bits wide,  buffer must be aligned to 16 bytes and data elements aligned to 16bits with packing enabled */
  kAxonDataWidthCount = 0x3,
  kAxonDataWidthMask = 0x3,
  kAxonDataWidth16to24 = kAxonDataWidth24 | (kAxonDataWidth16<<2),
  kAxonDataWidth12to24 = kAxonDataWidth24 | (kAxonDataWidth12<<2),
  kAxonDataWidth8to24 = kAxonDataWidth24 | (kAxonDataWidth8<<2),
  kAxonDataWidth8to16 = kAxonDataWidth16 | (kAxonDataWidth8<<2),
  kAxonDataWidth8to12 = kAxonDataWidth12 | (kAxonDataWidth8<<2),
  kAxonDataWidthNoExt = 0,
}AxonDataWidthEnum;

typedef enum {
  kAxonDataPackingDisabled, /**< data elements aligned to 32bits regardless of data width. buffers also aligned to 4bytes */
  kAxonDataPackingEnabled /**< data element alignment and buffer alignment requirements are specific to the data width */
} AxonDataPackEnum;

/**
 * Axon activation functions. These non-linearities are
 * applied to the output of an operation.
 * They can also be applied as an explicit operation.
 */
typedef enum {
  kAxonAfDisabled, /**< No activation function is run on the output */
  kAxonAfRelu,     /**< Relu function is run on the output */
  kAxonAfSigmoid,  /**< Sigmoid is run on the output */
  kAxonAfTanh,     /**< Tanh is run on the output */
  kAxonAfQuantSigmoid, /**< adds .5 to the number then clamps between 0 & 1 */
} AxonAfEnum;
/**
 * Output Rounding
 * Rounding will shift the output values by the number of bits
 * specified, then add 1 if the shifted MSB is a 1.
 */
typedef enum {
  kAxonRoundingNone = 0,  /**< numbers are not rounded   */
  kAxonRoundingMax = 32 /**< Maximum supported rounding value */
} AxonRoundingEnum;

/*
 * Buffer stepping
 * Axon can be configured to step over elements in a vector. This is useful
 * in cases where vectors are interleaved in the same array (stereo audio for example).
 */
typedef enum {
  kAxonStride0 = 0, /**< iterate over the same data element (effectively a scalar) */
  kAxonStride1 = 1, /**< data elements are in adjacent indexes in vector */
  kAxonStride2 = 2, /**< data elements are in every other index in vector */
  kAxonStrideEnumCount /**< number of items in enum, used for range checking of input */
} AxonStrideEnum;
/**
 * Generic input structure for Axon functions.
 * Not all fields are used by all functions; see function comments
 * for usage.
 */
typedef struct {
  AxonDataWidthEnum data_width; /**< width (in bits) of all data used/produced by the function */
  AxonDataPackEnum  data_packing;    /**< indicates if data is packed or not */
  AxonRoundingEnum  output_rounding; /**< level of rounding applied to the output */
  AxonAfEnum        output_af;       /**< Specifies which (if any) activation function to apply to the output. */
  AxonStrideEnum x_stride;           /**< spacing of input elements in vector. */
  AxonStrideEnum y_stride;           /**< spacing of input elements in vector. */
  AxonStrideEnum q_stride;           /**< spacing of output elements in vector. */
  uint16_t length;        /**< number of elements in the x vector. Other vectors may be of this size depending on the function being called */
  uint16_t y_length;      /**< number of elements in the y vector. Only used on functions were y vector length is allowed to be different from the x vector. */
  const int32_t *x_in;         /**< X input vector address */
  const int32_t *y_in;         /**< Y input vector address */
  int32_t a_in;         /**< A input scalar value */
  int32_t b_in;         /**< B input scalar value */
  int32_t *q_out;        /**< Q output vector address */
} AxonInputStruct;

/**
 * AxonOpHandles are used to track pre-defined axon operations. The user has the choice of defining and
 * executing an axon operation immediately, or defining the operation one time and referencing it multiple times
 * in the future.
 *
 * Defining the operation and referencing it in future execution sequences has the advantages of being more
 * efficient and allowing multiple operations to execute sequentially with little to no software intervention.
 */
typedef void *AxonOpHandle;

/**
 * Used for queued operations mode. In this mode,
 * Multiple operation lists can be queued up, each with a distinct
 * callback function. The callback function will only be invoked when the
 * command list is completed.
 * Axon interrupts will be processed in the interrupt context; AxonHostInterruptNotification() will not be invoked.
 */
typedef struct AxonMgrQueuedOpsStruct{
  AxonOpHandle *op_handle_list;        /**< pointer to the list of op handles to execute */
  uint8_t op_handle_count;             /**< number of op handles in op_handle_list */
  uint8_t resvd[3];                    /**< preserve 32bit alignment even if packed structures are enabled */
  void *callback_context;              /**< caller-provided pointer that is provided in the callback function. */
  void (*callback_function)(AxonResultEnum result, void *callback_context);  /**< caller-provided function to be invoked when the operation list is completed */
  struct AxonMgrQueuedOpsStruct *next;
}AxonMgrQueuedOpsStruct;


/**
 * Calculates an FFT, q_out=FFT(x_in)
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 * @param axon_input populated by the caller with the input and output vector size and locations.
 *   axon_input->data_width Must be kAxonDataWidth24
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the vector. Must be 2^n, where n is in [5..9] (ie, 32, 64,...256,512)
 *   axon_input->x_in   Complex, 24bit input vector aligned on 32bit boundaries.
 *                     The real component "r" of element number "n" Xrn is followed by the imaginary component Xin.
 *                     followed by the imaginary component such that the indices of the real component (r)
 *                     Xrn = x_in[n*2].
 *                     Xin = x_in[n*2+1].
 *   axon_input->q_out  Complex, 24bit output vector of the same format and size as x_in
 *   axon_input->x_stride  spacing of input elements in vector.
 *   axon_input->q_stride  spacing of output elements in vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *  The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 *  @param async_mode
 *  See AxonAsyncModeEnum for options.
 */
AxonResultEnum AxonApiFft(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiFft.

 * @param axon_handle
 *   see @AxonApiFft
 *
 * @param axon_input
 *   see @AxonApiFft

 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpFft(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);


/**
 * "FIR"=>"Finite Impulse Response" filter, Calculates an FIR, q_out=FIR(x_in, y_in)
 * @param axon_input populated by the caller with the input and output vector size and locations.
 *   axon_input->data_width Can be any value in AxonDataWidthEnum. Note that any width other than kAxonDataWidth24 requires
 *                         that x_in, y_in, and q_out all be 16byte aligned.
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 4, and be at least 4 greater than y_length
 *   axon_input->y_length  Number of elements in the y vector (filter coefficients). Must be a at least 12, a multiple of 4 and the last coefficient must be 0.
 *                         0 pad to 12 if less than 12 coefficients are needed.
 *   axon_input->x_in   Samples input to the filter. 24bit values aligned on 32bit boundaries.
 *   axon_input->y_in   Filter coefficients, 24bit aligned on 32bit boundaries.
 *   axon_input->q_out  24bit output vector sized to length-y_length
 *   axon_input->x_stride  spacing of input elements in input vector.
 *   axon_input->y_stride  spacing of input elements in filter tap vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 *   Remaining axon_input fields are unused.
 */
AxonResultEnum AxonApiFir(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiFir.

 * @param axon_handle
 *   see @AxonApiFir
 *
 * @param axon_input
 *   see @AxonApiFir

 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpFir(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);


/**
 * "Sqrt"=> "Square Root" Calculates a the square root of vector X, q_out=SQRT(x_in)
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vector size and locations.
 *   axon_input->data_width must be kAxonDataWidth24.
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector. 24bit integer values aligned on 32bit boundaries
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 *   Remaining axon_input fields are unused.
 */
AxonResultEnum AxonApiSqrt(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiSqrt.

 * @param axon_handle
 *   see @AxonApiSqrt
 *
 * @param axon_input
 *   see @AxonApiSqrt

 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpSqrt(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);


/**
 * Calculates the natural log of a vector, q_out=logn(x_in)
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vector size and locations.
 *   axon_input->data_width must be kAxonDataWidth24.
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector. 24bit values aligned on 32bit boundaries in Q11.12 format (ie, fixed point)
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 *   Remaining axon_input fields are unused.
 */
AxonResultEnum AxonApiLogn(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiLogn.
 *
 * @param axon_handle
 *   see @AxonApiLogn
 *
 * @param axon_input
 *   see @AxonApiLogn
 *
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpLogn(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * Raises "e" to the power of vector X, q_out=e^(x_in). Values must be positive. Negative values will have unpredicatable results.
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vector size and locations.
 *   axon_input->data_width must be kAxonDataWidth24.
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector. 24bit values aligned on 32bit boundaries in Q11.12 format (ie, fixed point)
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 *   Remaining axon_input fields are unused.
 */
AxonResultEnum AxonApiExp(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiExp.
 * @param axon_input
 *   see @AxonApiExp
 * @param axon_handle
 *   see @AxonApiExp
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpExp(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);


/**
 * "Xpy"=>"X plus Y" Adds vectors X and Y.
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->y_in   Y input vector, same format as x_in.
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->y_stride  spacing of input elements in input Y vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiXpy(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiXpy.
 * @param axon_input
 *   see @AxonApiXpy
 * @param axon_handle
 *   see @AxonApiXpy
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpXpy(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Xmy"=>"X minus Y" Subtracts vector Y from vector X.
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->y_in   Y input vector, same format as x_in.
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->y_stride  spacing of input elements in input Y vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiXmy(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiXmy.
 * @param axon_input
 *   see @AxonApiXmy
 * @param axon_handle
 *   see @AxonApiXmy
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpXmy(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Xspys"=>"X Squared Plus Y Squared" adds the square of vector Y to the square of vector X.
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->y_in   Y input vector, same format as x_in.
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->y_stride  spacing of input elements in input Y vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiXspys(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiXspys.
 * @param axon_input
 *   see @AxonApiXspys
 * @param axon_handle
 *   see @AxonApiXspys
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpXspys(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Xsmys"=>"X Squared Minus Y Squared" subtracts the square of vector Y to the square of vector X.
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->y_in   Y input vector, same format as x_in.
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->y_stride  spacing of input elements in input Y vector.
 *   axon_input->q_stride  spacing of output elements in output vector.

 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiXsmys(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiXsmys.
 * @param axon_input
 *   see @AxonApiXsmys
 * @param axon_handle
 *   see @AxonApiXsmys
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpXsmys(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Xty"=>"X Times Y" multiplies vector Y with vector X.
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->y_in   Y input vector, same format as x_in.
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->y_stride  spacing of input elements in input Y vector.
 *   axon_input->q_stride  spacing of output elements in output vector.

 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiXty(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiXty.
 * @param axon_input
 *   see @AxonApiXty
 * @param axon_handle
 *   see @AxonApiXty
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpXty(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Axpby"=>"aX+bY" Sum product of scalar "a" and X with product of scalar "b" and Y.
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->y_in   Y input vector, same format as x_in.
 *   axon_input->a_in   a scalar value
 *   axon_input->b_in   b scalar value
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->y_stride  spacing of input elements in input Y vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiAxpby(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * "AxpbyPointer"=>"aX+bY" Sum product of scalar "a" and X with product of scalar "b" and Y.
 * @param axon_handle
 *   see @AxonApiAxpby
 * @param axon_input
 *    see @AxonApiAxpby with only the following changes:
 *   axon_input->a_in   pointer to "a" scalar value
 *   axon_input->b_in   pointer to "b" scalar value
 * @param async
 *    see @AxonApiAxpby
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiAxpbyPointer(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiAxpby.
 * @param axon_input
 *   see @AxonApiAxpby
 * @param axon_handle
 *   see @AxonApiAxpby
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpAxpby(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * Defines an operation for AxonApiAxpbyPointer.
 * @param axon_input
 *   see @AxonApiAxpby with only the following changes:
 *   axon_input->a_in   pointer to "a" scalar value
 *   axon_input->b_in   pointer to "b" scalar value
 * @param axon_handle
 *   see @AxonApiAxpby
 * @param axon_op_handle
 *   see @AxonApiDefineOpAxpby
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpAxpbyPointer(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Axpb"=>"aX+b" Sum product of scalar "a" and X with scalar "b".
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->a_in   a scalar value
 *   axon_input->b_in   b scalar value
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->y_stride  spacing of input elements in input Y vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiAxpb(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * "AxpbPointer"=>"aX+b" Sum product of scalar "a" and X with scalar "b".
 * @param axon_handle
 *   see @AxonApiAxpb
 * @param axon_input
 *   see @AxonApiAxpb with only the following changes:
 *   axon_input->a_in   pointer to "a" scalar value
 *   axon_input->b_in   pointer to "b" scalar value
 * @param async
 *   see @AxonApiAxpb
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiAxpbPointer(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiAxpb.
 * @param axon_input
 *   see @AxonApiAxpb
 * @param axon_handle
 *   see @AxonApiAxpb
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpAxpb(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);
/**
 * Defines an operation for AxonApiAxpbPointer.
 * @param axon_input
 *   see @AxonApiAxpb with only the following changes:
 *   axon_input->a_in   pointer to "a" scalar value
 *   axon_input->b_in   pointer to "b" scalar value
 * @param axon_handle
 *   see @AxonApiAxpb
 * @param axon_op_handle
 *   see @AxonApiDefineOpAxpb
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)*/
AxonResultEnum AxonApiDefineOpAxpbPointer(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);


/**
 * "Axpby"=>"aX+bY" Sum product of scalar "a" and X with product of scalar "b" and Y.
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->y_in   Y input vector, same format as x_in.
 *   axon_input->a_in   a scalar value
 *   axon_input->b_in   b scalar value
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->y_stride  spacing of input elements in input Y vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiAxpby(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiAxpby.
 * @param axon_input
 *   see @AxonApiAxpby
 * @param axon_handle
 *   see @AxonApiAxpby
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpAxpby(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Axpb"=>"aX+b" Sum product of scalar "a" and X with scalar "b".
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->a_in   a scalar value
 *   axon_input->b_in   b scalar value
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->y_stride  spacing of input elements in input Y vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiAxpb(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiAxpb.
 * @param axon_input
 *   see @AxonApiAxpb
 * @param axon_handle
 *   see @AxonApiAxpb
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpAxpb(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Xs"=>"X Squared" Square the elements of vector X
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiXs(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiXs.
 * @param axon_input
 *   see @AxonApiXs
 * @param axon_handle
 *   see @AxonApiXs
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpXs(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Acorr"=>"Autocorrelation" calculate the autocorrelation to the delay specified in a_in
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->output_relu If true all negative values in output will be replaced with 0.
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 4, and no greater than 1024
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->a_in   Time delay scalar to calculate Autocorrelation to.
 *   axon_input->q_out  Buffer for generated output, same format as x_in, but only a_in elements long.
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiAcorr(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiAcorr.
 * @param axon_input
 *   see @AxonApiAcorr
 * @param axon_handle
 *   see @AxonApiAcorr
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 *   Note: only one autocorrelation operation can be defined at a time.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpAcorr(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "L2Norm"=>"2nd norm" calculate the 2nd Norm of a vector (sum of elements squared)
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 4, and no greater than 1024
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->q_out  Buffer to int32 for generated output
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *   axon_input->q_stride  spacing of output elements in output vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiL2norm(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiL2norm.
 * @param axon_input
 *   see @AxonApiL2norm
 * @param axon_handle
 *   see @AxonApiL2norm
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpL2norm(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Acc" => "Accumulate" sums the input vector X
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 4, and no greater than 1024
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->q_out  Buffer to int32 for generated output
 *   axon_input->x_stride  spacing of input elements in input X vector.
 *
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiAcc(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiAcc.
 * @param axon_input
 *   see @AxonApiAcc
 * @param axon_handle
 *   see @AxonApiAcc
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpAcc(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Mar" => "Multiply, Accumulate, recursive" sums the products of elements in input vector X and input vector Y
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 4, and no greater than 1024
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->y_in   Y input vector, formatted according data_packing & data_width.
 *   axon_input->q_out  Buffer to int32 for generated output
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiMar(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiAcc.
 * @param axon_input
 *   see @AxonApiAcc
 * @param axon_handle
 *   see @AxonApiAcc
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpMar(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Relu" function performs max(0,X) on input vector X
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  Number of elements in the x vector (data samples). Must be a multiple of 2, at least 2, and no greater than 512
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->q_out  Buffer for generated output, same format and size as x_in. Note: can be same location as x_in (output overwrites input)
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiRelu(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

/**
 * Defines an operation for AxonApiRelu.
 * @param axon_input
 *   see @AxonApiRelu
 * @param axon_handle
 *   see @AxonApiRelu
 * @param axon_op_handle
 *   User provides a pointer to an AxonOpHandle instance and this gets populated with the
 *   operation handle that can later be supplied to AxonApiExecuteOps()
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpRelu(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * "Matrix Multiply"  Multiplies input vector X of dimensions [1, length] with an input matrix Y[length, y_length] to
 * produce an output vector[1, y_length]
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  height(length) of x vector and width of the Y matrix.
 *                       Must be a <= 1024
 *                       multiple of 4 for 32 or unpacked data,
 *                       multiple of 8 for 16/12 packed data,
 *                       multiple of 16 for 8bit packed data
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->y_length  Number of rows in the y matrix (and length of q_out). Must be a multiple of 4, and no greater than 1024
 *   axon_input->y_in   y input matrix, organized by rows (adjacent values in a row are located in adjacent memory locations),
 *                      formatted according data_packing & data_width.
 *   axon_input->q_out  Buffer for generated output, same format as x_in, sized for y_length elements. Note: can be same location as x_in (output overwrites input)
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiMatrixMult(void *axon_handle, const AxonInputStruct *axon_input, AxonAsyncModeEnum async_mode);

AxonResultEnum AxonApiDefineOpMatrixMult(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/*
 * Identical to AxonApiDefineOpMatrixMult except output is always 32 bit, regardless of input data width.
 */
AxonResultEnum AxonApiDefineOpMatrixMult32BitOutput(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * MemCpy  Takes axon_input x_in vector and copies to q_out. This function is a simple memcpy(), but allowing the user to define
 * an op specifically for copying vectors allows the user to copy vectors from FLASH to RAM only when needed. This helps with memory management.
 * The user can optionally 0 pad the output by setting y_length to the number of elements to pad w/ 0.
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance().
 *
 * @param axon_input populated by the caller with the input and output vectors size and locations.
 *   axon_input->data_width must be <= kAxonDataWidth24 (all widths supported)
 *   axon_input->data_packing Can be True or False
 *   axon_input->output_rounding Must be <= kAxonRoundingMax
 *   axon_input->length  height(length) of x vector and width of the Y matrix.
 *                       Must be a <= 1024
 *                       multiple of 4 for 32 or unpacked data,
 *                       multiple of 8 for 16/12 packed data,
 *                       multiple of 16 for 8bit packed data
 *   axon_input->y_length number of elements to 0 pad the copy with.
 *   axon_input->x_in   X input vector, formatted according data_packing & data_width.
 *   axon_input->q_out  Buffer for generated output, same format as x_in, sized for y_length elements. Note: can be same location as x_in (output overwrites input)
 *   Remaining axon_input fields are unused.
 *
 * @param async
 *  If false, function returns after operation has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 *
 * @return kAxonResultSuccess on success or a negative error code (see AxonResultEnum)
 */
AxonResultEnum AxonApiDefineOpMemCpy(void *axon_handle, const AxonInputStruct *axon_input, AxonOpHandle *axon_op_handle);

/**
 * Executes a sequence of pre-defined axon operations in the order they appear in ops[].
 * Operations are defined by invoking AxonApiDefineOp<op_name>, which returns a handle to the operation.
 * Those handles are then inserted into an array to be executed.
 *
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance(). Note: this handle must
 *   be the same handle that was used to define the operation.
 * @param op_count Number of operations populated in ops[] to be executed.
 * @param ops[]    List of operation handles to execute.
 *
 * @param async
 *  If false, function returns after operation(s) has completed.
 *  If true, function returns immediately and AxonHostInterruptNotification will be invoked when the operation has completed.
 *   The user then invokes AxonApiGetAsyncResult to determine the outcome of the operation.
 */
AxonResultEnum AxonApiExecuteOps(void *axon_handle, uint32_t op_count, AxonOpHandle ops[], AxonAsyncModeEnum async_mode);


/**
 * Queries the outcome of the most recently completed asynchronous operation.
 * @param axon_handle
 *   Handle to a physical axon instance instantiated via AxonInitInstance(). Note: this handle must
 *   be the same handle that was used to define the operation.
 */
AxonResultEnum AxonApiGetAsyncResult(void *axon_handle);

/**
 * Frees up one or more op handles so they can be assigned to new operations.
 */
AxonResultEnum AxonApiFreeOpHandles(void *axon_handle, uint32_t op_count, AxonOpHandle ops[]);

/**
 * Adds a list of operations to the queue to be executed at the next opportunity.
 * Operates exclusively in async mode. Using the function allows new operation lists to be
 * added to the queue while others are still active.
 * In comparison, AxonApiExecuteOps() must be fully completed before it can be invoked again.
 */
AxonResultEnum AxonApiQueueOpsList(void *axon_handle, AxonMgrQueuedOpsStruct *ops_info);


/**
 * Does nothing except force the library to be included in the final elf in case the there
 * are no direct dependencies on the driver by the application (ie, dependendencies are indirect 
 * via librarries).
 * Invoking this function will simply return true.
 */
AxonResultEnum AxonNop();


