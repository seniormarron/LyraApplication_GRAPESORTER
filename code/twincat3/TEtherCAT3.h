//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TEtherCAT3.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   TwinCat3
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TEtherCAT3H
#define TEtherCAT3H
//---------------------------------------------------------------------------

#include <windows.h>
#include "ADST3\TcAdsDef.h"
#include "ADST3\TcAdsAPI.h"
#include "TDataMng.h"

#define CUADRO_ELECTRICO_PORTNUMBER  853      ///> define port
#define ETHERCATRT_PORTNUMBER  851            ///> define port
#define OMRONDRIVE_PORTNUMBER  852            ///> define port

class TData;


#ifdef _ISDLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif

class TEtherCAT3                                                               {

   protected:
    static bool   IOConnected;                  ///< connection against TWincat has been stablished
    static int    counter;                      ///< number of instances of this class
    bool          m_IOConnected;                ///< this instace of ethercat is connected to its port
    TData         *pCodeVersion,
                  *pUpdate;
    int           m_codeVersion;               ///< Instaled code version at the moment CheckVersion is called

   TDataManager  *m_parent;

   long     nErr,
            nErr1,
            nErr2,
            nErr3,
            nErr4,
            nPort,
            lHdlInput,                          ///<  HANDLE  writing to TwinCAT 3  ElectricCabinetT3
            lHlOutput,                          ///<  HANDLE  reading to TwinCAT 3  ElectricCabinetT3
            lHlEthInfoData,                     ///<  HANDLE  reading to TwinCAT 3  TEtherCATRT3
            lHlEthInfoCfg;                      ///<  HANDLE  writing to TwinCAT 3  TEtherCATRT3


   public:
    TEtherCAT3(TDataManager *parent);
    virtual ~TEtherCAT3();
    virtual bool  IsRunning();                          ///<Comprueba si el driver está trabajando,
    virtual void IOOpen() = 0;

   protected:
    virtual bool  Update() = 0;
    virtual void  SetCfg() = 0;                  ///<Escribir configuración
    bool CheckVersion(DWORD cfgVersion, DWORD codeVersion);
    // Communication with EtherCAt level

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
