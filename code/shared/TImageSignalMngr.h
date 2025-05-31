//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TImageSignalMngr.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TImageSignalMngrH
#define TImageSignalMngrH

#include "TDataMngControl.h"
#include "TViewInterface.h"
#include "TImageSignalForm.h"
#include "TProcess.h"
#include "common.h"

#define MAX_SIGNALS    10

//forward declaration
class TImg;
class TModuleMngr;
class TCapturer;

/******************************************************************************
                               TImageSignalMngr
*******************************************************************************/

//------------------------------------------------------------------------------
/**
*  @class TImageSignalMngr
*  Form to adjust the image signal. This form show the signal in the
*  Chart and we can see the red, green, blue, ... signal.
*/

//---------------------------------------------------------------------------
class TImageSignalMngr : public TProcess                                      {

 protected:

   TImageSignalForm  *m_signalForm;


   TImg              *m_img;
   TData              *m_daSignlsComboBox,
                     *m_daCloseButton,
                     *m_daSignalsList,
                     *m_daScaleDepth,
                     *m_x1,
                     *m_x2,
//                   *m_deviation,
//                   *m_average,
//                   *m_maximum,
//                   *m_histMaximum,
//                   *m_histMinimum,
//                   *m_minimum,
                     *m_daShowForm,
                     *m_daClear,
                     *m_daReset,
                     *m_daResetBtn,
                     *m_daCalculate,
                     *m_daThresholdMs,
                     **m_daSignal;

   bool              m_closed;

   std::list<std::wstring> m_tokensDestinations;

   wchar_t           m_daSignals[512],
                     m_daSignalsCombo[16];
   __int64           m_lastTime;

   virtual bool MyInit();
   virtual bool MyGo();
   virtual bool MyStop();
   virtual TProcessData *MyProcess( TProcessData *processData);            ///< @see TProcess::MyProcess
   void __fastcall            DataModifiedPost(TData* da);                 ///< Event Handler method for Control of modified data


   void ShowFormImageSignal();


public:                                                                    ///< User declarations

   void calculateInfo();
   void SetClosed(bool aux);
   TImageSignalMngr(const wchar_t*name, TProcess *parent);
   virtual ~TImageSignalMngr();                                           ///< Default destructor

};
//---------------------------------------------------------------------------
extern PACKAGE TImageSignalMngr *ImageSignalMngr;
//---------------------------------------------------------------------------
#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
