//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TEtherCATRT.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   TwinCat2
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TEtherCATRTH
#define TEtherCATRTH

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include "TDataMng.h"
#include "ADS\TcAdsDEF.h"
#include "ADS\TcAdsAPI.h"
#include "IO API\TCatIoApi.h" 	                           // header file shipped with TwinCAT® I/O
#include "TEtherCAT.h"
#include "TIORealTime.h"
//#include "exp.h"

#include "Driver_EtherCAT.h" 	                              // TwinCAT® System Manager generated
//#include "DlgWaitDriver.h"
#include "TDataInt64.h"
#include "TDataArrayOfBytes.h"

#include "common.h"
#include "defines.h"

//#include "userdefinesTDataMng.h"

 /// Faulty flag codes
#define FAULTY_ETHERCAT_NOT_CONNECTED     0x10
#define FAULTY_EJECTION_DISABLED          0x20
#define FAULTY_INDEX_TRIGG_ADVANCED       0x40
#define FAULTY_EXECUTE_TIME               0x80
#define FAULTY_IREAD_EQUALS_IWRITE        0x100
#define FAULTY_FAIL_PUTEXP                0x200
#define FAULTY_FAIL_ENCODER               0x400


//--- Definiciones de EtherCAT
// OJO: Los defines en PC tienen uno más que en el autómata
#define  BUFF_SIZE      0x4000               // Tamaño del buffer de expulsion en tiempo
#define  NUM_EXP        200                  // Numero de expulsiones
#define  NUM_STROB      10                   // Número de strobes
#define  MAX_EXP        256                   // Tamaño del buffer de poner expulsiones

#define MS_UPDATE       1                    // periodo de actualizacion en MILISEGUNDOS. Al aumentar este tiempo, aumentan las oscilaciones
#define MS_SYNC_CLOCK   2000                 // periodo de sincronizacion de reloj en MILISEGUNDOS
#define N_COUNTTIME     40
// Ponemos el alineamiento en 1 byte para que siempre cuadre con el PLC
#pragma pack(push,1)

#include "Trigger.h"

//--- Strobe
// Cfg
typedef struct                                                                {
   bool        enabled;                      // Habilitación
   BYTE        offsetTime;                   // Offset en tiempo del strobe respecto al trigger (solo valores positivos, para retrasar)
   BYTE        strobeTime,                   // Tirempo de disparo del strobe (según la precisión configurada)
               trigger,                      // Trigger alq ue nos resincronizacremos
               tamSec;                       // Tamaño de la secuencia de disparos
   BYTE        sequence[TAM_SEC];            // Secuencia de disparo de este strobe
   BYTE        strobeType,                   // Tipo de strobe (0-> Visible, 1 -> NIR, 2-> rayos X, resto sin definir
               reSync;                       // Con que resincronizamos LA SECUENCIA (nada, base de tiempos, fotocélula,...)
   void Init()                                                         {
      //init with deault values
      enabled     = false;
      offsetTime  = 0;
      strobeTime  = 0;
      trigger     = 0;
      tamSec      = 1;
      memset( sequence, 0x00, sizeof( sequence));
      strobeType  = 0;
      reSync      = 0;
   }
}S_STROB_CFG;

// Data
typedef struct                                                    {
   BYTE           set;                       // Contador para activar strobe durante el tiempo necesario
   BYTE           timeToSet;                 // Contador en tiempo hasta que se arranca para controlar el offset en tiempo de la señal de strobe
   bool           limitado;                  // Para limitar el funcionamiento del strobe ( Máx 10 ms y 4 veces apagado el tiempo encendido)
   BYTE           contOn,                    // Contador tiempo encendido
                  contOff;                   // Contador tiempo apagado
   ULARGE_INTEGER next;                      // Próximo disparo
   ULARGE_INTEGER last;                      // Último disparo
   ULARGE_INTEGER lastNextTrig;              // Último valor de next del trigger asociado con el que nos hemos sincronizado.
   BYTE           seqIndex;                  // indice por el que vamos de una secuencia
}S_STROB_DATA;

// Cfg y data
typedef struct                                                    {
   // Cfg
   S_STROB_CFG  cfg[NUM_STROB];
   // Data
   S_STROB_DATA data[NUM_STROB];
}S_STROB;

//--- BaseTime

// Posibles fuentes de pulsos
typedef enum                                                      {
   ENCODER_EL1012    = 0,
   TIMER             = 1,
   ENCODER_EL5151    = 2
}E_SYNC;


// Cfg BaseTime
typedef struct                                                    {
   BYTE      Sync;                         // Tipo de sincronismo
}S_BASETIME_CFG;

// Data BaseTime
typedef struct                                                    {
   BYTE           oSync;                    // Para comprobar si ha cambiado la fuente de tiempos y reiniciar
   ULARGE_INTEGER time100ns,                // Valor del contador de precisión de 100ns del etherCAT
                  counter;                  // Contador de la base de tiempos
   DWORD          timer;                    // Contador de ciclos de ethercat (antiguo timer), precisión segun configuración de divisores
   ULARGE_INTEGER lastReSync;               // Valor de counter de la última resincronización
   DWORD          timerLastReSync,          // Valor de timer de la última resincronización
                  TimeLastPulse;            // Tiempo sin pulsos de encoder o de timer
}S_BASETIME_DATA;

// Cfg y data
typedef struct                                                    {
   // Cfg
   S_BASETIME_CFG  cfg;
   // Data
   S_BASETIME_DATA data;
}S_BASETIME;

//-- PutExp
//

typedef struct                                                    {
   BYTE              i_w;                       // índice escritura
   S_EXP             buffer[MAX_EXP];           // buffer para ir poniendo expulsiones
}S_PUTEXP_CFG;

// Data
typedef struct                                                    {
   BYTE              i_r;                       // índice lectura
   BYTE             contOut[NUM_EXP];          // contador de ciclos de activacion de cada salida
}S_PUTEXP_DATA;

// Cfg y data
typedef struct                                                    {
   // Cfg
   S_PUTEXP_CFG  cfg;
   // Data
   S_PUTEXP_DATA data;
}S_PUTEXP;

#pragma pack(pop)

// Estructura que se lee del PLC
typedef struct                                                                {
   S_BASETIME           baseTime;                           // Cfg e info de la base de tiempos
   S_TRIG               trig;                               // info y cfg de trigger
   S_STROB              strob;                              // info y cfg de strobes
   S_PUTEXP             putExp;                             // Configuración para poner expulsiones
   // Cfg
   BYTE                 status;                             // Status del modulo. Si no está running no disparamos. En los cambios puede que reseteemos
   BYTE                 mode,                               // Modo de disparo de válvulas
                        divisorEncTrigStrob,                // Divisor de la frecuencia de la tarea de sincronización, trigger y strobe
                        divisorExp;                         // Divisor de las tareas de expulsión
   // Data
   WORD                 numTerminales,                      // Número de terminales conectados
                        falloTerminales,                    // 0 -> Terminales OK  x -> Primer fallo en terminal x
                        codeVersion;                        // Código de versión de programa, en formato "Yxx". Si Y es igual, se supone versiones compatibles
   bool                 masterOk;                           // Tarjeta ETherCAT KO / OK
}S_ETHINFO;

// Test Driver
typedef struct                                                                {
   double   pulsTimer,                                      // pulsos de timer
            pulsEncoder,                                    // pulsos de encoder
            pulsEncoderVC;                                  // pulsos de encoder de vuelta completa
   int      numTerminales,                                  // Número de terminales configurados
            falloTerminales;                                // 0 -> Terminales OK  x -> Primer fallo en terminal x
   bool     masterOk,                                       // Tarjeta EtherCAT KO / OK
            modoManual,                                     // Modo Manual de func. de cintas
            variadoresOk,                                   // Entrada de variadores
            emergenciaOk,                                   // Parada de emergencia
            permExtOk,                                      // Perm. externo de marcha
            termCuadroOk;                                   // Terminales del cuadro KO / OK
} s_test_driver;

//class TFormWaitDrv;

//typedef struct            {
//   __int64 cpuCount;
//   __int64 time100ns;
//   __int64 cpuFreq;
//} s_cpuCountTime;
//
//-----------------------------------------------------------------------------

// Objeto de control de I/O por bus EtherCAT
class TEtherCATRT : public TIORealTime , public TEtherCAT                      {

 public:
   TEtherCATRT( wchar_t *name, TProcess *parent);                         /// Constructor
   virtual ~TEtherCATRT();                                                     /// Destructor
   void AddErrorText( std::wstring &errorText);

   //---- Inheritance from TIORealTime
   /// Init the driver
   /**
      Used as the Init process. It starts the communication with the EtherCAT level
   */
   virtual bool  MyInit();
   virtual bool  MyGo();
   virtual bool  MyStop();
   virtual TProcessData *MyProcess( TProcessData *processData);
   virtual bool  MyCheckDiagnose(e_diagnose_type type_diagnose, TBasicDiagnose *diag);

   // Manual Mode tests
   virtual bool  SetMode( unsigned char mode);                      /// Change working mode
   virtual bool  PutTrigger(unsigned char trig);                    /// Activate trigger signal


   // GetCounter
   virtual __int64 TEtherCATRT::GetCounter( bool update=false)                 {
      if( update)
         Update( );
      return eth.baseTime.data.counter.QuadPart;
   };

   // Get ejection counter by ejector
   virtual BYTE TEtherCATRT::GetCounterEjection(int ejector,  bool update=false)                 {
      if( update)
         Update( );
      if( ejector < NUM_EXP)
         return eth.putExp.data.contOut[ejector];
      else
         return 0;
   };
   // GetZeroLast
   inline DWORD TEtherCATRT::GetZeroLast(bool update = false)                {
      if( update)
         Update( );
      return eth.baseTime.data.TimeLastPulse;
   };

   virtual __int64 TEtherCATRT::GetTime( bool update=false)                   {
      if( update)
         Update( );
      return eth.baseTime.data.time100ns.QuadPart;
   };
   virtual __int64 GetLastReSync(bool update=false)                           {
      if( update)
         Update( );
      return eth.baseTime.data.lastReSync.QuadPart;
   };

   virtual bool GetLastTrig( int numTrigger, __int64 tmax, s_captureRealTimeData  &capRTData);
   virtual void IOOpen();

   int FreeCellsInBuffer( int triggerNumber)                                                    {

      int ret;
      if ( eth.trig.data[triggerNumber].ctrl_i_w >= eth.trig.cfg[triggerNumber].ctrl_i_r )                          {
         ret = NTRIG - (  eth.trig.data[triggerNumber].ctrl_i_w - eth.trig.cfg[triggerNumber].ctrl_i_r) -1;
      }
      else           {
         ret = eth.trig.cfg[triggerNumber].ctrl_i_r -  eth.trig.data[triggerNumber].ctrl_i_w -1;
      }
      return ret;
   }

   unsigned int CheckEjectionPeriode()                                                    {
      return m_ejectionPeriode;
   }

   int EnabledBuffered()                                                      {
      return m_enableBufferedExp;
   }

   int CheckEjectionList( bool &late, bool &noroom);

   virtual void MyPeriodic( __int64 & time);

 protected:
   //variables relacionadas con la buffered ejection
//   HANDLE               m_mutexExp;
   int                  m_enableBufferedExp,       ///< enables working with buffer
                        m_ejectionPeriode;         ///< periode ( ms) for checking ejection list
   std::list<S_EXP>     m_listExp;                 ///< lista con las ordenes de expulsion pendientes de ser enviadas
   int                  m_PLCBufferSize;           ///< Tamaño del buffer de expulsion del PLC, en pulsos de encoder
   S_EXP                m_ejectionArray[MAX_EXP];  ///< array para extraer ordenes a enviar de la lista de pendientes. No tiene sentido crear un array mayor que el tamaño del buffer circular de intercambio
   //fin variables para buffered ejection

   LARGE_INTEGER        m_freq;
   int                  m_enableEjection;
   int                  m_unlock,
                        m_maxEjectFail,
                        m_numRepEjectFail,
                        m_maxTriggerError,         ///< Maximum value to show trigger error faulty
                        m_numRepTriggerError,      ///< Counter to show trigger error faulty
                        m_restartTwincatError,
                        m_timeSyncMs;
   __int64              m_lastFaultyTime,
                        m_lastSyncTime;
   bool                 m_protect;
   bool                 m_reading;
   int                  m_counterSyncTooLate;      ///< contador para limitar el número de eventos
   HANDLE               m_mutex2,                  ///< mutex para acceder a datos compartidos  (m_offset1970)
                        m_mutexIR[NTRIG];          ///< mutex para los indices de lectura de los trigges
                                                   ///< ya que lo podemos adelantar desde 2 threads
                                                   ///< diferentes: el del GetTrigger y el del temporizador
                                                   ///< que desbloquea en caso de que no hay hueco
   __int64              microsec1601To1970;        ///< microseconds from 1601 to 1970

   bool                 m_initializing;            // true during initializing process

   TData               *pEnableEjection;

   __int64              m_lastSystemTime,
                        m_lastEthTime;
   int                  m_validCount;

   // Datos de trigger
   int                 m_trigToConfig;             //number of trigger to show in interface
   TData               *pTrigToConfig,
                       *pTrigConfiguration;
   TDataManager        *mngrTrigger;


   // Datos de Strobe
   int                 m_strobToConfig;            //number of  strobe to config
   TData               *pStrobeToConfig,
                       *pStrobeConfiguration;
   TDataManager        *mngrStrobe;
   TDataArrayOfBytes<TAM_SEC>  *pStrobSequence;

   S_ETHINFO            eth;                                // I/O a intercambiar con el PLC
   // estructuras para la memoria compartida de los TDatas para visualizar en interface
   S_TRIG_CFG           trigCfg;
   S_TRIG_DATA          trigData;
   S_STROB_CFG          strobCfg;
   S_STROB_DATA         strobData;


   //--- Tareas de IO
   // Control de válvulas trigger y strobe
   PDriver_EtherCAT_Outputs   pOutputs;                     // Salidas
   PDriver_EtherCAT_Inputs    pInputs;                      // Entradas
   MMRESULT                   idTimerUpdate;                // Handle del timer de la callback de Actualización. Para obtener frecuentemente los datos de los últimos disparoa

   bool                       m_updating;
   bool                       m_dispatchControlEnabled;     //dispatch an event when trying to GetLasTrig for a trigger that has control not enabled
   __int64                    m_offset1970,                 // for synchronizing clocks between pc and EtherCAT. is number of hundreds of nanoseconds between our time ( microseconds from 1970) and ethercat time ( hundreds of nanoseconds from 1600)
                              m_SyncLastTime;               // tiempo cuando hicimos el último ajuste
   double                     m_SyncDeriva;                 // deriva de temporizadores

   __int64                    m_lastNoRoomPutExp,           ///< variable to control late putexp faulty.
                              m_lastLatePutExp;             ///< variable to control no room putexp faulty.

   int                        m_traceFile;                  ///< for testing purposes
   unsigned int               m_lastUpdate,                 ///< tiempo ultima actualizacion de datos desde el PLC
                              m_lastCheckPending;           ///< tiempo ultimo chequeo de la lista de expulsiones pendientes

   int   m_drvInstalled,                    ///< El driver se encuentra instalado
         m_trigHabil,                       ///< Trigger habilitado/deshabilitado
         m_origPulse,                       ///< Origen de pulsos encoder o timer
         m_modoExpul,                       ///< Modo de la expulsión automatica o manual
         m_minPulsEnc,                      ///< Mínimo número de pulsos de encoder
         m_maxPulsEnc,                      ///< Máximo número de pulsos de encoder
         m_numTerminales,                   ///< Número de terminales
         m_minPulsTimer,
         m_maxPulsTimer,
         m_triggerCicleTime,
         m_vueltaCompleta,                  ///< Habilitar/deshabilitar comprobación de vuelta completade encoder. Debe etar deshabilitado cuando la cfg de pulsos por vuewlta no coincide con la real (por ejemplo V20)
         m_pulsosPorVuelta;

   void TestDriver( s_test_driver &res, int time);

   bool  GetEthTimeFromSystemTime( __int64 systemTime,__int64 &ethTime);
   bool  GetSystemTimeFromEthTime( __int64 ethTime,   __int64 &systemTime);

   /// Inheritance from TDataManager
   virtual bool Read( LT::TStream *stream, TDataInterface *intf , bool infoModif, bool *forceSave  );

   // Notification and TData & TdataManager management
   void __fastcall DataModifiedPost( TData* da);

   int   PutExp( S_EXP *exp, int num, bool ejectImmediatly = false);

   virtual bool Update();
   virtual void SetCfg();                                          // Escribir configuración

   void ResetTasksAndRestartTwinCAT();
   void SyncTimers( __int64 perfCounter, __int64 ethTime);

   int MyPutExp( S_EXP *exp, int num, bool &late,  bool &noroom);
   void MyPostPutExp( int num, bool late, bool noroom);

   virtual bool SetTimers();
   virtual void KillTimers();

   // Callback de ejecución de la tarea de IO
   static void CALLBACK TimerProc( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

   //-- Inline functions
   // GetTriggerEnabled
   inline bool  TEtherCATRT::GetTriggerEnabled(int i)                         {
      return i < NUM_TRIG ? eth.trig.cfg[i].enabled : false;
   };
   // GetPulseOrigin
   virtual inline DWORD TEtherCATRT::GetPulseOrigin()                         {
      return eth.baseTime.cfg.Sync;
   };
   // SetPulseOrigin
   virtual inline void TEtherCATRT::SetPulseOrigin( int origin)               {
      eth.baseTime.cfg.Sync = origin;
   };
   // Get Mode
   virtual unsigned char GetMode()                                            {
      return eth.mode;
   };

   /**
   @see TProcess::MsTimeOut
   si esta activada la buffered ejection, entonces devolver el periodo especificado
   */
   int virtual MsTimeOut()                                                    {
      if ( (!m_enableBufferedExp) ||  ( GetMode() == MODE_MANUAL))
         return TIORealTime::MsTimeOut();
      return this->m_ejectionPeriode;
   }

   /**
   @see TProcess::TimeOut
   revisar la lista de expulsiones pendientes
   */
   virtual void TimeOut()                                                     {
      TIORealTime::TimeOut();
      DispatchPendingEjections();

   };
   /**
   DispatchPendingEjections
   si está activada la buffered ejection, actualizar y revisar la lista de expulsiones pendientes
   */
   void DispatchPendingEjections()                                            {
//      #ifdef _DEBUG
//      int t = timeGetTime();
//      FILE *fitx=fopen( "ejection.txt", "a");
//      fprintf( fitx, "DispatchPendingEjections: time = %d\n", t);
//      fclose(fitx);
//      #endif

      if ( (!m_enableBufferedExp) ||  ( GetMode() == MODE_MANUAL) || Status()!= running)
         return;
      //actualizar datos desde el PLC
      Update();
      bool late = false,
            noroom = false;
      //revisar la lista de espulsiones pendientes y tratar de enviar las que proceda
      int num = CheckEjectionList( late, noroom);
      //si se ha escrito alguna orden en el buffer de intercambio, enviar datos al PLC
      //, o si ha habido algun problema, despachar logs
      MyPostPutExp( num, late, noroom);
   }

//      #ifdef _DEBUG
//      int t = timeGetTime();
//      FILE *fitx=fopen( "ejection.txt", "a");
//      fprintf( fitx, "DispatchPendingEjections: time = %d\n", t);
//      fclose(fitx);
//      #endif

//   friend class TFormWaitDrv;
};
//---------------------------------------------------------------------------

#endif





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
