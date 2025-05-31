//======================================================================
//           ADVANTECH - Trusted ePlatform Services                              
//======================================================================
//
// REL_SUSI.H - Header File for SUSI 
// 
// Copyright (c) Advantech Co., Ltd. All Rights Reserved
//======================================================================
// Revision History :
// --------------------------------------------
// July 2007 - first featuring in Susi v1.2	
// --------------------------------------------
// Author : Josh Chang, ESSD Presention
//======================================================================
#ifndef _SUSI_H_
#define _SUSI_H_

#ifdef __cplusplus 
extern "C" { 
#endif 

#define SUSI_API __declspec(dllexport)


//*****************************************************************************
//	Core Definition 
//*****************************************************************************
//-----------------------------------------------------------------------------
//	CPU Information
//-----------------------------------------------------------------------------
// Vendor
#define INTEL		1 << 0
#define VIA  		1 << 1
#define SIS  		1 << 2
#define NVIDIA  	1 << 3
#define AMD  		1 << 4
#define RDC  		1 << 5


//*****************************************************************************
//	Boot Counter Definition 
//*****************************************************************************
typedef enum
{	
	ESCORE_BOOTCOUNTER_STATUS  = 0x01, 
	ESCORE_BOOTCOUNTER_VALUE   = 0x02

} ESCORE_BOOTCOUNTER;

enum
{
	ESCORE_BOOTCOUNTER_MODE_GET = 0,
	ESCORE_BOOTCOUNTER_MODE_SET = 1
};


//*****************************************************************************
//	Run Timer Definition 
//*****************************************************************************
typedef enum 
{	
	ESCORE_RUNTIMER_STATUS_RUNNING		= 0x01, 
	ESCORE_RUNTIMER_STATUS_AUTORUN		= 0x02,
	ESCORE_RUNTIMER_VALUE_CONTINUALON	= 0x04,
	ESCORE_RUNTIMER_VALUE_TOTALON		= 0x08
} ESCORE_RUNTIMER;

enum
{
	ESCORE_RUNTIMER_MODE_GET = 0,
	ESCORE_RUNTIMER_MODE_SET = 1
};

typedef struct 
{
  DWORD dwOPFlag;
  BOOL  isRunning;
  BOOL  isAutorun; 
  DWORD dwTimeContinual;
  DWORD dwTimeTotal;

} SSCORE_RUNTIMER, *PSSCORE_RUNTIMER;


//*****************************************************************************
//	GPIO Definition 
//*****************************************************************************
typedef enum
{
   // Static mask
   ESIO_SMASK_PIN_FULL			= 0x0001,
   ESIO_SMASK_CONFIGURABLE		= 0x0002,
   ESIO_SMASK_FIXIN				= 0x0004,
   ESIO_SMASK_FIXOUT			= 0x0008,
   ESIO_SMASK_IF_OUT_READABLE	= 0x0010,
   // Dynamic mask
   ESIO_DMASK_DIRECTION			= 0x0020,
   ESIO_DMASK_IN				= 0x0040,   
   ESIO_DMASK_OUT				= 0x0080,
   ESIO_DMASK_READABLEOUT		= 0x0100
} ESIO_MASKFLAG;


//*****************************************************************************
//	HWM Definition
//*****************************************************************************
// Voltage
#define VMAX_SHIFT	14	
#define VCORE	(1 << 0)			// Vcore
#define V25		(1 << 1)			// 2.5V
#define V33		(1 << 2)			// 3.3V
#define V50		(1 << 3)			// 5V
#define V120	(1 << 4)			// 12V
#define V5SB	(1 << 5)			// V5 standby
#define V3SB	(1 << 6)			// V3 standby
#define VBAT	(1 << 7)			// VBAT
#define VN50	(1 << 8)			// -5V
#define VN120	(1 << 9)			// -12V
#define VTT		(1 << 10)			// VTT
#define VCORE2	(1 << 11)			// Vcore2
#define V105	(1 << 12)			// 1.05V
#define V15		(1 << 13)			// 1.5V
#define V18		(1 << VMAX_SHIFT)	// 1.8V

// Temperature
#define TCPU	(1 << 0)	// CPU temperaturez
#define TSYS	(1 << 1)	// System temperature
#define TAUX    (1 << 2)	// 3'rd thermal dioad

// Fan Speed
#define FCPU	(1 << 0)	// CPU FAN Speed
#define FSYS	(1 << 1)	// System FAN Speed
#define F2ND	(1 << 2)	// Other FAN Speed


//*****************************************************************************
//	IIC Definition 
//*****************************************************************************
#define SUSI_IIC_TYPE_PRIMARY				1
#define SUSI_IIC_TYPE_SMBUS					2
#define SUSI_IIC_TYPE_BOTH					3


//*****************************************************************************
// WDT Definition
//*****************************************************************************
#define WDTOUT0		0
#define WDTOUT1		1
#define WDTGPIO		2
#define WDTMAXNUM	3		// WDT Max. Number, for count amount.


//*****************************************************************************
//	SUSI APIs 
//*****************************************************************************
//-----------------------------------------------------------------------------
//	Driver Independent APIs  
//-----------------------------------------------------------------------------
SUSI_API int SusiDllGetLastError();
SUSI_API void SusiDllGetVersion(DWORD *major, DWORD *minor);
SUSI_API BOOL SusiDllInit();
SUSI_API BOOL SusiDllUnInit();
														// Old APIs for backard compatible, now use the whole new set
SUSI_API void SusiGetVersion(WORD *major, WORD *minor); // now use SusiDllGetVersion
SUSI_API BOOL SusiInit();								// now use SusiDllInit
SUSI_API BOOL SusiUnInit();								// now use SusiDllUnInit

//-----------------------------------------------------------------------------
//	CORE
//-----------------------------------------------------------------------------
SUSI_API int SusiCoreAvailable();
SUSI_API BOOL SusiCoreAccessBootCounter(DWORD mode, DWORD OPFlag, BOOL *enable, DWORD *value);
SUSI_API BOOL SusiCoreAccessRunTimer(DWORD mode, PSSCORE_RUNTIMER pRunTimer);
SUSI_API BOOL SusiCoreGetBIOSVersion(TCHAR *BIOSVersion, DWORD *size);
SUSI_API BOOL SusiCoreGetPlatformName(TCHAR *PlatformName, DWORD *size);
SUSI_API BOOL SusiCoreRebootSystem(DWORD delay, BOOL regSave);
SUSI_API BOOL SusiCoreGetBIOSString(TCHAR *stringBIOS, DWORD *size);

SUSI_API BOOL SusiCoreReadIO(DWORD addr, DWORD &value);
SUSI_API BOOL SusiCoreWriteIO(DWORD addr, DWORD value);
SUSI_API BOOL SusiCoreReadULongIO(DWORD addr, DWORD &value);
SUSI_API BOOL SusiCoreWriteULongIO(DWORD addr, DWORD value);
SUSI_API BOOL SusiCorePciBusSetULong(int busNum, int devNum, int funNum, UCHAR regIndex);
SUSI_API BOOL SusiCorePciBusGetULong(int busNum, int devNum, int funNum, UCHAR regIndex, DWORD &value);

SUSI_API BOOL SusiCoreGetCpuMaxSpeed(DWORD &Value);
SUSI_API BOOL SusiCoreGetCpuVendor(DWORD &Value);

SUSI_API BOOL SUSIPlusCpuSetThrottling(unsigned char step);
SUSI_API BOOL SUSIPlusCpuGetThrottling(unsigned char *step);
SUSI_API BOOL SUSIPlusCpuSetSpeedStep(HANDLE proc_handler, unsigned char cpu_index, unsigned char step);
SUSI_API BOOL SUSIPlusCpuGetSpeedStep(HANDLE proc_handler, unsigned char cpu_index, unsigned char *step);

SUSI_API BOOL SusiCoreEnableBootfail();
SUSI_API BOOL SusiCoreDisableBootfail();
SUSI_API BOOL SusiCoreRefreshBootfail();

SUSI_API int SusiGetBIOSVersion(TCHAR *BIOSVersion, BYTE size);		// Old API for backward compatible, now use SusiCoreGetBIOSVersion!
SUSI_API int SusiGetPlatformName(TCHAR *PlatformName, BYTE size);	// Old API for backward compatible, now use SusiCoreGetPlatformName!


//-----------------------------------------------------------------------------
//	WDT
//-----------------------------------------------------------------------------
SUSI_API int SusiWDAvailable();
SUSI_API BOOL SusiWDDisable();
SUSI_API BOOL SusiWDGetRange(DWORD *minimum, DWORD *maximum, DWORD *stepping);
SUSI_API BOOL SusiWDSetConfig(DWORD delay, DWORD timeout);
SUSI_API BOOL SusiWDTrigger();
SUSI_API BOOL SusiWDDisableEx(int group_number);
SUSI_API BOOL SusiWDSetConfigEx(int group_number, DWORD delay, DWORD timeout);
SUSI_API BOOL SusiWDTriggerEx(int group_number);
SUSI_API BOOL SusiSetDetectPowerOffProtection(DWORD time);
SUSI_API BOOL SusiClearDetectPowerOffProtection();

//-----------------------------------------------------------------------------
//	GPIO
//-----------------------------------------------------------------------------
// New API set for configurable GPIO
SUSI_API BOOL SusiIOCountEx(DWORD *inCount, DWORD *outCount);
SUSI_API BOOL SusiIOQueryMask(DWORD flag, DWORD *Mask);
SUSI_API BOOL SusiIOReadEx(BYTE PinNum, BOOL *status);
SUSI_API BOOL SusiIOReadMultiEx(DWORD TargetPinMask, DWORD *StatusMask);
SUSI_API BOOL SusiIOSetDirection(BYTE PinNum, BYTE IO, DWORD *PinDirMask); 
SUSI_API BOOL SusiIOSetDirectionMulti(DWORD TargetPinMask, DWORD *PinDirMask);
SUSI_API BOOL SusiIOWriteEx(BYTE PinNum, BOOL status); 
SUSI_API BOOL SusiIOWriteMultiEx(DWORD TargetPinMask, DWORD StatusMask);

//Old APIs for backard compatible, now use the whole new set
SUSI_API BOOL SusiIOAvailable();
SUSI_API BOOL SusiIOInitial(DWORD statuses);
SUSI_API BOOL SusiIOCount(WORD *inCount, WORD *outCount); 
SUSI_API BOOL SusiIORead(BYTE pin, BOOL *status);
SUSI_API BOOL SusiIOReadMulti(DWORD pins, DWORD *statuses);
SUSI_API BOOL SusiIOWrite(BYTE pin, BOOL status);
SUSI_API BOOL SusiIOWriteMulti(DWORD pins, DWORD statuses);

//-----------------------------------------------------------------------------
//	SMBus
//-----------------------------------------------------------------------------
SUSI_API int SusiSMBusAvailable();
SUSI_API BOOL SusiSMBusReadBlock(BYTE SlaveAddress, BYTE RegisterOffset, BYTE *Result, BYTE *ByteCount);
SUSI_API BOOL SusiSMBusI2CReadBlock(BYTE SlaveAddress, BYTE RegisterOffset, BYTE *Result, BYTE *ByteCount);
SUSI_API BOOL SusiSMBusReadByte(BYTE SlaveAddress, BYTE RegisterOffset, BYTE *Result);
SUSI_API BOOL SusiSMBusReadByteMulti(BYTE SlaveAddress, BYTE RegisterOffset, BYTE *Result, BYTE ByteCount);
SUSI_API BOOL SusiSMBusReadQuick(BYTE SlaveAddress);
SUSI_API BOOL SusiSMBusReadWord(BYTE SlaveAddress, BYTE RegisterOffset, WORD *Result);
SUSI_API BOOL SusiSMBusReceiveByte(BYTE SlaveAddress, BYTE *Result);
SUSI_API int SusiSMBusScanDevice(BYTE SlaveAddress_7);
SUSI_API BOOL SusiSMBusSendByte(BYTE SlaveAddress, BYTE Result);
SUSI_API BOOL SusiSMBusWriteBlock(BYTE SlaveAddress, BYTE RegisterOffset, BYTE *Result, BYTE ByteCount);
SUSI_API BOOL SusiSMBusI2CWriteBlock(BYTE SlaveAddress, BYTE RegisterOffset, BYTE *Result, BYTE ByteCount);
SUSI_API BOOL SusiSMBusWriteByte(BYTE SlaveAddress, BYTE RegisterOffset, BYTE Result);
SUSI_API BOOL SusiSMBusWriteByteMulti(BYTE SlaveAddress, BYTE RegisterOffset, BYTE *Result, BYTE ByteCount);
SUSI_API BOOL SusiSMBusWriteQuick(BYTE SlaveAddress);
SUSI_API BOOL SusiSMBusWriteWord(BYTE SlaveAddress, BYTE RegisterOffset, WORD Result);
SUSI_API BOOL SusiSMBusReset(void);

//-----------------------------------------------------------------------------
//	IIC
//-----------------------------------------------------------------------------
// New
SUSI_API int SusiIICAvailable();
SUSI_API BOOL SusiIICRead(DWORD IICType, BYTE SlaveAddress, BYTE *ReadBuf, DWORD ReadLen);
SUSI_API BOOL SusiIICWrite(DWORD IICType, BYTE SlaveAddress, BYTE *WriteBuf, DWORD WriteLen);
SUSI_API BOOL SusiIICWriteReadCombine(DWORD IICType, BYTE SlaveAddress, BYTE *WriteBuf, DWORD WriteLen, BYTE *ReadBuf, DWORD ReadLen);
//Old APIs for backard compatible, now use the whole new set
SUSI_API BOOL SusiIICReadByteMulti(BYTE SlaveAddress, BYTE *ReadBuf, DWORD ReadLen); // Old API for backward compatible, now use SusiIICRead!
SUSI_API BOOL SusiIICWriteByteMulti(BYTE SlaveAddress, BYTE *WriteBuf, DWORD WriteLen); // Old API for backward compatible, now use SusiIICWrite!  

//-----------------------------------------------------------------------------
//	VC
//-----------------------------------------------------------------------------
SUSI_API int SusiVCAvailable();
SUSI_API BOOL SusiVCGetBright(BYTE *brightness);
SUSI_API BOOL SusiVCGetBrightRange(BYTE *minimum, BYTE *maximum, BYTE *stepping);
SUSI_API BOOL SusiVCScreenOff();
SUSI_API BOOL SusiVCScreenOn();
SUSI_API BOOL SusiVCSetBright(BYTE brightness);

//-----------------------------------------------------------------------------
//	HWM
//-----------------------------------------------------------------------------
SUSI_API int SusiHWMAvailable();
SUSI_API BOOL SusiHWMGetFanSpeed(WORD fanType, WORD *retval, WORD *typeSupport = NULL);
SUSI_API BOOL SusiHWMGetTemperature(WORD tempType, float *retval, WORD *typeSupport = NULL);
SUSI_API BOOL SusiHWMGetVoltage(DWORD voltType, float *retval, DWORD *typeSupport = NULL);
SUSI_API BOOL SusiHWMSetFanSpeed(WORD fanType, BYTE setval, WORD *typeSupport = NULL);


//*****************************************************************************
//	AdvLib APIs 
//*****************************************************************************
//  Only for backward compatible with Advantech Library,
//	new users of SUSI Library can simply ignore these
//-----------------------------------------------------------------------------
//	Driver Independent APIs
//-----------------------------------------------------------------------------
SUSI_API void AdvLibGetVersion(WORD *major, WORD *minor);
SUSI_API BOOL AdvLibInit();
SUSI_API BOOL AdvLibUnInit();

//-----------------------------------------------------------------------------
//	CORE
//-----------------------------------------------------------------------------
SUSI_API int AdvLibGetBIOSVersion(TCHAR *BIOSVersion, BYTE size);
SUSI_API int AdvLibGetPlatformName(TCHAR *PlatformName, BYTE size);

//-----------------------------------------------------------------------------
//	WDT
//-----------------------------------------------------------------------------
SUSI_API int AdvLibWDAvailable();
SUSI_API BOOL AdvLibWDDisable();
SUSI_API BOOL AdvLibWDGetRange(DWORD *minimum, DWORD *maximum, DWORD *stepping);
SUSI_API BOOL AdvLibWDSetConfig(DWORD delay, DWORD timeout);
SUSI_API BOOL AdvLibWDTrigger();

//-----------------------------------------------------------------------------
//	GPIO
//-----------------------------------------------------------------------------
SUSI_API BOOL AdvLibIOAvailable();
SUSI_API BOOL AdvLibIOCount(WORD *inCount, WORD *outCount);
SUSI_API BOOL AdvLibIOInitial(DWORD statuses);
SUSI_API BOOL AdvLibIORead(BYTE pin, BOOL *status);
SUSI_API BOOL AdvLibIOReadMulti(DWORD pins, DWORD *statuses);
SUSI_API BOOL AdvLibIOWrite(BYTE pin, BOOL status);
SUSI_API BOOL AdvLibIOWriteMulti(DWORD pins, DWORD statuses);

//-----------------------------------------------------------------------------
//	SMBus
//-----------------------------------------------------------------------------
SUSI_API BOOL AdvLibSMBusAvailable();
SUSI_API BOOL AdvLibSMBusReadByte(BYTE SlaveAddress, BYTE RegisterOffset, BYTE *Result);
SUSI_API BOOL AdvLibSMBusReadByteMulti(BYTE SlaveAddress,BYTE RegisterOffset, BYTE *Result, BYTE ByteCount);
SUSI_API BOOL AdvLibSMBusReadWord(BYTE SlaveAddress, BYTE RegisterOffset, WORD *Result);
SUSI_API BOOL AdvLibSMBusWriteByte(BYTE SlaveAddress, BYTE RegisterOffset, BYTE Result);
SUSI_API BOOL AdvLibSMBusWriteByteMulti(BYTE SlaveAddress,BYTE RegisterOffset, BYTE *Result, BYTE ByteCount);
SUSI_API BOOL AdvLibSMBusWriteWord(BYTE SlaveAddress, BYTE RegisterOffset, WORD Result);

//-----------------------------------------------------------------------------
//	VC
//-----------------------------------------------------------------------------
SUSI_API int AdvLibVCAvailable();
SUSI_API BOOL AdvLibVCGetBright(BYTE *brightness);
SUSI_API BOOL AdvLibVCGetBrightRange(BYTE *minimum, BYTE *maximum, BYTE *stepping);
SUSI_API BOOL AdvLibVCScreenOff();
SUSI_API BOOL AdvLibVCScreenOn();
SUSI_API BOOL AdvLibVCSetBright(BYTE brightness);

//-----------------------------------------------------------------------------
//	HWM
//-----------------------------------------------------------------------------
SUSI_API int AdvLibHWMAvailable();
SUSI_API BOOL AdvLibHWMGetFanSpeed(WORD fanType, WORD *retval, WORD *typeSupport = NULL);
SUSI_API BOOL AdvLibHWMGetTemperature(WORD tempType, float *retval, WORD *typeSupport = NULL);
SUSI_API BOOL AdvLibHWMGetVoltage(DWORD voltType, float *retval, DWORD *typeSupport = NULL);

//-----------------------------------------------------------------------------
//	PWS
//-----------------------------------------------------------------------------
SUSI_API BOOL SusiPWRSetCPUFrequency(DWORD sel);


#ifdef __cplusplus 
} 
#endif 

#endif //_SUSI_H_
