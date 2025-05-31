#ifndef _SUSI4_H_
#define _SUSI4_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once

#ifndef SUSI_API
	#define SUSI_API __stdcall
#endif
#else
	#define SUSI_API
#endif

#ifdef __cplusplus
extern "C" {
#endif


//-----------------------------------------------------------------------------
//
//	Status codes
//
//-----------------------------------------------------------------------------
/* Description
 *   The SUSI API library is not yet or unsuccessfully initialized. 
 *   SusiLibInitialize needs to be called prior to the first access of any 
 *   other SUSI API function.
 * Actions
 *   Call SusiLibInitialize..
 */
#define SUSI_STATUS_NOT_INITIALIZED				0xFFFFFFFF

/* Description
 *   Library is initialized.
 * Actions
 *   none.
 */
#define SUSI_STATUS_INITIALIZED					0xFFFFFFFE

/* Description
 *   Memory Allocation Error.
 * Actions
 *   Free memory and try again..
 */
#define SUSI_STATUS_ALLOC_ERROR					0xFFFFFFFD

/* Description 
 *   Time out in driver. This is Normally caused by hardware/software 
 *   semaphore timeout. 
 * Actions
 *   Retry.
 */
#define SUSI_STATUS_DRIVER_TIMEOUT				0xFFFFFFFC

/* Description 
 *  One or more of the SUSI API function call parameters are out of the 
 *  defined range.
 * Actions
 *   Verify Function Parameters.
 */
#define SUSI_STATUS_INVALID_PARAMETER			0xFFFFFEFF

/* Description
 *   The Block Alignment is incorrect.
 * Actions
 *   Use Inputs and Outputs to correctly select input and outputs. 
 */
#define SUSI_STATUS_INVALID_BLOCK_ALIGNMENT		0xFFFFFEFE

/* Description
 *   This means that the Block length is too long.
 * Actions
 *   Use Alignment Capabilities information to correctly align write access.
 */
#define SUSI_STATUS_INVALID_BLOCK_LENGTH		0xFFFFFEFD

/* Description
 *   The current Direction Argument attempts to set GPIOs to a unsupported 
 *   directions. I.E. Setting GPI to Output.
 * Actions
 *   Use Inputs and Outputs to correctly select input and outputs. 
 */
#define SUSI_STATUS_INVALID_DIRECTION			0xFFFFFEFC

/* Description
 *   The Bitmask Selects bits/GPIOs which are not supported for the current ID.
 * Actions
 *   Use Inputs and Outputs to probe supported bits..
 */
#define SUSI_STATUS_INVALID_BITMASK				0xFFFFFEFB

/* Description
 *   Watchdog timer already started.
 * Actions
 *   Call SusiWDogStop, before retrying.
 */
#define SUSI_STATUS_RUNNING						0xFFFFFEFA

/* Description
 *   This function or ID is not supported at the actual hardware environment.
 * Actions
 *   none.
 */
#define SUSI_STATUS_UNSUPPORTED					0xFFFFFCFF

/* Description
 *   Selected device was not found
 * Actions
 *   none.
 */
#define SUSI_STATUS_NOT_FOUND					0xFFFFFBFF

/* Description
 *    Device has no response.
 * Actions
 *   none.
 */
#define SUSI_STATUS_TIMEOUT						0xFFFFFBFE

/* Description
 *   The selected device or ID is busy or a data collision was detected.
 * Actions
 *   Retry.
 */
#define SUSI_STATUS_BUSY_COLLISION				0xFFFFFBFD

/* Description
 *   An error was detected during a read operation.
 * Actions
 *   Retry.
 */
#define SUSI_STATUS_READ_ERROR					0xFFFFFAFF

/* Description
 *   An error was detected during a write operation.
 * Actions
 *   Retry.
 */
#define SUSI_STATUS_WRITE_ERROR					0xFFFFFAFE


/* Description
 *   The amount of available data exceeds the buffer size. Storage buffer 
 *   overflow was prevented. Read count was larger than the defined buffer
 *   length.
 * Actions
 *   Either increase the buffer size or reduce the block length.
 */
#define SUSI_STATUS_MORE_DATA					0xFFFFF9FF

/* Description
 *   Generic error message. No further error details are available.
 * Actions
 *   none.
 */
#define SUSI_STATUS_ERROR						0xFFFFF0FF

/* Description
 *   The operation was successful.
 * Actions
 *   none.
 */
#define SUSI_STATUS_SUCCESS						0


//-----------------------------------------------------------------------------
//
//	APIs
//
//-----------------------------------------------------------------------------
//=============================================================================
// Library
//=============================================================================
// Should be called before calling any other API function is called.
//
// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Already initialized				| SUSI_STATUS_INITIALIZED
// Else										| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiLibInitialize(void);

// Should be called before program exit 
//
// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// Else										| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiLibUninitialize(void);

//=============================================================================
// Board
//=============================================================================
// Board information values
#define SUSI_ID_GET_INDEX(Id)					(Id & 0xFF)
#define SUSI_ID_GET_TYPE(Id)					(Id & 0x000FF000)

#define SUSI_ID_BOARD_INFO_BASE					0x00000000
#define SUSI_ID_GET_SPEC_VERSION				0x00000000
#define SUSI_ID_BOARD_BOOT_COUNTER_VAL			0x00000001		// Units: Boot count
#define SUSI_ID_BOARD_RUNNING_TIME_METER_VAL	0x00000002		// Units = Minutes
#define SUSI_ID_BOARD_PNPID_VAL					0x00000003
#define SUSI_ID_BOARD_PLATFORM_REV_VAL			0x00000004

#define SUSI_ID_BOARD_VER_BASE					0x00010000
#define SUSI_ID_BOARD_DRIVER_VERSION_VAL		0x00010000
#define SUSI_ID_BOARD_LIB_VERSION_VAL			0x00010001
#define SUSI_ID_BOARD_FIRMWARE_VERSION_VAL		0x00010002

#define SUSI_ID_HWM_TEMP_MAX					10									// Maximum temperature item number
#define SUSI_ID_HWM_TEMP_BASE					0x00020000
#define SUSI_ID_HWM_TEMP_CPU					(SUSI_ID_HWM_TEMP_BASE + 0)			// 0.1 Kelvins
#define SUSI_ID_HWM_TEMP_CHIPSET				(SUSI_ID_HWM_TEMP_BASE + 1)			// 0.1 Kelvins
#define SUSI_ID_HWM_TEMP_SYSTEM					(SUSI_ID_HWM_TEMP_BASE + 2)			// 0.1 Kelvins
#define SUSI_ID_HWM_TEMP_CPU2					(SUSI_ID_HWM_TEMP_BASE + 3)			// 0.1 Kelvins
#define SUSI_ID_HWM_TEMP_OEM0					(SUSI_ID_HWM_TEMP_BASE + 4)			// 0.1 Kelvins
#define SUSI_ID_HWM_TEMP_OEM1					(SUSI_ID_HWM_TEMP_BASE + 5)			// 0.1 Kelvins
#define SUSI_ID_HWM_TEMP_OEM2					(SUSI_ID_HWM_TEMP_BASE + 6)			// 0.1 Kelvins
#define SUSI_ID_HWM_TEMP_OEM3					(SUSI_ID_HWM_TEMP_BASE + 7)			// 0.1 Kelvins
#define SUSI_ID_HWM_TEMP_OEM4					(SUSI_ID_HWM_TEMP_BASE + 8)			// 0.1 Kelvins
#define SUSI_ID_HWM_TEMP_OEM5					(SUSI_ID_HWM_TEMP_BASE + 9)			// 0.1 Kelvins

#define SUSI_ID_HWM_VOLTAGE_MAX					20									// Maximum voltage item number
#define SUSI_ID_HWM_VOLTAGE_BASE				0x00021000
#define SUSI_ID_HWM_VOLTAGE_VCORE				(SUSI_ID_HWM_VOLTAGE_BASE + 0)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_VCORE2				(SUSI_ID_HWM_VOLTAGE_BASE + 1)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_2V5					(SUSI_ID_HWM_VOLTAGE_BASE + 2)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_3V3					(SUSI_ID_HWM_VOLTAGE_BASE + 3)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_5V					(SUSI_ID_HWM_VOLTAGE_BASE + 4)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_12V					(SUSI_ID_HWM_VOLTAGE_BASE + 5)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_5VSB				(SUSI_ID_HWM_VOLTAGE_BASE + 6)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_3VSB				(SUSI_ID_HWM_VOLTAGE_BASE + 7)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_VBAT				(SUSI_ID_HWM_VOLTAGE_BASE + 8)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_5NV					(SUSI_ID_HWM_VOLTAGE_BASE + 9)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_12NV				(SUSI_ID_HWM_VOLTAGE_BASE + 10)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_VTT					(SUSI_ID_HWM_VOLTAGE_BASE + 11)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_24V					(SUSI_ID_HWM_VOLTAGE_BASE + 12)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_OEM0				(SUSI_ID_HWM_VOLTAGE_BASE + 16)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_OEM1				(SUSI_ID_HWM_VOLTAGE_BASE + 17)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_OEM2				(SUSI_ID_HWM_VOLTAGE_BASE + 18)		// millivolts
#define SUSI_ID_HWM_VOLTAGE_OEM3				(SUSI_ID_HWM_VOLTAGE_BASE + 19)		// millivolts

#define SUSI_ID_HWM_FAN_MAX						10									// Maximum fan item number
#define SUSI_ID_HWM_FAN_BASE					0x00022000
#define SUSI_ID_HWM_FAN_CPU						(SUSI_ID_HWM_FAN_BASE + 0)			// RPM
#define SUSI_ID_HWM_FAN_SYSTEM					(SUSI_ID_HWM_FAN_BASE + 1)			// RPM
#define SUSI_ID_HWM_FAN_CPU2					(SUSI_ID_HWM_FAN_BASE + 2)			// RPM
#define SUSI_ID_HWM_FAN_OEM0					(SUSI_ID_HWM_FAN_BASE + 3)			// RPM
#define SUSI_ID_HWM_FAN_OEM1					(SUSI_ID_HWM_FAN_BASE + 4)			// RPM
#define SUSI_ID_HWM_FAN_OEM2					(SUSI_ID_HWM_FAN_BASE + 5)			// RPM
#define SUSI_ID_HWM_FAN_OEM3					(SUSI_ID_HWM_FAN_BASE + 6)			// RPM
#define SUSI_ID_HWM_FAN_OEM4					(SUSI_ID_HWM_FAN_BASE + 7)			// RPM
#define SUSI_ID_HWM_FAN_OEM5					(SUSI_ID_HWM_FAN_BASE + 8)			// RPM
#define SUSI_ID_HWM_FAN_OEM6					(SUSI_ID_HWM_FAN_BASE + 9)			// RPM

#define SUSI_ID_SUPPORTED_BASE					0x00030000
#define SUSI_ID_SMBUS_SUPPORTED					0x00030000							// Get Support information
#define SUSI_SMBUS_EXTERNAL_SUPPORTED			(1 << 0)
#define SUSI_SMBUS_OEM0_SUPPORTED				(1 << 1)
#define SUSI_SMBUS_OEM1_SUPPORTED				(1 << 2)
#define SUSI_SMBUS_OEM2_SUPPORTED				(1 << 3)
#define SUSI_SMBUS_OEM3_SUPPORTED				(1 << 4)
#define SUSI_ID_I2C_SUPPORTED					0x00030100							// Get Support information
#define SUSI_I2C_EXTERNAL_SUPPORTED				(1 << 0)
#define SUSI_I2C_OEM0_SUPPORTED					(1 << 1)
#define SUSI_I2C_OEM1_SUPPORTED					(1 << 2)
#define SUSI_I2C_OEM2_SUPPORTED					(1 << 3)

#define SUSI_KELVINS_OFFSET						2731
#define SUSI_ENCODE_CELCIUS(Celsius)			(((Celsius) * 10) + SUSI_KELVINS_OFFSET)
#define SUSI_DECODE_CELCIUS(Celsius)			(((Celsius) - SUSI_KELVINS_OFFSET) / 10)

// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// pValue==NULL								| SUSI_STATUS_INVALID_PARAMETER
// Unknown ID								| SUSI_STATUS_UNSUPPORTED
// Else										| SUSI_STATUS_SUCCESS		
uint32_t SUSI_API SusiBoardGetValue(
    uint32_t Id,		// IN	Value Id
    uint32_t *pValue    // OUT	Return Value
    );

// Board information string
#define SUSI_ID_BOARD_MANUFACTURER_STR			0
#define SUSI_ID_BOARD_NAME_STR					1
#define SUSI_ID_BOARD_REVISION_STR				2
#define SUSI_ID_BOARD_SERIAL_STR				3
#define SUSI_ID_BOARD_BIOS_REVISION_STR			4
#define SUSI_ID_BOARD_HW_REVISION_STR			5
#define SUSI_ID_BOARD_PLATFORM_TYPE_STR			6

// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// pBufLen==NULL							| SUSI_STATUS_INVALID_PARAMETER
// pBufLen!=NULL&&*pBufLen&&pBuffer==NULL	| SUSI_STATUS_INVALID_PARAMETER
// Unknown ID								| SUSI_STATUS_UNSUPPORTED
// strlength + 1 > *pBufLen					| SUSI_STATUS_MORE_DATA
// Else										| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiBoardGetStringA(
	uint32_t Id,		// IN		Name Id
	char *pBuffer,		// OUT		Destination pBuffer
	uint32_t *pBufLen	// INOUT	pBuffer Length
    );

// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// pBufLen == NULL							| SUSI_STATUS_INVALID_PARAMETER
// Length != 1, 2 or 4						| SUSI_STATUS_INVALID_PARAMETER
// Else										| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiBoardReadIO(
	uint16_t Port,		// IN	IO port address
	uint32_t *pValue,	// OUT	Value point
	uint32_t Length		// IN	Value length, should be 1, 2 or 4
	);

uint32_t SUSI_API SusiBoardWriteIO(
	uint16_t Port,		// IN	IO port address
	uint32_t Value,		// IN	Value
	uint32_t Length		// IN	Value length, should be 1, 2 or 4
	);

uint32_t SUSI_API SusiBoardReadPCI(
	uint8_t Bus,		// IN	PCI Bus number
	uint8_t Device,		// IN	PCI Device number
	uint8_t Function,	// IN	PCI Function number
	uint32_t Offset,	// IN	Configuration space offset
	uint8_t *pData,		// OUT	Data point
	uint32_t Length		// IN	Data length
	);

uint32_t SUSI_API SusiBoardWritePCI(
	uint8_t Bus,		// IN	PCI Bus number
	uint8_t Device,		// IN	PCI Device number
	uint8_t Function,	// IN	PCI Function number
	uint32_t Offset,	// IN	Configuration space offset
	uint8_t *pData,		// IN	Data point
	uint32_t Length		// IN	Data length
	);

uint32_t SUSI_API SusiBoardReadMemory(
	uint32_t Address,	// IN	Memory address
	uint8_t *pData,		// OUT	Data point
	uint32_t Length		// IN	Data length
	);

uint32_t SUSI_API SusiBoardWriteMemory(
	uint32_t Address,	// IN	Memory address
	uint8_t *pData,		// IN	Data point
	uint32_t Length		// IN	Data length
	);

uint32_t SUSI_API SusiBoardReadMSR(
	uint32_t index,	// IN	CPU MSR index
	uint32_t *EAX,	// OUT	CPU register EAX
	uint32_t *EDX	// OUT	CPU register EFX
	);

uint32_t SUSI_API SusiBoardWriteMSR(
	uint32_t index,	// IN	CPU MSR index
	uint32_t EAX,	// IN	CPU register EAX
	uint32_t EDX	// IN	CPU register EDX
	);


//=============================================================================
// SMBus
//=============================================================================
#define SUSI_SMBUS_MAX_DEVICE			5
#define SUSI_ID_SMBUS_EXTERNAL			0	// Baseboard SMBus Interface
#define SUSI_ID_SMBUS_OEM0				1
#define SUSI_ID_SMBUS_OEM1				2
#define SUSI_ID_SMBUS_OEM2				3
#define SUSI_ID_SMBUS_OEM3				4

// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// pBufLen == NULL							| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id								| SUSI_STATUS_UNSUPPORTED
// Bus Busy  SDA/SDC low					| SUSI_STATUS_BUSY_COLLISION
// Arbitration Error/Collision Error		| SUSI_STATUS_BUSY_COLLISION
//	On Write 1 write cycle					|
//	SDA Remains low							|
// Time-out due to clock stretching			| SUSI_STATUS_TIMEOUT
// start<Addr Byte 1><R>Nak					| SUSI_STATUS_NOT_FOUND
// start<Addr Byte 1><W>Ack<CMD Byte 1>Nak  | SUSI_STATUS_WRITE_ERROR
// Else										| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiSMBReadByte(
	uint32_t Id,		// IN	Bus Id
	uint8_t Addr,		// IN	Encoded 7-Bit device address
	uint8_t Cmd,		// IN	Offset / Command
	uint8_t *pBuffer	// OUT	Transfer Byte Data pBuffer
	);

// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// pBufLen == NULL							| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id								| SUSI_STATUS_UNSUPPORTED
// Bus Busy  SDA/SDC low					| SUSI_STATUS_BUSY_COLLISION
// Arbitration Error/Collision Error		| SUSI_STATUS_BUSY_COLLISION
//	On Write 1 write cycle					|
//	SDA Remains low							|
// Time-out due to clock stretching			| SUSI_STATUS_TIMEOUT
// start<Addr Byte 1><W>Nak					| SUSI_STATUS_NOT_FOUND
// start<Addr Byte 1><W>Ack<CMD Byte 1>Nak  | SUSI_STATUS_WRITE_ERROR
// Else										| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiSMBWriteByte(
	uint32_t Id,		// IN	Bus Id
	uint8_t Addr,		// IN	Encoded 7-Bit device address
	uint8_t Cmd,		// IN	Offset / Command
	uint8_t Data		// IN	Transfer Byte Data
	);

uint32_t SUSI_API SusiSMBReadWord(
	uint32_t Id,		// IN	
	uint8_t Addr,		// IN	
	uint8_t Cmd,		// IN	
	uint16_t *pBuffer	// OUT	
	);

uint32_t SUSI_API SusiSMBWriteWord(
	uint32_t Id,		// IN
	uint8_t Addr,		// IN
	uint8_t Cmd,		// IN
	uint16_t Data		// IN
	);

uint32_t SUSI_API SusiSMBReceiveByte(
	uint32_t Id,		// IN
	uint8_t Addr,		// IN
	uint8_t *pData		// OUT
	);

uint32_t SUSI_API SusiSMBSendByte(
	uint32_t Id,		// IN
	uint8_t Addr,		// IN
	uint8_t Data		// IN
	);

uint32_t SUSI_API SusiSMBReadQuick(
	uint32_t Id,		// IN
	uint8_t Addr		// IN
	);

uint32_t SUSI_API SusiSMBWriteQuick(
	uint32_t Id,		// IN
	uint8_t Addr		// IN
	);

uint32_t SUSI_API SusiSMBReadBlock(
	uint32_t Id,		// IN
	uint8_t Addr,		// IN
	uint8_t Cmd,		// IN
	uint8_t *pBuffer,	// OUT
	uint32_t *pLength	// INOUT
	);

uint32_t SUSI_API SusiSMBWriteBlock(
	uint32_t Id,		// IN
	uint8_t Addr,		// IN
	uint8_t Cmd,		// IN
	uint8_t *pBuffer,	// IN
	uint32_t Length		// IN
	);

uint32_t SUSI_API SusiSMBI2CReadBlock(
	uint32_t Id,		// IN
	uint8_t Addr,		// IN
	uint8_t Cmd,		// IN
	uint8_t *pBuffer,	// OUT
	uint32_t Length		// IN
	);

uint32_t SUSI_API SusiSMBI2CWriteBlock(
	uint32_t Id,		// IN
	uint8_t Addr,		// IN
	uint8_t Cmd,		// IN
	uint8_t *pBuffer,	// IN
	uint32_t Length		// IN
	);

//=============================================================================
// I2C
//=============================================================================
#define SUSI_I2C_MAX_DEVICE			5
#define SUSI_ID_I2C_EXTERNAL		0		//Baseboard I2C Interface
#define SUSI_ID_I2C_OEM0			1
#define SUSI_ID_I2C_OEM1			2
#define SUSI_ID_I2C_OEM2			3

// I2C Address Format 
//
// L = Set to 0   
// H = Set to 1   
// X = Don't Care(Direction Bit)
// 0-F Address Bit 
//
// Bits 31-16 are Reserved and should be set to 0
//
// Bit Offset      | F E D C B A 9 8 7 6 5 4 3 2 1 0
// ----------------+--------------------------------
// 7  Bit Address  | L L L L L L L L 6 5 4 3 2 1 0 X
// 10 Bit Address  | H H H H L 9 8 X 7 6 5 4 3 2 1 0
//
// Examples where Don't Care bits set to 0
//				Encoded	Encoded
//  Address		7Bit    10Bit
//  0x01		0x02	0xF001
//  0x58		0xA0	0xF058
//  0x59		0xA2	0xF059
//  0x77		0xEE	0xF077
//  0x3FF				0xF6FF
//
#define SUSI_I2C_ENC_7BIT_ADDR(x)		(((x) & 0x07F) << 1)
#define SUSI_I2C_DEC_7BIT_ADDR(x)		(((x) >> 1) & 0x07F)
#define SUSI_I2C_ENC_10BIT_ADDR(x)		(((x) & 0xFF) | (((x) & 0x0300) << 1) | 0xF000)
#define SUSI_I2C_DEC_10BIT_ADDR(x)		(((x) & 0xFF) | (((x) >> 1) & 0x300))
#define SUSI_I2C_IS_10BIT_ADDR(x)		(((x) & 0xF800) == 0xF000)
#define SUSI_I2C_IS_7BIT_ADDR(x)		(!SUSI_I2C_IS_10BIT_ADDR(x))


// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// (WriteLen>1)&&(pWBuffer==NULL)			| SUSI_STATUS_INVALID_PARAMETER
// (RBufLen>1)&&(pRBuffer==NULL) 			| SUSI_STATUS_INVALID_PARAMETER
// ((WriteLen==0)&&(RBufLen==0))			| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id								| SUSI_STATUS_UNSUPPORTED
// Bus Busy  SDA/SDC low					| SUSI_STATUS_BUSY_COLLISION
// Arbitration Error/Collision Error		| SUSI_STATUS_BUSY_COLLISION
//	On Write 1 write cycle					|
//	SDA Remains low							|
// Time-out due to clock stretching			| SUSI_STATUS_TIMEOUT
// start<Addr Byte 1><W>Nak					| SUSI_STATUS_NOT_FOUND
// start<Addr Byte 1><R>Nak					| SUSI_STATUS_NOT_FOUND
// start<Addr Byte 1><W>Ack<CMD Byte 1>Nak  | SUSI_STATUS_WRITE_ERROR
// Else										| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiI2CWriteReadCombine(
	uint32_t Id,		// IN		I2C Bus Id
	uint8_t Addr,		// IN		Encoded 7Bit I2C Device Address 
	uint8_t *pWBuffer,	// INOPT	Write Data pBuffer
	uint32_t WriteLen,	// IN		Number of Bytes to write
	uint8_t *pRBuffer,	// OUTOPT	Read Data pBuffer
	uint32_t ReadLen	// IN		Number of Bytes to Read
    );

#define SusiI2CWrite(Id, Addr, pBuffer, ByteCnt) \
          SusiI2CWriteReadCombine(Id, Addr, pBuffer, ByteCnt, NULL, 0)
#define SusiI2CRead(Id, Addr, pBuffer, ByteCnt) \
          SusiI2CWriteReadCombine(Id, Addr, NULL, 0, pBuffer, ByteCnt)

// Bits 31 & 30 Selects Command Type
#define SUSI_I2C_STD_CMD				(uint32_t)(0 << 30)
#define SUSI_I2C_EXT_CMD				(uint32_t)(2 << 30)
#define SUSI_I2C_NO_CMD					(uint32_t)(1 << 30)    
#define SUSI_I2C_CMD_TYPE_MASK			(uint32_t)(3 << 30)    

#define SUSI_I2C_ENC_STD_CMD(x)			(((x) & 0xFF) | SUSI_I2C_STD_CMD)
#define SUSI_I2C_ENC_EXT_CMD(x)			(((x) & 0xFFFF) | SUSI_I2C_EXT_CMD)
#define SUSI_I2C_IS_EXT_CMD(x)			(((x) & SUSI_I2C_CMD_TYPE_MASK) == SUSI_I2C_EXT_CMD)
#define SUSI_I2C_IS_STD_CMD(x)			(((x) & SUSI_I2C_CMD_TYPE_MASK) == SUSI_I2C_STD_CMD)
#define SUSI_I2C_IS_NO_CMD(x)			(((x) & SUSI_I2C_CMD_TYPE_MASK) == SUSI_I2C_NO_CMD)

// Addr Byte 1 Below Designates Addr MSB in a 10bit address transfer and 
// the complete address in an 7bit address transfer.
//
// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// pBuffer==NULL							| SUSI_STATUS_INVALID_PARAMETER
// ReadLen==0					 			| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id								| SUSI_STATUS_UNSUPPORTED
// Bus Busy  SDA/SDC low					| SUSI_STATUS_BUSY_COLLISION
// Arbitration Error/Collision Error		| SUSI_STATUS_BUSY_COLLISION
//	On Write 1 write cycle					|
//	SDA Remains low							|
// Time-out due to clock stretching			| SUSI_STATUS_TIMEOUT
// start<Addr Byte 1><R>Nak					| SUSI_STATUS_NOT_FOUND
// start<Addr Byte 1><W>Ack<Addr Byte 2>Nak | SUSI_STATUS_WRITE_ERROR or
//											| SUSI_STATUS_NOT_FOUND
// start<Addr Byte 1><W>Ack<CMD Byte 1>Nak  | SUSI_STATUS_WRITE_ERROR
// start<Addr Byte 1><W>Ack<Data Byte 1>Nak | SUSI_STATUS_WRITE_ERROR
// Else										| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiI2CReadTransfer(
	uint32_t Id,		// IN	I2C Bus Id
	uint32_t Addr,		// IN	Encoded 7/10Bit I2C Device Address
	uint32_t Cmd,		// IN	I2C Command / Offset
	uint8_t *pBuffer,	// OUT	Transfer Data pBuffer 
	uint32_t ReadLen	// IN	Byte Count to read
    );

// Addr Byte 1 Below Designates Addr MSB in a 10bit address transfer and 
// the complete address in an 7bit address transfer.
//
// Condition								| Return Values 
// -----------------------------------------+------------------------------
// Library Uninitialized					| SUSI_STATUS_NOT_INITIALIZED
// pBuffer==NULL							| SUSI_STATUS_INVALID_PARAMETER
// ByteCnt==0					 			| SUSI_STATUS_INVALID_PARAMETER
// ByteCnt > MaxLength			            | SUSI_STATUS_INVALID_BLOCK_LENGTH
// Unknown Id								| SUSI_STATUS_UNSUPPORTED
// Bus Busy  SDA/SDC low					| SUSI_STATUS_BUSY_COLLISION
// Arbitration Error/Collision Error		| SUSI_STATUS_BUSY_COLLISION
//	On Write 1 write cycle					|
//	SDA Remains low							|
// Time-out due to clock stretching			| SUSI_STATUS_TIMEOUT
// start<Addr Byte 1><W>Nak					| SUSI_STATUS_NOT_FOUND
// start<Addr Byte 1><W>Ack<Addr Byte 2>Nak | SUSI_STATUS_WRITE_ERROR or
//											| SUSI_STATUS_NOT_FOUND
// start<Addr Byte 1><W>Ack<CMD Byte 1>Nak  | SUSI_STATUS_WRITE_ERROR
// start<Addr Byte 1><W>Ack<Data Byte 1>Nak | SUSI_STATUS_WRITE_ERROR
// Else										| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiI2CWriteTransfer(
	uint32_t Id,		// IN	I2C Bus Id
	uint32_t Addr,		// IN	Encoded 7/10Bit I2C Device Address
	uint32_t Cmd,		// IN	I2C Command / Offset
	uint8_t *pBuffer,	// IN	Transfer Data pBuffer 
	uint32_t ByteCnt	// IN	Byte Count to write
    );


// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Bus Busy  SDA/SDC low						| SUSI_STATUS_BUSY_COLLISION
// Arbitration Error/Collision Error			| SUSI_STATUS_BUSY_COLLISION
//	On Write 1 write cycle						|
//	SDA Remains low								|
// Time-out due to clock stretching				| SUSI_STATUS_TIMEOUT
//												|
// 7Bit Address									|
// start<Addr Byte><W>Nak						| SUSI_STATUS_NOT_FOUND
//												|
// 10Bit Address								|
// start<Addr Byte MSB><W>Nak					| SUSI_STATUS_NOT_FOUND
// start<Addr Byte MSB><W>Ack<Addr Byte LSB>Nak | SUSI_STATUS_NOT_FOUND
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiI2CProbeDevice(
	uint32_t Id,		// IN	I2C Bus Id
	uint32_t Addr		// IN	Encoded 7/10Bit I2C Device Address
    );

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// pFreq == NULL								| SUSI_STATUS_INVALID_PARAMETER
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiI2CGetFrequency(
	uint32_t Id,		// IN	I2C Bus Id
	uint32_t *pFreq		// OUT	I2C Bus Frequency (KHz)
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Freq > 100 && Freq != 400					| SUSI_STATUS_INVALID_PARAMETER
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiI2CSetFrequency(
	uint32_t Id,		// IN	I2C Bus Id
	uint32_t Freq		// IN	I2C Bus Frequency (KHz)
	);

// I2C value item IDs
#define SUSI_ID_I2C_MAXIMUM_BLOCK_LENGTH	0x00000000

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Unknown Item Id								| SUSI_STATUS_UNSUPPORTED
// pValue == NULL								| SUSI_STATUS_INVALID_PARAMETER
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiI2CGetCaps(
	uint32_t Id,		// IN	I2C Bus Id
	uint32_t ItemId,	// IN	I2C Item Id
	uint32_t *pValue	// OUT	I2C Item value
	);


//=============================================================================
// GPIO -  32 GPIOs Per Bank
//=============================================================================
// Single GPIO ID Mapping
#define SUSI_ID_GPIO_BASE					0
#define SUSI_ID_GPIO(GPIO_NUM)				(SUSI_ID_GPIO_BASE + GPIO_NUM)

// Multiple GPIOs ID Mapping
#define SUSI_ID_GPIO_BANK_BASE				0x00010000
#define SUSI_ID_GPIO_BANK(BANK_NUM)			(SUSI_ID_GPIO_BANK_BASE + BANK_NUM)
#define SUSI_ID_GPIO_PIN_BANK(GPIO_NUM)		(SUSI_ID_GPIO_BANK_BASE | ((GPIO_NUM) >> 5))
#define SUSI_GPIO_PIN_BANK_MASK(GPIO_NUM)	((1 << ((GPIO_NUM) & 0x1F))

// Levels
#define SUSI_GPIO_LOW     0
#define SUSI_GPIO_HIGH    1

// Directions
#define SUSI_GPIO_INPUT   1
#define SUSI_GPIO_OUTPUT  0

// Item ID
#define SUSI_ID_GPIO_INPUT_SUPPORT			0x00000000
#define SUSI_ID_GPIO_OUTPUT_SUPPORT			0x00000001

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pValue==NULL)								| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiGPIOGetCaps(
	uint32_t Id,		// IN		GPIO ID or Bank ID
	uint32_t ItemId,	// IN		Item ID
	uint32_t *pValue	// OUT		Item value
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Bitmask==0 when Bank mode					| SUSI_STATUS_INVALID_PARAMETER
// pDirection==NULL								| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// (Bitmask&~(Inputs|Outputs))					| SUSI_STATUS_INVALID_BITMASK
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiGPIOGetDirection(
	uint32_t Id,			// IN		GPIO ID or Bank ID
	uint32_t Bitmask,		// INOPT	Bit mask of Affected Bits
	uint32_t *pDirection	// OUT		Current Direction
    );

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Bitmask==0 when Bank mode					| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// (Bitmask&~(Inputs|Outputs))					| SUSI_STATUS_INVALID_BITMASK
// (Bitmask&Direction)&Inputs					| SUSI_STATUS_INVALID_DIRECTION
// (Bitmask&Direction)&Outputs					| SUSI_STATUS_INVALID_DIRECTION
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiGPIOSetDirection(
	uint32_t Id,			// IN		GPIO ID or Bank ID
	uint32_t Bitmask,		// INOPT	Bit mask of Affected Bits
	uint32_t Direction		// IN		Direction
    );

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Bitmask==0 when Bank mode					| SUSI_STATUS_INVALID_PARAMETER
// pLevel==NULL									| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// (Bitmask&~(Inputs|Outputs))					| SUSI_STATUS_INVALID_BITMASK
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiGPIOGetLevel(
	uint32_t Id,			// IN		GPIO ID or Bank ID
	uint32_t Bitmask,		// INOPT	Bit mask of Affected Bits
	uint32_t *pLevel		// OUT		Current Level
    );

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Bitmask==0 when Bank mode					| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiGPIOSetLevel(
	uint32_t Id,			// IN		GPIO ID or Bank ID
	uint32_t Bitmask,		// INOPT	Bit mask of Affected Bits
	uint32_t Level			// IN		Level
    );

//=============================================================================
// VGA
//=============================================================================
#define SUSI_ID_BACKLIGHT_MAX			3
#define SUSI_ID_BACKLIGHT_1				0
#define SUSI_ID_BACKLIGHT_2				1
#define SUSI_ID_BACKLIGHT_3				2

#define SUSI_BACKLIGHT_SET_ON			1
#define SUSI_BACKLIGHT_SET_OFF			0

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pEnable==NULL								| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaGetBacklightEnable( 
	uint32_t Id,		// IN	Backlight Id
	uint32_t *pEnable	// OUT	Backlight enable status
						//		0 = Disable, others = Enable
    );

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaSetBacklightEnable(
	uint32_t Id,		// IN	Backlight Id
	uint32_t Enable		// IN	Control backlight enable
						//		0 = Disable, others = Enable
    );

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// pBright==NULL								| SUSI_STATUS_INVALID_PARAMETER
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaGetBacklightBrightness( 
	uint32_t Id,		// IN	Backlight Id
	uint32_t *pBright   // OUT	Backlight Brightness
    );

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Bright > MAX value							| SUSI_STATUS_INVALID_PARAMETER
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaSetBacklightBrightness(
	uint32_t Id,		// IN	Backlight Id
	uint32_t Bright		// IN	Backlight Brightness
    );

#define SUSI_BACKLIGHT_LEVEL_MAXIMUM			9
#define SUSI_BACKLIGHT_LEVEL_MINIMUM			0

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// pBright==NULL								| SUSI_STATUS_INVALID_PARAMETER
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaGetBacklightLevel(
	uint32_t Id,
	uint32_t *pLevel
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// pBright==NULL								| SUSI_STATUS_INVALID_PARAMETER
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaSetBacklightLevel(
	uint32_t Id,
	uint32_t Level
	);

#define SUSI_BACKLIGHT_POLARITY_ON		1
#define SUSI_BACKLIGHT_POLARITY_OFF		0
// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pPolarity==NULL								| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaGetPolarity( 
	uint32_t Id,		// IN	Backlight Id
	uint32_t *pPolarity	// OUT	Backlight PWM invert
						//		0 = Non-invert, 1 = Invert
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaSetPolarity( 
	uint32_t Id,		// IN	Backlight Id
	uint32_t Polarity	// IN	Control backlight PWM invert
						//		0 = Non-invert, 1 = Invert
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pFrequency==NULL								| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaGetFrequency( 
	uint32_t Id,			// IN	Backlight Id
	uint32_t *pFrequency	// OUT	Backlight PWM frequency
	);


// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaSetFrequency( 
	uint32_t Id,			// IN	Backlight Id
	uint32_t Frequency		// IN	Control backlight PWM frequency
	);

// VGA value item IDs
#define SUSI_ID_VGA_BRIGHTNESS_MAXIMUM		0x00010000
#define SUSI_ID_VGA_BRIGHTNESS_MINIMUM		0x00010001

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pValue==NULL									| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiVgaGetCaps( 
	uint32_t Id,		// IN	Backlight Id
	uint32_t ItemId,	// IN	Value item Id
	uint32_t *pValue	// OUT	Return Value
	);


//=============================================================================
// Storage
//=============================================================================
#define SUSI_ID_STORAGE_STD				0x00000000
#define SUSI_ID_STORAGE_OEM0			0x00000001
#define SUSI_ID_STORAGE_OEM1			0x00000002

// Storage value item IDs
#define SUSI_ID_STORAGE_TOTAL_SIZE		0x00000000
#define SUSI_ID_STORAGE_BLOCK_SIZE		0x00000001
#define SUSI_ID_STORAGE_LOCK_STATUS		0x00010000
#define SUSI_ID_STORAGE_PSW_MAX_LEN		0x00010001

#define SUSI_STORAGE_STATUS_LOCK		1
#define SUSI_STORAGE_STATUS_UNLOCK		0
// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pValue==NULL									| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiStorageGetCaps(
	uint32_t Id,		// IN	Storage Id
	uint32_t ItemId,	// IN	Value item Id
	uint32_t *pValue	// OUT	Return Value
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pBuffer==NULL								| SUSI_STATUS_INVALID_PARAMETER
// BufLen==0									| SUSI_STATUS_INVALID_PARAMETER
// Offset+BufLen>TotalSize						| SUSI_STATUS_INVALID_BLOCK_LENGTH
// Read error									| SUSI_STATUS_READ_ERROR
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiStorageAreaRead(
	uint32_t Id,		// IN	Storage Area Id
	uint32_t Offset,	// IN	Byte Offset
	uint8_t *pBuffer, 	// OUT	Pointer to Data pBuffer
	uint32_t BufLen  	// IN	Read Data count 
);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pBuffer==NULL								| SUSI_STATUS_INVALID_PARAMETER
// BufLen==0									| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiStorageAreaWrite(
	uint32_t Id,		// IN	Storage Area Id
	uint32_t Offset,	// IN	Byte Offset
	uint8_t *pBuffer, 	// IN	Pointer to Data pBuffer
	uint32_t BufLen  	// IN	Write Data count 
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pBuffer==NULL								| SUSI_STATUS_INVALID_PARAMETER
// BufLen==0									| SUSI_STATUS_INVALID_PARAMETER
// Unlock error									| SUSI_STATUS_WRITE_ERROR
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiStorageAreaSetUnlock(
	uint32_t Id,		// IN	Storage Area Id
	uint8_t *pBuffer, 	// IN	Pointer to Data pBuffer
	uint32_t BufLen  	// IN	Buffer length 
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pBuffer==NULL								| SUSI_STATUS_INVALID_PARAMETER
// BufLen==0									| SUSI_STATUS_INVALID_PARAMETER
// Lock error									| SUSI_STATUS_WRITE_ERROR
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiStorageAreaSetLock(
	uint32_t Id,		// IN	Storage Area Id
	uint8_t *pBuffer, 	// IN	Pointer to Data pBuffer
	uint32_t BufLen  	// IN	Buffer length 
	);


//=============================================================================
// Fan Control
//=============================================================================

// Fan control value item IDs
#define SUSI_ID_FC_CONTROL_SUPPORT_FLAGS		0x00000000	// Reference "Control Support Flags"
#define SUSI_ID_FC_AUTO_SUPPORT_FLAGS			0x00000001	// Reference "Auto Support Flags"

// Control Support Flags
#define SUSI_FC_FLAG_SUPPORT_OFF_MODE			(1 << 0)	// Support OFF mode
#define SUSI_FC_FLAG_SUPPORT_FULL_MODE			(1 << 1)	// Support FULL mode
#define SUSI_FC_FLAG_SUPPORT_MANUAL_MODE		(1 << 2)	// Support Manual mode
#define SUSI_FC_FLAG_SUPPORT_AUTO_MODE			(1 << 3)	// Support Auto mode

// Auto Support Flags
#define SUSI_FC_FLAG_SUPPORT_AUTO_LOW_STOP		(1 << 0)	// Support Low Stop Behavior (Depend on Auto mode)
#define SUSI_FC_FLAG_SUPPORT_AUTO_LOW_LIMIT		(1 << 1)	// Support Low Limit Behavior (Depend on Auto mode)
#define SUSI_FC_FLAG_SUPPORT_AUTO_HIGH_LIMIT	(1 << 2)	// Support High Limit Behavior (Depend on Auto mode)
#define SUSI_FC_FLAG_SUPPORT_AUTO_PWM			(1 << 8)	// Support PWM operate mode (Depend on Auto mode)
#define SUSI_FC_FLAG_SUPPORT_AUTO_RPM			(1 << 9)	// Support RPM operate mode (Depend on Auto mode)

// Note:
//	Item ID also can use SUSI_ID_HWM_TEMP_XXX to get is it support in this function or not.

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pValue==NULL									| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiFanControlGetCaps( 
	uint32_t Id,		// IN	Thermal Id
	uint32_t ItemId,	// IN	Value item Id
	uint32_t *pValue	// OUT	Return Value
	);


#define SUSI_FAN_AUTO_CTRL_OPMODE_PWM	0
#define SUSI_FAN_AUTO_CTRL_OPMODE_RPM	1
typedef struct _AutoFan {
	uint32_t TmlSource;		// Thermal Source (Mapping to SUSI_ID_HWM_FAN_XXX)
	uint32_t OpMode;
	uint32_t LowStopLimit;	// Temperature (0.1 Kelvins)
	uint32_t LowLimit;		// Temperature (0.1 Kelvins)
	uint32_t HighLimit;		// Temperature (0.1 Kelvins)
	uint32_t MinPWM;		// Enable when OpMode == FAN_AUTO_CTRL_OPMODE_PWM
	uint32_t MaxPWM;		// Enable when OpMode == FAN_AUTO_CTRL_OPMODE_PWM
	uint32_t MinRPM;		// Enable when OpMode == FAN_AUTO_CTRL_OPMODE_RPM
	uint32_t MaxRPM;		// Enable when OpMode == FAN_AUTO_CTRL_OPMODE_RPM
} AutoFan , *PAutoFan ;

// Mode
#define SUSI_FAN_CTRL_MODE_OFF			0
#define SUSI_FAN_CTRL_MODE_FULL			1
#define SUSI_FAN_CTRL_MODE_MANUAL		2
#define SUSI_FAN_CTRL_MODE_AUTO			3
typedef struct _SusiFanControl {
	uint32_t Mode;
	uint32_t PWM;			// Manual mode only (0 - 100%)
	AutoFan AutoControl;	// Auto mode only
} SusiFanControl, *PSusiFanControl;

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pConfig==NULL								| SUSI_STATUS_INVALID_PARAMETER
// pConfig->Size Invalid						| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiFanControlGetConfig(
	uint32_t Id,				// IN	Fan Id
	SusiFanControl *pConfig		// OUT	Fan config
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pConfig==NULL								| SUSI_STATUS_INVALID_PARAMETER
// pConfig->Size Invalid						| SUSI_STATUS_INVALID_PARAMETER
// Config Invalid								| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiFanControlSetConfig(
	uint32_t Id,				// IN	Fan Id
	SusiFanControl *pConfig		// IN	Fan config
	);


//=============================================================================
// Thermal Protect
//=============================================================================
#define SUSI_ID_THERMAL_MAX						4		
#define SUSI_ID_THERMAL_PROTECT_1				0
#define SUSI_ID_THERMAL_PROTECT_2				1
#define SUSI_ID_THERMAL_PROTECT_3				2
#define SUSI_ID_THERMAL_PROTECT_4				3


// Thermal Protection value item IDs
#define SUSI_ID_TP_EVENT_SUPPORT_FLAGS			0x00000000	// Reference "Support Flags"
#define SUSI_ID_TP_EVENT_TRIGGER_MAXIMUM		0x00000001	// Send Event
#define SUSI_ID_TP_EVENT_TRIGGER_MINIMUM		0x00000002
#define SUSI_ID_TP_EVENT_CLEAR_MAXIMUM			0x00000003
#define SUSI_ID_TP_EVENT_CLEAR_MINIMUM			0x00000004

// Support Flags
#define SUSI_THERMAL_FLAG_SUPPORT_SHUTDOWN		(1 << 0)
#define SUSI_THERMAL_FLAG_SUPPORT_THROTTLE		(1 << 1)
#define SUSI_THERMAL_FLAG_SUPPORT_POWEROFF		(1 << 2)

// Note:
//	Item ID also can use SUSI_ID_HWM_TEMP_XXX to get is it support in this function or not.

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pValue==NULL									| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiThermalProtectionGetCaps( 
	uint32_t Id,		// IN	Thermal Id
	uint32_t ItemId,	// IN	Value item Id
	uint32_t *pValue	// OUT	Return Value
	);


// Thermal Protection Event type
#define SUSI_THERMAL_EVENT_SHUTDOWN				0x00
#define SUSI_THERMAL_EVENT_THROTTLE				0x01
#define SUSI_THERMAL_EVENT_POWEROFF				0x02
#define SUSI_THERMAL_EVENT_NONE					0xFF

typedef struct _SusiThermalProtect{
	uint32_t SourceId;					// Reference SUSI_ID_HWM_TEMP_XXX
	uint32_t EventType;
	uint32_t SendEventTemperature;		// 0.1 Kelvins
	uint32_t ClearEventTemperature;		// 0.1 Kelvins
} SusiThermalProtect, *PSusiThermalProtect;

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pConfig==NULL								| SUSI_STATUS_INVALID_PARAMETER
// pConfig->Size Invalid						| SUSI_STATUS_INVALID_PARAMETER
// Config Invalid								| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t
SUSI_API
SusiThermalProtectionSetConfig(
	uint32_t Id,				// IN	Thermal Id
	SusiThermalProtect *pConfig	// IN	Thermal config
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pConfig==NULL								| SUSI_STATUS_INVALID_PARAMETER
// pConfig->Size Invalid						| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t
SUSI_API
SusiThermalProtectionGetConfig(
	uint32_t Id,				// IN	Thermal Id
	SusiThermalProtect *pConfig	// OUT	Thermal config
	);


//=============================================================================
// WDT
//=============================================================================
#define SUSI_ID_WATCHDOG_MAX				3
#define SUSI_ID_WATCHDOG_1					0
#define SUSI_ID_WATCHDOG_2					1
#define SUSI_ID_WATCHDOG_3					2

// WDT value item IDs
#define SUSI_ID_WDT_DELAY_MAXIMUM			0x00000001
#define SUSI_ID_WDT_DELAY_MINIMUM			0x00000002
#define SUSI_ID_WDT_EVENT_MAXIMUM			0x00000003
#define SUSI_ID_WDT_EVENT_MINIMUM			0x00000004
#define SUSI_ID_WDT_RESET_MAXIMUM			0x00000005
#define SUSI_ID_WDT_RESET_MINIMUM			0x00000006
#define SUSI_ID_WDT_UNIT_MINIMUM			0x0000000F
#define SUSI_ID_WDT_DELAY_TIME				0x00010001
#define SUSI_ID_WDT_EVENT_TIME				0x00010002
#define SUSI_ID_WDT_RESET_TIME				0x00010003
#define SUSI_ID_WDT_EVENT_TYPE				0x00010004

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// pValue==NULL									| SUSI_STATUS_INVALID_PARAMETER
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiWDogGetCaps( 
	uint32_t Id,		// IN	WDog Id
	uint32_t ItemId,	// IN	Value item Id
	uint32_t *pValue	// OUT	Return Value
	);


// Event Types
#define SUSI_WDT_EVENT_TYPE_NONE			0x00000000
#define SUSI_WDT_EVENT_TYPE_IRQ				0x00000001
#define SUSI_WDT_EVENT_TYPE_SCI				0x00000002
#define SUSI_WDT_EVENT_TYPE_PWRBTN			0x00000003

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Running										| SUSI_STATUS_RUNNING
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiWDogStart( 
	uint32_t Id,		// IN	WDog Id
	uint32_t DelayTime,	// IN	Value delay time
	uint32_t EventTime,	// IN	Value event time
	uint32_t ResetTime,	// IN	Value reset time
	uint32_t EventType	// IN	Determine event type
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiWDogStop( 
	uint32_t Id		// IN	WDog Id
	);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiWDogTrigger( 
	uint32_t Id		// IN	WDog Id
	);


typedef void (*SUSI_WDT_INT_CALLBACK)(void*);

// Condition									| Return Values 
// ---------------------------------------------+------------------------------
// Library Uninitialized						| SUSI_STATUS_NOT_INITIALIZED
// Unknown Id									| SUSI_STATUS_UNSUPPORTED
// Else											| SUSI_STATUS_SUCCESS
uint32_t SUSI_API SusiWDogSetCallBack( 
	uint32_t Id,						// IN	WDog Id
	SUSI_WDT_INT_CALLBACK pfnCallback,	// IN	WDog INT event callback
	void *Context						// IN	WDog INT event callback context
	);

#ifdef __cplusplus
}
#endif

#endif /* _SUSI4_H_ */
