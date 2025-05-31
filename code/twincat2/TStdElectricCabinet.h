//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TStdElectricCabinet.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   TwinCat2
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TStdElectricCabinetH
#define TStdElectricCabinetH

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>

#include "TEtherCAT.h"
#include "TDataMngControl.h"
#include "TDataMng.h"

#include "ADS\TcAdsDEF.h"
#include "ADS\TcAdsAPI.h"
#include "IO API\TCatIoApi.h"

#include "Cuadro_Electrico.h"
#include "TBitState.h"
#include "TElectricCabinet.h"


 /// Faulty flag codes
#define FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD 0x10


// Estados ETHERCAT -> PC
typedef struct                                  {
   DWORD                ESTADO,                       // Variable con los posibles estados enviados A la aplicación
                        CODE_VERSION;                 // Versión de la aplicación del cuadro eléctrico
   bool                 Llave_permiso;

}S_OUTPUTS_CUADRO;

// Estados PC -> ETHERCAT
typedef struct                                  {
   DWORD                ESTADO_SOFT;                  // Estado enviado DESDE la aplicación

   bool                 Arranca_cinta,
                        Para_cinta,
                        Arranca_alimentacion;

}S_INPUTS_CUADRO;


// Posibles estados enviados desde EtherCAT
#define BIT_MARCHA         	0x01                       // La máquina está en marcha. ESTE DEBE SER SIEMPRE EL PRIMER BIT, YA QUE ES EL ÚNICO ESTÁTICO
#define FALLO_EMERGENCIA   	0x02                       // Fallo paro de emergencia.
#define FALLO_PERM_EXTERNO 	0x04                       // Fallo habilitación permiso externo.
#define FALLO_NO_HAY_LLAVE 	0x08                       // Fallo habilitacion llave analisis.
#define FALLO_ETHERCAT 		0x10                          // Fallo Ethercat.
#define BIT_PARO           	0x80000000                 // La máquina está en paro. ESTE DEBE SER SIEMPRE EL ULTIMO BIT.

#define FALLO_MASK         0x00FF

// Posibles estados a enviar a EtherCAT
#define  FALLO_GRAVE          0x01                    // Fallo grave: Parpadeo rápido luz ambar
#define  FALLO_LEVE           0x02                    // Fallo leve: Parpadeo lento luz ambar
#define  MARCHA_APP           0x04                    // Indicar al EtherCAT que hemos arrancado la máquina.
#define  MARCA_PEND_PERMEXT   0x08
//#define  NEXT...             0x10



//------------------------------------------------------------------------------
/*
   TStdElectricCabinet
*/
class TStdElectricCabinet : public TElectricCabinet , public TEtherCAT        {

 public:

   TStdElectricCabinet( wchar_t *name, TDataManagerControl *parent);    // Constructor
   virtual ~TStdElectricCabinet();                                      // Destructor

   virtual bool  MyCheckDiagnose(e_diagnose_type type_diagnose, TBasicDiagnose *diag);

   //-- Inline functions
   inline bool IsOk()                                                         {
      return (IOConnected && inited);
   };


   // Arrancar cinta
   virtual bool ArrancaCinta()                                                {
	  if (!CheckHayFalloNoArranque())                                         {
         m_pArrancaCinta->SetAsByte(1);    // accion pulsador
         return true;
      }
      else
         return false;
   };
   // Parar cinta
   virtual bool PararCinta()                                                  {
      m_pParaCinta->SetAsByte(1);         // accion pulsador
      return true;
   };

   // Arrancar alimentador
   virtual bool ArrancaAlimentacion()                                         {
	  if (!CheckHayFalloNoArranque())                                         {
         m_pArrancaAliment->SetAsByte(1);   // accion interruptor
         return true;
      }
      else
         return false;
   };
   // Parar alimnetación
   virtual bool PararAlimentacion()      {   // accion interruptor
      m_pArrancaAliment->SetAsByte(0);
      return true;

   };

    virtual bool EstadoMarcha()                                               {
       return m_stateMarcha;
    };

   virtual void IOOpen();
   virtual void SetSoftState( DWORD softState)                                {
      inCuadro.ESTADO_SOFT = softState;
   }
   virtual DWORD GetCabinetState()                                            {
      return outCuadro.ESTADO;
   }

 protected:

   TDataByte            *m_pArrancaCinta,
                        *m_pParaCinta,
                        *m_pArrancaAliment;


   TBitStateMngr       *m_bitStateMngr;
   bool                 inited;                             // Configuración inicializada
   TData               *daEstadoEtherCAT;

   bool                 m_stateMarcha;

   // Control del cuadro eléctrico
   S_INPUTS_CUADRO      inCuadro;                           // Estados enviados HACIA la tarea de EtherCAT de control del cuadro
   S_OUTPUTS_CUADRO     outCuadro;                          // Estados enviados DESDE la tarea de EtherCAT

   //--- Tareas de IO
   // Control de cuadro eléctrico
   PCuadro_Electrico_Outputs  pOutputs_Cuadro;              // Salidas
   PCuadro_Electrico_Inputs   pInputs_Cuadro;               // Entradas
   MMRESULT                   idTimer_Cuadro;               // Handle del timer de la callback

   static void CALLBACK TimerProc_Cuadro( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

   virtual bool Read( LT::TStream *stream, TDataInterface *intf = NULL, bool infoModif = true, bool *forceSave =NULL );
   virtual bool   Update();
   virtual void   SetCfg();
   virtual bool   SetTimers();
   virtual void   KillTimers();
   virtual bool   MyInit();

   int   CheckHayFalloNoArranque();
   void  AddErrorText( std::wstring &errorText);
   void __fastcall DataModifiedPost( TData* da);

   friend class TFormWaitDrv;



};
//---------------------------------------------------------------------------

#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
