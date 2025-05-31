//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TEtherCAT3.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   TwinCat3
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#pragma hdrstop

#include <windows.h>
#include "TEventDispatcher.h"
#include "TData.h"
#include "TEtherCAT3.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

int TEtherCAT3::counter = 0;
bool TEtherCAT3::IOConnected = false;

TEtherCAT3::TEtherCAT3( TDataManager *parent)                                  {

   TDataManager *mngrVersion = new TDataManager( L"Version control", parent);
   pUpdate              = new TData( mngrVersion, L"Update version",          NULL, 0, 0, 1,       NULL, (FLAG_LEVEL_0 | FLAG_SAVE_NO_VALUE | FLAG_NOTIFY));
   pCodeVersion         = new TData( mngrVersion, L"Code Version",   NULL, 0, 0, 9999,    NULL, (FLAG_LEVEL_0 | FLAG_SAVE | FLAG_RDONLY | FLAG_NOTIFY));
      if ( ++counter == 1)        {

      AdsPortClose();
      timeBeginPeriod(1);
   }

   m_codeVersion = -1;
   m_parent = parent;

   mngrVersion->DataModEventPost = DataModifiedPost;
}

//-----------------------------------------------------------------------------


TEtherCAT3::~TEtherCAT3()                                                       {

   counter--;
   if ( IOConnected && (counter==0) )                                         {

      AdsPortClose();
      timeEndPeriod(1);
      IOConnected = false;
   }

}

//-----------------------------------------------------------------------------
// CheckVersion
bool TEtherCAT3::CheckVersion(DWORD cfgVersion, DWORD codeVersion)                               {
   if( !IOConnected || !pCodeVersion)
      return false;
   bool actualizar = false;

   m_codeVersion = codeVersion;

   //-- Comprobación de versión
   // FORMATO: YYXX, ( ej: 100 = 01.00)
   //    YY: Major Versión,
   //    XX: Minor Version
   //       Versiones con distinto major Version NO compatibles, versiones con igual
   //       major version y distinto minor version compatibles.


   wchar_t txt[512];
   if( cfgVersion < 100 || (unsigned)cfgVersion != codeVersion)                                 {
      // Versión de Driver distinta
      if( cfgVersion < 100)                                           {
         //-- Primera vez que se arranca la aplicación, o error de versión
         swprintf( txt, L"(%4.2f)", codeVersion/100.);
         DISPATCH_EVENT( WARN_CODE, L"EV_ETH_NOVERSION", txt, m_parent);
         actualizar   = true;
      }
      else                                                           {
         // Comprobamos la Major Version
         if( (unsigned)(cfgVersion / 100) != codeVersion / 100)               {
            swprintf( txt, L"CFG: (%4.2f). INST: (%4.2f)", cfgVersion/100., codeVersion/100.);
            DISPATCH_EVENT( WARN_CODE, L"EV_ETH_INCOMPATIBLEVERSION", txt, m_parent);
            // Drivers no compatibles
         }
         // Comprobamos minor Version
         else if( (unsigned)(cfgVersion % 100) != codeVersion % 100)          {
            swprintf( txt, L"CFG: (%4.2f). INST: (%4.2f)", cfgVersion/100., codeVersion/100.);
            DISPATCH_EVENT( WARN_CODE, L"EV_ETH_COMPATIBLEVERSION", txt, m_parent);
            // Drivers compatibles pero con versión distinta
         }
      }
      // Como no hemos actualizado los Tdatas en el update de arriba, aquí actualizamos en ambos casos
      if( actualizar)
         pCodeVersion->SetAsInt( codeVersion);
      return actualizar;
   }

   return true;
}
//----------------------------------------------------------------------------

void __fastcall TEtherCAT3::DataModifiedPost( TData* da)                     {
   if( da == pUpdate && da->AsInt() && m_codeVersion != -1)                 {
      pCodeVersion->SetAsInt( 9999);
      pCodeVersion->SetAsInt( m_codeVersion);
      da->SetAsInt(0);
   }

}

//----------------------------------------------------------------------------
bool TEtherCAT3::IsRunning()                                        {

   // Si no hemos conectado, no esta Ok, no hace falta la comprobación
   if( !IOConnected) {
      return false;
   }

   SC_HANDLE      sc_manager,
                  sc_service;
   SERVICE_STATUS status;

   // Conectamos al manager de servicios
   sc_manager = OpenSCManager(   NULL,	NULL,	SC_MANAGER_ALL_ACCESS);
   if( sc_manager != NULL)                                                       {
      // Abirmos el servicion indicado
      sc_service = OpenService( sc_manager, L"TcSysSrv", SERVICE_ALL_ACCESS);
      if( sc_service != NULL)                                                    {

         // Comprobamos el estado
         QueryServiceStatus( sc_service, &status);
         if( status.dwCurrentState == SERVICE_RUNNING)                  {

            //-- Aseguramos que todas las tareas de PLC están arrancadas
            AmsAddr     Addr ;        //  ToDo: comentado por problemas con la libreria TCATAds en el RequestCfg, despues de un Close
            PAmsAddr    pAddr = &Addr;
             pAddr->port = NULL;
            // Abrimos puerto en PLC local
            AdsPortOpen();
            if (!AdsGetLocalAddress(pAddr))                                          {
               pAddr->port = ETHERCATRT_PORTNUMBER;

               // forzamos el arranque de todas las tareas de PLC
               int err;
               do                                                                   {
                  err = AdsSyncWriteControlReq (pAddr, ADSSTATE_RUN, 0, 0, NULL);
                  pAddr->port += 1;      // Cada PLC está separado por 1  en Twincat3
               }while (err == ADSERR_NOERR || err == ADSERR_DEVICE_INVALIDSTATE);
            }
            AdsPortClose();
            return true;
         }
      }
   }

   return false;
}





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
