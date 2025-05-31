//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TStdElectricCabinetT3.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   TwinCat3
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TStdElectricCabinetT3H
#define TStdElectricCabinetT3H

#include "TEtherCAT3.h"
#include "TDataMngControl.h"
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include "TDataMng.h"

#include "ADST3\TcAdsDef.h"
#include "ADST3\TcAdsAPI.h"
#include "TBitState.h"
#include "TElectricCabinet.h"

 /// Faulty flag codes
#define FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD 0x10 ///<bit 0 of flagsArray

//***** Estructuras y defines para el control del cuadro el�ctrico
// Estados ETHERCAT -> PC
typedef struct                                 {
	DWORD                ESTADO,                    ///< Variable con los posibles estados enviados A la aplicaci�n
                        CODE_VERSION;              ///< Versi�n de la aplicaci�n del cuadro el�ctrico
}S_OUTPUTS_CUADRO_T3;

// Posibles estados enviados desde EtherCAT
#define BIT_MARCHA         0x01                    ///< La m�quina est� en marcha. ESTE DEBE SER SIEMPRE EL PRIMER BIT, YA QUE ES EL �NICO EST�TICO

// Estados PC -> ETHERCAT
typedef struct                                  {
 //  DWORD   MARCHA_SOFT;
 //  DWORD   PARO_SOFT;
   DWORD   ESTADO_SOFT;                            ///< Estado enviado DESDE la aplicaci�n
}S_INPUTS_CUADRO_T3;

// Posibles estados a enviar a EtherCAT
#define  FALLO_GRAVE          0x01                 ///< Fallo grave: Parpadeo r�pido luz ambar
#define  FALLO_LEVE           0x02                 ///< Fallo leve: Parpadeo lento luz ambar
#define  MARCHA_APP           0x04                 ///< Indicar al EtherCAT que hemos arrancado la m�quina, ya que ahora lo hacemos desde aqu�. Sirve para encender la luz verde
#define  MARCA_PEND_PERMEXT   0x08
#define  WATCHDOG             0x10

// Estructura que contendr� la configuraci�n de la comunicaci�n de alarmas al automata
typedef struct{
   int   sEstaMEnab,                               ///< Habilitaci�n del estado de marcha
         sEstaFEnab,                               ///< Habilitaci�n del estado de fallo
         eEstaMEnab,                               ///< Habilitaci�n entrada del estado de marcha
         eEstaFEnab;                               ///< Habilitaci�n entrada del estado de fallo
}s_cfgComAlar_T3;

// Objeto de control de I/O por bus EtherCAT

#ifdef _ISDLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif


class TStdElectricCabinetT3 : public TElectricCabinet , public TEtherCAT3        {

 public:
   TStdElectricCabinetT3( wchar_t *name, TDataManagerControl *parent);    ///< Constructor
   virtual ~TStdElectricCabinetT3();                                      ///< Destructor

   virtual bool  MyCheckDiagnose(e_diagnose_type type_diagnose, TBasicDiagnose *diag);

   //---- Funciones Inline
   // Comprueba si el driver est� funcionando
   inline bool IsOk()                                                         {
      return (IOConnected && inited);
   };
   virtual bool ArrancaCinta()                                                {
//      if (!CheckHayFallo())                                                   {
//         m_pArrancaCinta->SetAsInt(1);
//         return true;
//      }
//      else
//         return false;
   };
   virtual bool ArrancaAlimentacion()                                         {
//      if (!CheckHayFallo())                                                   {
//         m_pArrancaAliment->SetAsInt(1);
//         return true;
//      }
//      else
//         return false;
   };
      // Parar cinta
   virtual bool PararCinta()                                                  {
//      m_pArrancaCinta->SetAsByte(0);
//      return true;
   };
   // Parar alimnetaci�n
   virtual bool PararAlimentacion()                                           {
//      m_pArrancaAliment->SetAsByte(0);
//      return true;

   };

   virtual bool EstadoMarcha()                                               {
//       return m_stateMarcha;
    };


   virtual void IOOpen();
   virtual void SetSoftState( DWORD softState)                                {
      inCuadro.ESTADO_SOFT = softState;
//      SetCfg();
   }
   virtual DWORD GetCabinetState()                                            {
      return outCuadro.ESTADO;
   }

 protected:

   TBitStateMngr       *m_bitStateMngr;
   bool                 inited;                             ///< Configuraci�n inicializada
   TData               *daEstadoEtherCAT;

   int                  m_counterWatchdog;
   bool                 m_watchdog;

   // Control del cuadro el�ctrico
   S_INPUTS_CUADRO_T3      inCuadro;                        ///< Estados enviados HACIA la tarea de EtherCAT de control del cuadro
   S_OUTPUTS_CUADRO_T3     outCuadro;                       ///< Estados enviados DESDE la tarea de EtherCAT

   MMRESULT                idTimer_Cuadro;               ///< Handle del timer de la callback


   // Heredadas de TDataManager
   virtual bool Read( LT::TStream *stream, TDataInterface *intf = NULL, bool infoModif = true, bool *forceSave =NULL );

   // Callback de ejecuci�n de la tarea de IO
   // Tarea de control del cuadro el�ctrico
   static void CALLBACK TimerProc_Cuadro( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

   virtual bool   Update();
   virtual void   SetCfg();
   virtual void   Watchdog();
   virtual bool   SetTimers();
   virtual void   KillTimers();
   virtual bool   MyInit();

   void AddErrorText( std::wstring &errorText);

   friend class TFormWaitDrvT3;


};
//---------------------------------------------------------------------------

#endif




//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
