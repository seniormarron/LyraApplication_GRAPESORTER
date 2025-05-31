
#ifndef TriggerH
#define TriggerH

#define  NTRIG          8                    ///< Control trigger buffer size
#define  TAM_SEC        8                    ///< Max sequences size
#define  NUM_TRIG       8                    ///< Num of trigger available in the system

// Change alignment to byte to match twincat filesystem
#pragma pack(push,1)


//-------------------------------------------------------------------------
//--- Trigger
// Cfg
typedef struct                                                    {
   bool        enabled;                      ///< Enabled
   BYTE        reSync;                       ///< Resync Source (None, BaseTime, photocel,....)
   DWORD       cycleTime;                    ///< Time (0.01 ms) betwwen triggers in automatic mode
   BYTE        triggerTime;                  ///< Activation time of the trigger (independent from the strobes one)
   BYTE        offsetPulse;                  ///< Offset between trigger and resync event
   BYTE        offsetTime;                   // Offset en tiempo del trigger respecto al strobe (solo valores positivos, para adelantar el strobe)
   bool        invert,                       ///< Invert
               putTrigger,                   ///< Puttrigger manual
               ctrlEnabled;                  ///< Enable control trigger buffer
   BYTE        ctrl_i_r;                     ///< Read index of control trgger buffer
   void Init()                                                         {
      //Init with default values
      enabled     = false;
      reSync      = 0;
      cycleTime   = 3000;
      triggerTime = 10;
      offsetPulse = 0;
      offsetTime  = 0;
      invert      = false;
      putTrigger  = false;
      ctrlEnabled = false;
      ctrl_i_r    = 0;
   }
}S_TRIG_CFG;

// Data
typedef struct                                                    {
   BYTE           set;                       ///< Counter to control the activation of each trigger in time (not pulses)
   BYTE           timeToSet;                 // Contador en tiempo hasta que se arranca para controlar el offset en tiempo de la señal de trigger
   ULARGE_INTEGER next;                      ///< Next trigger
   ULARGE_INTEGER last;                      ///< Last trigger
   DWORD          totalCycles,               ///< Total num of activation of this trigger
                  cycles;                    ///< Total num of activation of this trigger since last ReSync event
   ULARGE_INTEGER lastSinc[NTRIG];           ///< Buffer with lasts triggers counter value (control trigger buffer)
   ULARGE_INTEGER lastTime[NTRIG];           ///< Buffer with lasts triggers high-res time value (control trigger buffer)
   DWORD          frameCount[NTRIG];         ///< Buffer with lasts framecount value (control trigger buffer)
   BYTE           lastStrobe[NTRIG];         ///< Buffer with lasts strobes triggered (control trigger buffer)
   BYTE           ctrl_i_w;                  ///< Write index of control trgger buffer
}S_TRIG_DATA;

// Cfg y data
typedef struct                                                    {
   // Cfg
   S_TRIG_CFG  cfg[NUM_TRIG];
   // Data
   S_TRIG_DATA data[NUM_TRIG];
}S_TRIG;
#pragma pack(pop)

#endif