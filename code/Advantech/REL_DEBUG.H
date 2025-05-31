//======================================================================
//           ADVANTECH - Trusted ePlatform Services                              
//======================================================================
//
// DEBUG.H - Header File for Function Index Codes in SUSI Advanced Debugging
// 
// Copyright (c) Advantech Co., Ltd. All Rights Reserved
//======================================================================
// Revision History :
// --------------------------------------------
// July 2007 - first featuring in Susi v1.2	
// --------------------------------------------
// Author : Josh Chang, ESSD Presention
//======================================================================

#ifndef _SUSI_DEBUG_H_
#define _SUSI_DEBUG_H_

//**********************************************************************
// Debug Code Format
//
// Debug code = Function Code + Error Code 
// Error Code may come from driver / library itself, please refer to 
// ERRDRV.H and REL_ERRLIB.H

//----------------------------------------------------------------------
// Function Index Codes : 100000+
//
typedef enum
{
	//-----------------------------------------------------
    // Dll :     00100000    ~   09900000
    //--------------------------------------------------
	// Driver Independent
  	ESusiInit                  =   100000, // dont use 00100000 -> error number will be generated
  	ESusiUnInit                =   200000, 
	ESusiGetVersion            =   300000,    
	ESusiDllInit               =   400000,
	ESusiDllUnInit             =   500000,
    ESusiDllGetVersion         =   600000,
	ESusiDllGetLastError       =   700000,
    //-----------------------------------------------------
    // Core :    10100000    ~   19900000
    //--------------------------------------------------
	ESusiCoreInit              = 10100000,  //*A 
	ESusiCoreAvailable         = 10200000,
  	ESusiCoreGetBIOSVersion    = 10300000,
  	ESusiCoreGetPlatformName   = 10400000,	
	ESusiCoreAccessBootCounter = 10500000,
	ESusiCoreAccessRunTimer    = 10600000,
	ESusiCoreRebootSystem      = 10700000,
//	ESusiReserved		       = 10800000,
//	ESusiReserved			   = 10900000,
	ESusiCoreReadIO            = 11000000,
	ESusiCoreWriteIO           = 11100000,
	ESusiCoreReadULongIO       = 11200000,
	ESusiCoreWriteULongIO      = 11300000,
	ESusiCorePciBusSetULong    = 11400000,
	ESusiCorePciBusGetULong    = 11500000,
	ESusiCoreGetCpuMaxSpeed    = 11600000,
    ESusiCoreGetCpuVendor      = 11700000,
//	ESusiReserved			   = 11800000,
//	ESusiReserved			   = 11900000,
    ESusiCoreEnableBootfail    = 12000000,
	ESusiCoreDisableBootfail   = 12100000,
	ESusiCoreRefreshBootfail   = 12200000,
	ESusiPlusSetThrottlingfail = 12300000,
	ESusiPlusSetSpeedStepfail  = 12400000,
	ESusiPlusGetThrottlingfail = 12500000,
	ESusiPlusGetSpeedStepfail  = 12600000,
  	//-----------------------------------------------------
    // Watchdog : 20100000   ~   29900000
    //--------------------------------------------------
	ESusiWDInit                = 20100000,  //*A 
  	ESusiWDAvailable           = 20200000,
  	ESusiWDDisable             = 20300000,
  	ESusiWDGetRange            = 20400000,
  	ESusiWDSetConfig           = 20500000,
  	ESusiWDTrigger             = 20600000,
	ESusiWDT                   = 20700000,
	ESusiWDTriggerEx           = 20800000,
	ESusiWDDisableEx           = 20900000,
	ESusiWDSetConfigEx         = 21000000,
  	//-----------------------------------------------------
    // GPIO :     30100000   ~   39900000
    //--------------------------------------------------
  	ESusiIOInit                = 30100000,  //*A 
	ESusiIOAvailable           = 30200000,	
  	ESusiIOCount               = 30300000,
  	ESusiIOInitial             = 30400000,
  	ESusiIORead                = 30500000,
  	ESusiIOReadMulti           = 30600000,
  	ESusiIOWrite               = 30700000,
  	ESusiIOWriteMulti          = 30800000,
  	ESusiIOCountEx             = 30900000,
  	ESusiIOQueryMask           = 31000000,
  	ESusiIOSetDirection        = 31100000,
  	ESusiIOSetDirectionMulti   = 31200000,
  	ESusiIOReadEx              = 31300000,
  	ESusiIOReadMultiEx         = 31400000,
  	ESusiIOWriteEx             = 31500000,
  	ESusiIOWriteMultiEx        = 31600000,	
  	//-----------------------------------------------------
    // SMBus :    40100000   ~   49900000
    //--------------------------------------------------
	ESusiSMBusInit             = 40100000,  //*A 
    ESusiSMBusAvailable        = 40200000,
  	ESusiSMBusReadByte         = 40300000,
  	ESusiSMBusReadByteMulti    = 40400000,
  	ESusiSMBusReadWord         = 40500000,
  	ESusiSMBusWriteByte        = 40600000,
  	ESusiSMBusWriteByteMulti   = 40700000,
  	ESusiSMBusWriteWord        = 40800000,
	ESusiSMBusReceiveByte      = 40900000,
	ESusiSMBusSendByte         = 41000000,
	ESusiSMBusWriteQuick       = 41100000,
	ESusiSMBusReadQuick        = 41200000,
	ESusiSMBusScanDevice       = 41300000,
	ESusiSMBusWriteBlock       = 41400000,
	ESusiSMBusReadBlock        = 41500000,
	ESusiSMBusI2CReadBlock     = 41600000,
	ESusiSMBusI2CWriteBlock    = 41700000,
	ESusiSMBusReset			   = 41800000,
  	//-----------------------------------------------------
    // IIC  :     50100000   ~   59900000
    //--------------------------------------------------
    ESusiIICInit               = 50100000,  //*A 
  	ESusiIICAvailable          = 50200000,
  	ESusiIICWriteReadCombine   = 50300000,
  	ESusiIICRead               = 50400000,
  	ESusiIICWrite              = 50500000,
	ESusiIICReadByteMulti      = 50600000,
	ESusiIICWriteByteMulti     = 50700000,
  	//-----------------------------------------------------
    // VC:        60100000   ~   69900000
    //--------------------------------------------------	
	ESusiVCInit                = 60100000,  //*A 
  	ESusiVCAvailable           = 60200000, 
  	ESusiVCGetBright           = 60300000,
  	ESusiVCGetBrightRange      = 60400000,
  	ESusiVCScreenOff           = 60500000,
  	ESusiVCScreenOn            = 60600000,
  	ESusiVCSetBright           = 60700000,
  	//-----------------------------------------------------
    // HWM :      70100000   ~   79900000
    //--------------------------------------------------
	ESusiHWMInit               = 70100000,  //*A 
    ESusiHWMAvailable          = 70200000,
    ESusiHWMGetFanSpeed        = 70300000,
    ESusiHWMGetTemperature     = 70400000,
    ESusiHWMGetVoltage         = 70500000,
	ESusiHWMSetFanSpeed        = 70600000,
	
	//-----------------------------------------------------
    // PWS :	  80100000   ~   89900000
    //--------------------------------------------------
	ESusiPWSInit               = 80100000,
	ESusiPWSAvailable          = 80200000,
	ESusiPWRSetCPUFrequency    = 80300000
	
	
	//-----------------------------------------------------
    // Rsrvd :    90100000 ~   2119900000
    //--------------------------------------------------

} ESUSI_FUNCODE;

//[Note]
// *A - this function is for internal use, not an open API

#endif // _SUSI_DEBUG_H_

