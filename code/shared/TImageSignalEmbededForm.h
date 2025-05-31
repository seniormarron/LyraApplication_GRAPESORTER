//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TImageSignalEmbededFormH
#define TImageSignalEmbededFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Chart.hpp"
#include "Series.hpp"
#include "TeEngine.hpp"
#include "TeeProcs.hpp"
#include <ExtCtrls.hpp>


#include "TData.h"
#include "TImg.h"
#include "TViewInterface.h"
#include "TImageSignalForm.h"
#include <ImgList.hpp>
#include <Buttons.hpp>

#define LB_SIGNAL_NAME L"Signal_"
#define MAX_SIGNALS        10
#define MAX_NUMBER        9999

//forward declaration
class  TImageSignalEmbededForm;
class TMyMachineModuleMngr;
class TImageSignalEmbededMngr;


/******************************************************************************
                               TImageSignalViewerEmbededFormInterface
*******************************************************************************/


//------------------------------------------------------------------------------
  class TImageSignalEmbededFormInterface : public TViewInterface{
 public:
   __fastcall TImageSignalEmbededFormInterface(TDataManager *mngr, TForm *form);
   virtual ~TImageSignalEmbededFormInterface();
   virtual bool   RefreshInterface ( TNode *tdata);                             ///< Updates changes of Tdata in a view.

   void calculateInfo();
   void ResetSignals();
   int getAverage(int serie, int x1, int x2);
   int getDeviation(int average, int serie, int x1, int x2);
   int getMaximum(int serie, int x1, int x2);
   int getMinimum(int serie, int x1, int x2);

 protected:
   virtual bool  GetValAsText            ( TComponent *c, wchar_t *text, int size, TData *data);  ///< Gets value from a control in a view.

   TImageSignalEmbededForm     *m_form;
   signalAndIndex        m_signalAndIndex[MAX_SIGNALS];

};

//---------------------------------------------------------------------------

/******************************************************************************
                               TImageSignalViewerEmbededForm
*******************************************************************************/


//------------------------------------------------------------------------------
/**
*  @class TImageSignalViewerEmbededForm
*  Form to adjust the image signal. This form show the signal in the
*  Chart and we can see the red, green, blue, ... signal.
*/

//---------------------------------------------------------------------------
class TImageSignalEmbededForm : public TView {
__published:	// IDE-managed Components
    TEdit *x1;
    TEdit *x2;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label4;
    TLabel *Label5;
   TEdit *deviation;
   TEdit *average;
    TLabel *Label6;
    TLabel *Label7;
   TEdit *maximum;
   TEdit *minimum;
   TComboBox *Signls;
   TLabel *Label8;
	TSpeedButton *BtnClose;
	TCheckBox *gridY;
	TCheckBox *gridX;
	TChart *Chart1;
	TEdit *HistoricMin;
	TEdit *HistoricMax;
	TLabel *Label9;
	TLabel *Label10;
	TSpeedButton *BtnReset;
	TPanel *Panel1;

   void __fastcall OnChange(TObject *Sender);
   void __fastcall CloseBtn__Click(TObject *Sender);
   void __fastcall gridXClick(TObject *Sender);
   void __fastcall gridYClick(TObject *Sender);
	void __fastcall BtnResetClick(TObject *Sender);
private:	// User declarations

   signalAndIndex        m_signalAndIndex[MAX_SIGNALS];
   TLineSeries* 		 m_series[MAX_SIGNALS];
//   TMyMachineModuleMngr       *m_modMngr;          ///< TMyMachineModuleMngr used to store cfg after deletion
//   TImageSignalFormInterface  *m_ViewInterface;    ///< interface
//   TPanel                     *m_panelToDock;      ///< panel to dock at the form
   int                        numSeries;
public:		// User declarations
    __fastcall TImageSignalEmbededForm(TComponent* Owner, TDataManager *mn);
    __fastcall ~TImageSignalEmbededForm();
//    void AddManager( TDataManager *mn)                                         {
//      if ( m_ViewInterface)                                                   {
//         m_ViewInterface->Add( mn);
//         m_ViewInterface->RefreshAll( mn);
//      }
//   }
//   void  SetShowImg(TImageSignalMngr* showImg);
   void Add(WORD* data,int size,  UnicodeString name );
   void Clear();
   int getSignalSelected();
   int getMax();
   int getMin();
//   void Refresh();
   void setAverage(int x);
   void setDeviation(int x);
   void setMin(int x);
   void setMax(int x);
   void setHistoricMin(int x);
   void setHistoricMax(int x);
   void setScaleDepth(int x);
   void SetChart(TChart *chart);
//   protected:
//   void __fastcall MyWndProc(Messages::TMessage &Message);

};
//---------------------------------------------------------------------------
extern PACKAGE TImageSignalEmbededForm *ImageSignalEmbededForm;
//---------------------------------------------------------------------------
#endif






//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
