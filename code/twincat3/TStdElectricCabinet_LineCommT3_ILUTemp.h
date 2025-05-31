//---------------------------------------------------------------------------
// Modulo:        TEtherCAT.h
// Comentario:
//
//---------------------------------------------------------------------------

//#ifndef TStdElectricCabinet_LineCommT3_ILUTempH
//#define TStdElectricCabinet_LineCommT3_ILUTempH

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>

#include "ADS\TcAdsDEF.h"
#include "ADS\TcAdsAPI.h"

#include "TDataMng.h"
#include "TDataMngControl.h"
#include "THistZone.h"

#include "TEtherCAT3.h"
#include "TBitState.h"
#include "TStdElectricCabinet_LineComm_Base.h"
//#include "IO API\TCatIoApi.h"                               // header file shipped with TwinCAT® I/O
//#include "Cuadro_Electrico_LineComm.h"                      // TwinCAT® System Manager generated


#ifndef _MSC_VER 
#include "TConditionsLineCommForm.h"
#else
#include "TConditionsLineCommFormQt.h"
#endif


 /// Faulty flag codes
#define FAULTY_ETHERCAT_NOT_CONNECTED_STDBOARD     0x10                   //bit 0 of flagsArray
#define FAULTY_ILU_TEMP_IS_TOO_LOW                 0x100
#define FAULTY_ILU_TEMP_IS_TOO_HIGHT               0x200
#define FAULTY_HOPPER_LEVEL_EXCEED                 0x400
#define FAULTY_LOW_SUPPLY_PRESSURE                 0x800


//***** Estructuras y defines para el control del cuadro eléctrico
// Estados ETHERCAT -> PC
typedef struct                                 {
   DWORD                ESTADO,                             // Variable con los posibles estados enviados A la aplicación
                        CODE_VERSION;                       // Versión de la aplicación del cuadro eléctrico
   float                TEMPERATURA_ILU_1,				      // Temperatura pórtico de iluminación 1 
                        TEMPERATURA_ILU_2;				      // Temperatura pórtico de iluminación 2 
   unsigned short     SIGNAL_SWICH_HOPPER_LEVEL;          // Señal conmutada fotocelula nivel tolva entrada producto
   unsigned short     SIGNAL_ANALOG_HOPPER_LEVEL;         // Señal analogica fotocelula nivel tolva entrada producto
   unsigned short     SIGNAL_ANALOG_AIR_PRESSURE;         // Señal analogica modulo medición presión aire

}S_OUTPUTS_CUADRO_COMM_T3_ILUTEMP;

// Posibles estados enviados desde EtherCAT
#define BIT_MARCHA         0x01                             // La máquina está en marcha. ESTE DEBE SER SIEMPRE EL PRIMER BIT, YA QUE ES EL ÚNICO ESTÁTICO
#define BIT_PARO           0x80000000                       // La máquina está en Pparo. ESTE DEBE SER SIEMPRE EL ULTIMO BIT

// Estados PC -> ETHERCAT
typedef struct                                  {
   DWORD                ESTADO_SOFT;                        // Estado enviado DESDE la aplicación
   bool                 DIGITAL_OUTPUT_1,
                        DIGITAL_OUTPUT_2,
                        DIGITAL_OUTPUT_3;
   DWORD                ANALOG_OUTPUT;

                                                            // Indica si la máquina tiene control de temperatura de iluminación.
   BYTE                  CTRL_TEMP_ILU;			               // 0->no hay control de T
                                                            // 1->un control de T
                                                            // 2->dos controles de T

}S_INPUTS_CUADRO_COMM_T3_ILUTEMP;

// Posibles estados a enviar a EtherCAT
#define  FALLO_GRAVE          0x01                          // Fallo grave: Parpadeo rápido luz ambar
#define  FALLO_LEVE           0x02                          // Fallo leve: Parpadeo lento luz ambar
#define  MARCHA_APP           0x04                          // Indicar al EtherCAT que hemos arrancado la máquina, ya que ahora lo hacemos desde aquí. Sirve para encender la luz verde
#define  MARCA_PEND_PERMEXT   0x08
#define  WATCHDOG             0x10

// Estructura que contendrá la configuración de la comunicación de alarmas al automata
typedef struct{
   int   sEstaMEnab,       // Habilitación del estado de marcha
         sEstaFEnab,       // Habilitación del estado de fallo
         eEstaMEnab,       // Habilitación entrada del estado de marcha
         eEstaFEnab;       // Habilitación entrada del estado de fallo
}s_cfgComAlar_comm_T3_ILUTEMP;



#define PRESSURE_FACTOR_CALIBRATION        1000


// Objeto de control de I/O por bus EtherCAT

#ifdef _ISDLL
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif

class TConditionsLineCommForm;
class TConditionsMngr;


class DLL TStdElectricCabinet_LineCommT3_ILUTemp : public TStdElectricCabinet_LineComm_Base, public TEtherCAT3        {

 public:
   TStdElectricCabinet_LineCommT3_ILUTemp( wchar_t *name, TDataManagerControl *parent);    // Constructor
   virtual ~TStdElectricCabinet_LineCommT3_ILUTemp();                                      // Destructor

   virtual bool  MyCheckDiagnose(e_diagnose_type type_diagnose, TBasicDiagnose *diag);

   //---- Funciones Inline
   // Comprueba si el driver está funcionando
   inline bool IsOk()                                                         {
      return (IOConnected && inited);
   };
   virtual void IOOpen();
   virtual void SetSoftState( DWORD softState)                                {
      inCuadro.ESTADO_SOFT = softState;
   }
   virtual DWORD GetCabinetState()                                            {
      return outCuadro.ESTADO;
   }

 protected:

   TDataManager         *m_daTempMngr,
                        *m_daHopperMngr,
                        *m_daPressureMngr;

   TBitStateMngr        *m_bitStateMngr;
   bool                 inited;                                ///< Configuración inicializada
   TData                *daEstadoEtherCAT;

   int                  m_counterWatchdog;
   bool                 m_watchdog;

	bool						event_temp,
                        event_hopper,
                        event_min_pressure;

   float                m_maxTemp,
                        m_minTemp,
                        m_levelNormalized,
                        m_inputPressure,
                        m_minPressure;

   int                  m_maxValLevel,
                        m_minValLevel,
                        m_percMaxLevel,
                        m_percMinLevel,
                        m_levelPercentage,
                        m_maxValPressTest;


   // Control del cuadro eléctrico
   S_INPUTS_CUADRO_COMM_T3_ILUTEMP      inCuadro;                              ///< Estados enviados HACIA la tarea de EtherCAT de control del cuadro
   S_OUTPUTS_CUADRO_COMM_T3_ILUTEMP     outCuadro;                             ///< Estados enviados DESDE la tarea de EtherCAT

   MMRESULT                   idTimer_Cuadro;                  ///< Handle del timer de la callback
   MMRESULT                   m_idTimer_checkStates;           ///< Handle del timer de la callback



   // Callback de ejecución de la tarea de IO
   // Tarea de control del cuadro eléctrico
   static void CALLBACK TimerProc_Cuadro     ( UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
   static void CALLBACK TimerProc_CheckStates( UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

   virtual bool   Update();
   virtual void   SetCfg();
   virtual void   Watchdog();
   virtual bool   SetTimers();
   virtual void   KillTimers();
   virtual bool   MyInit();
   virtual void   CheckState();
   virtual void   DataModifiedPost( TData *da);
   virtual void   MyPeriodic(__int64 & time);               ///< @see TDataManagerControl::MyPeriodic.
   void           AddErrorText( std::wstring &errorText);

   // -- Miscellaneous
   void ConvertStateToDigitalAnalog();
   void IlluminationTemperatureControl();                   ///< Control for temperature illumination.
   void HopperLevelControl();                               ///< Control for fotocel hopper level control
   void PressureControl();                                  ///< Control for temeprature illumination.


   friend class TFormWaitDrvT3;
};
//---------------------------------------------------------------------------

//#endif
