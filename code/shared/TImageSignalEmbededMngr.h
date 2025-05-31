//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TImageSignalEmbededMngrH
#define TImageSignalEmbededMngrH

#include "TDataMngControl.h"
#include "TViewInterface.h"
#include "TImageSignalEmbededForm.h"
#include "TProcess.h"
#include "common.h"

#define MAX_SIGNALS    10

//forward declaration
class TImg;
class TModuleMngr;
class TCapturer;

/******************************************************************************
                               TImageSignalEmbededMngr
*******************************************************************************/

//------------------------------------------------------------------------------
/**
*  @class TImageSignalEmbededMngr
*  Form to adjust the image signal. This form show the signal in the
*  Chart and we can see the red, green, blue, ... signal.
*/

//---------------------------------------------------------------------------
class TImageSignalEmbededMngr : public TProcess                                      {

 protected:

   TImageSignalEmbededForm  *m_signalForm;


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
   TImageSignalEmbededMngr(const wchar_t*name, TProcess *parent);
   virtual ~TImageSignalEmbededMngr();                                           ///< Default destructor
   void SetChart(TChart *chart);
};
//---------------------------------------------------------------------------
extern PACKAGE TImageSignalEmbededMngr *ImageSignalEmbededMngr;
//---------------------------------------------------------------------------
#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
