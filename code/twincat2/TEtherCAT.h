//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TEtherCAT.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   TwinCat2
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#ifndef TEtherCATH
#define TEtherCATH

#include <windows.h>
#include "ADS\TcAdsDEF.h"
#include "ADS\TcAdsAPI.h"
#include "IO API\TCatIoApi.h"                 ///< header file shipped with TwinCAT® I/O
#include "TDataMng.h"

class TData;




class TEtherCAT                                                               {

   protected:
    static bool    IOConnected;                  ///< connection against TWincat has been stablished
    static int     counter;                      ///< number of instances of this class
    bool           m_IOConnected;                ///< this instace of ethercat is connected to its port
    TData         *pCodeVersion,
                  *pUpdate;
    int            m_codeVersion;               ///< Instaled code version at the moment CheckVersion is called
    TDataManager  *m_parent;

//   long     nErr,
//            nErr1,
//            nErr2,
//            nPort,
//            lHdlInput,                          ///<  HANDLE  writing to TwinCAT 3  ElectricCabinetT3
//            lHlOutput,                          ///<  HANDLE  reading to TwinCAT 3  ElectricCabinetT3
//            lHlEthInfoData,                     ///<  HANDLE  reading to TwinCAT 3  TEtherCATRT3
//            lHlEthInfoCfg;                      ///<  HANDLE  writing to TwinCAT 3  TEtherCATRT3


   public:
    TEtherCAT(TDataManager *parent);
    virtual ~TEtherCAT();
    virtual void IOOpen() = 0;
    virtual bool  IsRunning();                          ///<Comprueba si el driver está trabajando,

   protected:
    virtual bool  Update() = 0;
    virtual void  SetCfg() = 0;                  ///<Escribir configuración
    bool CheckVersion(DWORD cfgVersion, DWORD codeVersion);
    // Communication with EtherCAt level
    void IOOpenParam( int portNumber, void **ppOutputs, int sizeOutputs, void **pInputs , int sizeInputs);
    virtual bool SetTimers()  = 0;
    virtual void KillTimers() = 0;

    void __fastcall DataModifiedPost( TData* da);
};


#endif




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
