//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      LTAISForm.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------





#include <windows.h>
#include <vcl.h>
#include <dbt.h>

#pragma hdrstop

#include "LTAISForm.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvSmoothListBox"
#pragma link "Chart"
#pragma link "TeEngine"
#pragma link "TeeProcs"
#pragma link "AdvSmoothButton"
#pragma link "AdvSmoothImageListBox"
#pragma link "AdvSmoothListBox"
#pragma link "GDIPPictureContainer"
#pragma link "AdvSmoothSplashScreen"
#pragma link "AdvSmoothSlideShow"
#pragma link "AdvSmoothDock"
#pragma link "AdvSmoothGauge"
#pragma link "AdvSmoothTrackBar"
#pragma link "AdvSmoothLabel"
#pragma link "AdvSmoothPanel"
#pragma link "AdvSmoothStatusIndicator"
#pragma link "DBChart"
#pragma link "Series"
#pragma link "AdvSmoothToggleButton"
#pragma link "AdvSmoothComboBox"
#pragma link "TeeData"
#pragma resource "*.dfm"

#include <Keyboard.hpp>
#include <IOUtils.hpp>

#include "TMyMachineModuleMngr.h"
#include "TVisualizerMngrDouble.h"
#include "TVisualizerInFormDouble.h"
#include "THardlockChecker.h"
#include "TCapturer.h"
#include "process.h"
#include "TModule.h"
#include "logIn.h"
#include "TEventDispatcher.h"
#include "TImgPlane.h"
#include "TGridInterfaceUser.h"
#include "TAuxGraph.h"
#include "TZoneClassifier.h"
#include "TProdGraphInterface.h"
#include "TAuxHistCatInstantaneous.h"
#include "TAuxValveCycles.h"
#include "userdefinesFlags.h"
#include "LTInit.h"
#include "TCapturerMngr.h"
#include "TAux.h"
#include "TSaver.h"
#include "TSaveCfgDlg.h"
#include "TLoadCfgDlg.h"
#include "LTAIS_version.h"
#include "definesLabels.h"
#include "TEjectorTest.h"
#include "mastermind_num_version.h"
#include "TImageSignalEmbededMngr.h"
#include "TCounterByCategory.h"

// Forms
#include "TypeMngForm.h"
#include "TShowMsgForm.h"
#include "ExtractDeviceForm.h"


long timeNoAction    = 0;
bool locked          = false;

TFormLT *FormLT;
#define LBL_NO_TYPES             L"No Types"
#define LBL_NO_USERS             L"No Users"

#include "TClassifierInterface.h"

// ---------------------------------------------------------------------------
__fastcall TFormLT::TFormLT(TComponent* Owner) : TForm(Owner)                 {


   // Timers
   m_timerSpeed = new TTimer(NULL);
   if (m_timerSpeed)                                                          {
      m_timerSpeed->Enabled  = false;
      m_timerSpeed->Interval = 500;
      m_timerSpeed->OnTimer  = OnTimerSpeedButton;
   }


   m_incrTimer        = 1;
   m_v_incrTimer      = 0;


   EventsBt->Text = "";

   // Intro Splah characteristics
   IntroSplash->BeginUpdate();
   IntroSplash->BasicProgramInfo->AutoLoad = true;
   IntroSplash->BasicProgramInfo->ProgramVersion->Text = "";
   IntroSplash->BasicProgramInfo->CopyRight = "";
   IntroSplash->BasicProgramInfo->ProgramName->ColorStart = clTeal;
   IntroSplash->BasicProgramInfo->ProgramName->ColorEnd = clTeal;
   IntroSplash->BasicProgramInfo->ProgramName->Text = "Lyra Techs Artificial Intelligence Software";
   IntroSplash->EndUpdate();
   IntroSplash->Show();

   m_closing                     = false;
   m_checkHardlockAttempts       = 0;


   m_prevEstadoMarcha            = false;
//   TimerControlMaquina->Enabled  = true;

   m_formAdjustColorAux = NULL;

   m_pieSeries = new TPieSeries(Chart3);

   Chart3->AddSeries(m_pieSeries);

   // Configurar opciones de la gráfica
   m_pieSeries->Circled = true;  // Mostrar gráfica circular
   //m_pieSeries->ExplodedSlice[0] = true;  // Resaltar el primer sector (opcional)

   m_pieSeries->Add(0, "Aceptado", clGreen);  // Sector "Aceptado" con color verde
   m_pieSeries->Add(0, "Rechazado", clRed);  // Sector "Rechazado" con color rojo

}
//------------------------------------------------------------------------------
__fastcall TFormLT::~TFormLT()                                                {

   if ( m_timerSpeed)
      delete m_timerSpeed;

   if ( m_auxHistCatInstantaneous)
      delete m_auxHistCatInstantaneous;

   if ( m_auxValveCycles)
      delete m_auxValveCycles;
}
// ------------------------------------------------------------------------------
void __fastcall TFormLT::MyWndProc(Messages::TMessage &Message)               {

   MSG msg;

   WndProc(Message);

   // hardlock
   if (Message.Msg == WM_DEVICECHANGE)                                        {
	  // Este evento no está en la ayuda pero si en la API. Se produce varias veces
      // al conectar/desconectar cualquier dispositivo USB/1394. Lo atendemos la
      // segunda vez que siempre ya están disponibles todos los dispositivos
      if (Message.WParam == DBT_DEVNODES_CHANGED) {

//         THardlockChecker::CheckDevnodes();
//         s_sharedPermissions permissions = THardlockChecker::GetPermissions();
//
//         if ((permissions.machineHardlock == false && permissions.machineHardlockNeeded == true)    ||
//             (permissions.specialHASPhardLock == 0  && permissions.machineHardlockNeeded == false)) {
//            // if check failed we activate the checking timer
//            //Timer_HardLockCheck->Enabled = true;
//         }
//         else                                                                 {
//            //Timer_HardLockCheck->Enabled = false;
//            m_checkHardlockAttempts      = 0;
//            LOG_INFO1(L"MyWndProc HardLock Checked");
//            if (m_modMngr)                                                    {
//               m_modMngr->UpdateAccessLevel( false);
//            }
//         }
      }
   }

   try                                                                        {
      if (Message.Msg == msg_paint_drawable)                                  {
         TPainter *painter = dynamic_cast<TPainter*>((TNode*)Message.WParam);
		 if (painter)
            painter->Paint((TNode*) Message.LParam);
      }
      else if (Message.Msg == msg_refresh_interface || Message.Msg == msg_refresh_interface_all)
         ProcessRefreshMessage( this->Handle,  Message);

      else if (Message.Msg == msg_paint_maingraph)                            {
         if( m_modMngr)
            m_modMngr->UpdateGraphs();
      }
   }
   catch (Exception &exe)                                                     {
      LOG_ERROR1(exe.Message.c_str());
   }
}

//-----------------------------------------------------------------------------
void __fastcall TFormLT::FormShow(TObject *Sender)                            {

   if (m_persistence && (m_persistence->GetMachineCodeValidated()==false))    {
	  // stop machine
	  if (m_module)
		 m_module->Stop();

	  exit(EXIT_FAILURE);
   }

   if (m_module->Start())                                                     {

      IntroSplash->BeginUpdate();
      IntroSplash->ProgressBar->Position = 90;
      IntroSplash->EndUpdate();
      IntroSplash->BeginUpdate();
      IntroSplash->ProgressBar->Position = 100;
      IntroSplash->EndUpdate();
      IntroSplash->Hide();
   }

   DISPATCH_EVENT(INFO_CODE, L"EV_KERNEL_APP_INIT", L"Starting application.", m_module->GetMngr());
   LOG_INFO2( LoadText(L"EV_KERNEL_APP_INIT"), L"Starting application." );

   SetFocus();

   BringToFront();


   // Put the application version in clTeal color when work in debug mode
#ifdef _DEBUG
   Version->Font->Color = clTeal;
#endif

   if (m_modMngr)                                                             {
      // --Acciones en MyMachine()

      // Titulo aplicacion segun modelo de maquina configurado
      int machineModel = m_modMngr->GetMachineModel();
//      if (machineModel==HYPERION)                                             {
//         Img_LogoHYPERION->Show();
//         Logo_TEALABS->Hide();
//      }
//      else if (machineModel==TEALABS)                                         {
//         Img_LogoHYPERION->Hide();
//         Logo_TEALABS->Show();
//      }

	  if (machineModel==GRAPE_SORTER)                                         {
		 Image_Logo_GrapeSorter->Show();
//         Logo_TEALABS->Hide();
      }
      else                                                                    {

      }
   }

   if (m_visualizer && m_visualizer2)                                         {

      m_visualizer->DrawInformation( false);
      m_visualizer2->DrawInformation( false);

      m_visualizer->ShowScrollBar( false, false, true);
      m_visualizer2->ShowScrollBar( false, false, true);

      m_visualizer->HideOptionsPanel();
      m_visualizer2->HideOptionsPanel();

   }

   TimerUpdateMainScreen->Enabled = true;
   TimerControlMaquina->Enabled   = true;

   // Start locked
   if( GetLockMode() == BLOQ_PASSWORD)
      locked = true;

   // bloquear controles de visualización especial
   VisualizarControlesEspeciales(0);

//   //Get cameras' list
//   if (m_modMngr)                                                             {
//      m_modMngr->GetCamerasList(m_modMngr);
//   }
/*
   // Estados de los bootnes y los indicadores al arrancar

   EstadoMaquinaTodoOk->Visible=true;
   BotonParoCinta->Down=false;
   EstadoMaquinaParada->Visible=false;
   BotonMarchaCinta->Down=true;
*/


   std::list<TDataManager *>::iterator it = m_modMngr->DataManagerBegin();
   std::list<TDataManager *>::iterator itEnd = m_modMngr->DataManagerEnd();
   while ( it != itEnd)                                                       {
	  TImageSignalEmbededMngr* imageSignalEmbededMngr = dynamic_cast<TImageSignalEmbededMngr*>(*it);
	  if (imageSignalEmbededMngr)                                             {
		 imageSignalEmbededMngr->SetChart(Chart2);
		 break;
	  }
	  it++;
   }

   //new TColorAjustFormAux(L"TColorAjustFormAux", m_modMngr);

   TimerCheckLicense->Enabled = true;

}

//-----------------------------------------------------------------------------
void __fastcall TFormLT::ButtonCloseClick(TObject *Sender)                    {
   Close();
}

//-----------------------------------------------------------------------------
void TFormLT::LogoutUser()                                                    {
   if (m_modMngr)                                                             {
      m_modMngr->LogoutUser();
   }
}

// ------------------------------------------------------------------------------
bool TFormLT::LoginUser(wchar_t *user, wchar_t *pass)                         {
   if (m_modMngr)
     return (m_modMngr->LoginUser(user, pass) > None );

   return false;
}

/*
// -----------------------------------------------------------------------------
void __fastcall TFormLT::CfgButtonClick(TObject *Sender) {

//   if (Sender != CfgButton)
      return;

  if (!m_modMngr)
         return;

   if (!m_configState)                                                        {

      if (m_modMngr->GetAccessLevel() == None)                                {
         TFormLogIn *logIn = new TFormLogIn(this, m_modMngr);
         logIn->FormStyle  = fsStayOnTop;
         int result        = logIn->ShowModal();

         while  ((result != mrOk) && (result != mrCancel) )
            result = logIn->ShowModal();

         if( result == mrOk)
            m_modMngr->UpdateAccessLevel( false);

         //ShutDownBt->Caption = LoadText(ShutDownBt->Name.c_str());
         delete logIn;
      }

      if (m_modMngr->GetAccessLevel() >= User)                                {
        // ShutDownBt->Caption = LoadText(L"ShutDownLockBt");
         m_configState = true;

		 if (m_visualizer && m_visualizer2)                                    {
            m_visualizer->DrawInformation( true);
            m_visualizer2->DrawInformation( true);

            m_visualizer->ShowScrollBar( true, true, false);
            m_visualizer2->ShowScrollBar( true, true, false);
         }
      }

//      if (PanelEjection->Visible == true)                                     {
//
//         //Save configuration
//         if ( m_module)
//            m_module->SendCfg();
//         PanelEjection->Visible = false ;
//      }

   }
   else                                                                       {

      if (m_visualizer && m_visualizer2)                                      {
         m_visualizer->DrawInformation( false);
         m_visualizer2->DrawInformation( false);

         m_visualizer->ShowScrollBar( false, false, true);
         m_visualizer2->ShowScrollBar( false, false, true);

         m_visualizer->HideOptionsPanel();
         m_visualizer2->HideOptionsPanel();
      }

      m_configState = false;
      m_infoProdVisible = false;
     // ShutDownBt->Caption = LoadText(ShutDownBt->Name.c_str());
      if (keyboard->Visible)
         keyboard->Visible = false;
   }

   SetVisible(NULL);
}
*/

// ---------------------------------------------------------------------------
void __fastcall TFormLT::m_timerBloqueoTimer(TObject *Sender)                 {


   if (!m_modMngr)
      return;

   if (timeNoAction < (10000 - 1))
      ++timeNoAction;

//L   if (timeNoAction > (m_modMngr->GetTimeClose()) && FormUnlock && FormUnlock->Visible)
//L      FormUnlock->Close();

   bool lockedAnt = locked;
   if (timeNoAction > (m_modMngr->GetLockTime()) && m_modMngr && m_modMngr->GetAccessLevel() < TechnicalService) { // bloquear pantalla
      locked = true;
   }
   if (locked && !lockedAnt)                                                  {
      LockScreen();
      m_modMngr->UpdateAccessLevel( true);
   }
}

// ---------------------------------------------------------------------------
//
void TFormLT::LockScreen()                                                    {

      bool closeConfig = true;
      if (m_modMngr)                                                          {
         LogoutUser();
//         // close configuration if not special hardlock in
//         s_completeAccessLevel al;
//         m_modMngr->GetCompleteAccessLevel(al);
//         if (al.hardlockAccessLevel >= TechnicalService)
//            closeConfig = false;
      }

//      if (m_configState && closeConfig)
//         CfgButtonClick(CfgButton); // salir de configuración
}

// ----------------------------------------------------------------------------
int TFormLT::GetLockMode()                                                    {

   if (m_modMngr)
      return m_modMngr->GetLockMode();
   else
      return BLOQ_NORMAL;

}

// ----------------------------------------------------------------------------
void __fastcall TFormLT::PideClave()                                          {

   // int al = GetAccessLevel();

   if (!m_modMngr)
      return;

   if (m_modMngr->GetAccessLevel() < TechnicalService)                        {

      // Si no hemos accedido con los hardlocks pedir contraseña
      TFormLogIn *clave = new TFormLogIn(this, m_modMngr);
      clave->FormStyle  = fsStayOnTop;

      int result = clave->ShowModal();
      if( result == mrOk)
         m_modMngr->UpdateAccessLevel( false);
      delete clave;
   }

   SetVisible(NULL);
}


// ----------------------------------------------------------------------------

// Set Visible

void __fastcall TFormLT::SetVisible(TObject *Sender)                          {

   if (m_closing)
      return;

   TimerUpdateMainScreen->Interval  = 500;
   TimerUpdateMainScreen->Enabled   = !m_menuAjustes;
//   TimerInfoProd->Enabled  = m_menuAjustes ? false : TimerInfoProd->Enabled;

   m_timerClassInt->Enabled = PanelUI->Visible;
   sbKeyBoard->Visible  = true;


   // -- Enabled interfaces.
   if (m_prodgraphInterface)
      m_prodgraphInterface->SetEnabled(true);
	 // m_prodgraphInterface->SetEnabled(!m_configState);
 /*
   if (m_classInt)
	  m_classInt->SetEnabled(true);

   if (m_gridIntfUser)
      m_gridIntfUser->SetEnabled(!PanelInternalCfg->Visible);

 */


   PageControlCfgChange(NULL);
}

void __fastcall TFormLT::ShutDownButtonClick(TObject *Sender) {

   if (m_menuAjustes)                                                         {

      LockScreen();
      m_modMngr->UpdateAccessLevel( true);
      if( GetLockMode() == BLOQ_PASSWORD)
         locked = true;
      if (m_modMngr->GetAccessLevel() < TechnicalService)                     {
		 if (m_visualizer && m_visualizer2)                                   {
            m_visualizer->HideOptionsPanel();
            m_visualizer2->HideOptionsPanel();
            m_visualizer->DrawInformation( false);
            m_visualizer2->DrawInformation( false);
            m_visualizer->ShowScrollBar( false, false, true);
            m_visualizer2->ShowScrollBar( false, false, true);
         }
      }
   }
   else                                                                       {
      TryShutDown();
   }

}

void TFormLT::CloseMachine() {

   // Close ModuleManager

   // 1.- Stop Machine
   StopButtonClick(NULL);


   if (m_modMngr) {
      TData *data = m_modMngr->GetDataObject(LBL_STATE);
      while ((data->AsInt() != configured) && (data->AsInt() != closed))
         Sleep(10);

      // 2.- Close Machine
      m_modMngr->GetModule()->Close();

   }
   // 3.- Close Application
   Close();

}
// ---------------------------------------------------------------------------
bool TFormLT::ShutdownComputer( bool reboot)                                              {

   bool ret = false;
   // I need to ajust privileges (look SetPrivileges() of NVS)
   HANDLE hToken;
   TOKEN_PRIVILEGES *pTkp;

   // primero hay que darnos privilegio de SE_BACKUP_NAME
   if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
      if ((pTkp = (TOKEN_PRIVILEGES*)malloc(sizeof(TOKEN_PRIVILEGES)+sizeof(LUID_AND_ATTRIBUTES))) != NULL)
         if (LookupPrivilegeValue(0, SE_SHUTDOWN_NAME,&pTkp->Privileges[0].Luid)) {

            pTkp->PrivilegeCount = 1;
            pTkp->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, false, pTkp, sizeof(TOKEN_PRIVILEGES), 0, 0);
            free(pTkp);
            // shutdown
            if( !reboot)
               ret = ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION);
            else
               ret = ExitWindowsEx(EWX_SHUTDOWN | EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION);
         }
   return ret;
}


// -----------------------------------------------------------------------------
void TFormLT::SetChartProduction(THistZone *zone) {

   // if ( zone->GetIdZone() == 0)
   // ToDo: implementar caso de varias zonas en diferentes Charts
   if (m_prodgraphInterface)                                                  {
	  ChartLoad->Visible = true;

	  Chart1->Visible = false;

	  m_prodgraphInterface->Set(zone, Chart1, ChartLoad);

     //TODELETE
	 ChartLoad->Visible = true;
	 Chart1->Visible = false;
	 PanelDivision->Visible = false;
	 PanelDivision2->Visible = true;
	 if (m_prodgraphInterface) {
	  //firts disable, for refresh chart
		m_prodgraphInterface->SetEnabled(false);
		m_prodgraphInterface->SetEnabled(true);
	 }
	 //////////
   }

}

void __fastcall TFormLT::FormDestroy(TObject *Sender) {

   m_closing = true;

   if (m_module) {
      m_module->Stop();
      m_module->Close();
      DISPATCH_EVENT(INFO_CODE, L"EV_CORE_CLOSE", NULL, m_module->GetMngr());
      LOG_INFO1( LoadText(L"EV_CORE_CLOSE"));
   }

   if (m_prodgraphInterface) {
      delete m_prodgraphInterface;
      m_prodgraphInterface = NULL;
   }

   // if ( m_timeInterface)      {
   // delete m_timeInterface;
   // m_timeInterface = NULL;
   // }

   if (m_module) {
      delete m_module;
      m_module = NULL;
   }

   if (m_modMngr) {
      delete m_modMngr;
      m_modMngr = NULL;
   }

   if (m_visualizer) {
      delete m_visualizer;
      m_visualizer = NULL;
   }


   if (m_visualizer2) {
      delete m_visualizer2;
      m_visualizer2 = NULL;
   }

   if (m_persistence) {
      delete m_persistence;
      m_persistence = NULL;
   }

   if (m_processTimes) {
      m_timerProcessTime->Enabled = false;
      delete m_processTimes;
      m_processTimes = NULL;
   }

//   if (m_hdcPaint) {
//      ReleaseDC(PanelVisualizer->Handle, m_hdcPaint);
//   }

}
// ---------------------------------------------------------------------------

void __fastcall TFormLT::FormCreate(TObject *Sender)                          {


   m_closing      = false;

   m_machineMngr  = NULL;
   m_GM           = NULL;
   m_modMngr      = NULL;
   m_module       = NULL;
   m_GMgridIntf   = NULL;
   m_gridIntf     = NULL;
   m_classInt     = NULL;
   m_classIntExt  = NULL;
   m_gridIntfUser = NULL;

   wchar_t * app_name   = ( wcslen(NAME_MODULE) > 0) ? NAME_MODULE: APP_NAME;
   m_modMngr            = new TMyMachineModuleMngr(app_name, NULL);
   m_persistence        = new TXmlManager(FILE_NAME_CFG, true, m_modMngr); // create persistence
   Caption              = app_name;

   m_visualizer         = NULL;
   m_visualizer2        = NULL;


   m_module    = new TModule(app_name,TARGET_LOCATION , PATH_EVENTS_FILE,m_modMngr, m_persistence); // create Module

   // EL GM, si existe en config va aparte.
   if ((wcscmp(GM_NAME,L"GM") == 0) && (wcslen(TARGET_LOCATION) > 0))                                        {
         m_machineMngr   = new TMachineMngr(GM_NAME, NULL);
         m_GM        = new TGm(GM_NAME,FILE_NAME_CFG, GM_ENTRY_POINT, PATH_EVENTS_FILE,(TMachineMngr*)m_machineMngr); // create Module
         m_persistence = m_GM->getPersistence();
         if (m_GM)
			//m_GMgridIntf = new TGridInterface(PanelControlGM, m_GM->GetMngr());
			//descomentar la linea anterior, si se añade panel de control para configuracion del GM
            m_GMgridIntf = NULL; //añadir el panel al formulario

   }
   if (!m_persistence)
      LOG_ERROR1(L"Local Persistence Not Found.");

   if ( m_modMngr)                                                            {

      m_EventsInterface = new TEventsInterface(this, m_modMngr);
      m_gridIntf        = new TGridInterface(PanelGridInterface, m_modMngr);
	  m_classInt        = new TClassifierInterface(this, PanelUser, PanelGraph, m_modMngr, false, FLAG_IFC_SHOW_1, FLAG_USER  << 6);
	  m_gridIntfUser    = new TGridInterfaceUser(this, PanelInternalCfg, m_modMngr, false, FLAG_IFC_SHOW_2, FLAG_IFC_SHOW_EMBEDDED);
//    m_classIntExt     = new TClassifierInterface(this, PanelMainScreenCfg, PanelGraphMainScreen, m_modMngr, false, FLAG_USER  << 7, FLAG_USER  << 6);
   }

   if (m_gridIntf)
      m_gridIntf->SetEnabled(false);
   if (m_gridIntfUser)
      m_gridIntfUser->SetEnabled(false);
   if (m_classInt)
      m_classInt->SetEnabled(false);
   if (m_classIntExt)
      m_classIntExt->SetEnabled(false);


   //Cambio de nombre de los flags
   if (m_gridIntf)                                                            {
      TForm *formConfig = m_gridIntf->GetConfigForm();
      if (formConfig)                                                         {
         TCheckBox *cb    = dynamic_cast<TCheckBox*>    (formConfig->FindComponent(L"CfgCB_22"));
         if (cb)
            cb->Caption = L"Embedded_Ext";
         TCheckBox *cb2    = dynamic_cast<TCheckBox*>    (formConfig->FindComponent(L"CfgCB_23"));
         if (cb2)
            cb2->Caption = L"Internal_Ext";

      }
   }

   m_prodgraphInterface             = new TProdGraphInterface(this);
   m_prodgraphInterface->SetDeleteMe(false);

   m_auxHistCatInstantaneous        = new TAuxHistCatInstantanous( PanelDivision);
   m_auxValveCycles                 = new TAuxValveCycles(PanelDivision2);

   TTVisualizerFormDouble *m_form = new TTVisualizerFormDouble(PanelVisualizer);

// visualizer (camaras area)
//   m_visualizer = new TVisualizerMngrDouble(L"Visualizer", m_modMngr, PanelVisualizer, m_form, this);
//   m_visualizer->SetDeleteMe(false);
//
//   m_visualizer2 = new TVisualizerMngrDouble(L"Visualizer2", m_modMngr, PanelVisualizer2, NULL, this);//m_form2);
//   m_visualizer2->SetDeleteMe(false);

// visualizer continuous (camaras lineales)
   m_visualizer   = new TVisualizerMngrDoubleContinuous(L"Visualizer", m_modMngr, PanelVisualizer, m_form, this);
   m_visualizer->SetDeleteMe(false);

   m_processTimes = new TAuxProcessTime(PanelProcessTime, m_timerProcessTime, L"processTimes", m_modMngr);
   m_processTimes->SetDeleteMe(false);

   m_hdcPaint        = GetDC(PanelVisualizer->Handle);
   m_insideDraw      = false;
   m_infoProdVisible = false;
   m_showLimits0     = false;
   m_showLimits1     = false;

   m_menuAjustes = false;
   m_panelAlarma = false;



   strcpy(m_appName, "");
   m_lastLang = DEFAULT_ACTIVE_LANG;
   LoadKeyBoard();

   // Interfaces
   m_EventsInterface->SetEnabled(true);
   m_gridIntf->SetEnabled(false);
   if ( m_gridIntfUser)
      m_gridIntfUser->SetEnabled(false);
   // m_timeInterface->SetEnabled(false);

   PageControlCfg->ActivePage = Classification;

   WindowProc = MyWndProc;

   m_modMngr->SetAccessLevel(e_accessLevel::User);

}
// ---------------------------------------------------------------------------


void __fastcall TFormLT::EventsBtClick(TObject *Sender)                       {

   if ((!m_gridIntf)  || (!m_EventsInterface))
      return;

   if (PanelEvents->Visible)                                                  {
      PanelEvents->Visible = false;
      // m_EventsInterface->SetEnabled(false);
      return;
   }

   PanelEvents->Visible = true;

}



// ---------------------------------------------------------------------------
void __fastcall TFormLT::PageControlCfgChange(TObject *Sender)                {

   if (!PanelAjustes->Visible)                                                {
      m_gridIntf->SetEnabled(false);
      if ( m_classInt)
         m_classInt->SetEnabled(false);
      if ( m_gridIntfUser)
         m_gridIntfUser->SetEnabled(false);
      return;
   }

   switch (PageControlCfg->ActivePageIndex)                                   {

      // Clasificador
      case 0:
         m_gridIntf->SetEnabled(false);
         if ( m_classInt)
            m_classInt->SetEnabled(true);
         if ( m_gridIntfUser)
            m_gridIntfUser->SetEnabled(false);
         break;

      // Parametros Usuario
      case 1:
         m_gridIntf->SetEnabled(false);
         if ( m_classInt)
            m_classInt->SetEnabled(false);
         if ( m_gridIntfUser)
            m_gridIntfUser->SetEnabled(true);
         break;

      // Grid
      case 2:
         // solo habilito el interface si se esta visualizando porque tengo permiso
         if (TabSheeExtCfg->Visible)                                         {
            m_gridIntf->SetEnabled(true);
         }
         if ( m_classInt)
            m_classInt->SetEnabled(false);
         if ( m_gridIntfUser)
            m_gridIntfUser->SetEnabled(false);
         break;
      // otros
      default:
         m_gridIntf->SetEnabled(false);
         if ( m_classInt)
            m_classInt->SetEnabled(false);
         if ( m_gridIntfUser)
            m_gridIntfUser->SetEnabled(false);
         break;

   }
}

// ---------------------------------------------------------------------------
/**
 Load configuration from a file.
 Should no paths are defined:
   - if master o TechnicalService logged: An open dialog is loaded.
   - if user/advanceduser logged: A warning message is showed to the user.
 Notice this method stops and closes machine.
 */
void __fastcall TFormLT::LoadCfgBtClick(TObject *Sender) {
   /* There are two differents paths:
      1.- CfgDirectory: directory where load configuration from. Set by master/TechnicalService.
      2.- External directory where load configuration from, too (USB, disk). Set by master/TechnicalService.
      * Internally they are treated as the same.
      * Both, relative and absolute paths can be used. If a relative path is entered, take into a count the current working directory ( /bin). */
   if( !m_modMngr)
      return;

   TCHAR directory[100];
   DWORD a = GetCurrentDirectory(100, directory);

   TData *dirData          = m_modMngr->GetDataObject(L"CfgDirectory");

   wchar_t fullPath[1024]  = L"";
   wchar_t fullPathC[1024]  = L"";

//   bool isUser = (m_modMngr->GetAccessLevel() < TechnicalService);
// Cambio Dani Ivorra para no pedir permisos al cargar config
   bool isUser = false;
   // getting access
   if (dirData)                                                               {

      unsigned int size = SIZEOF_CHAR(fullPath);
      dirData->AsText(fullPath, size);

      bool relative = false;
      if ( fullPath[1] == ':' )
         relative = false;
      else
         relative = true;


      if ( relative)
         swprintf( fullPathC, L"%s\\%s",directory, fullPath);
      else
         wcscpy( fullPathC, fullPath);

      if (!DirectoryExists(fullPathC))                                        {
         ShowMsg(LoadText(L"CFG_DIRNOTEXIST"), L"WARNING", MY_MB_OK);
         if (isUser)
            return;
      }
   }

   wchar_t* fileName = NULL;

   TLoadCfgDlg *dlg = new TLoadCfgDlg(Application, m_persistence, isUser);
   if (!dlg)
      return;

   dlg->LocalDir->Caption = fullPathC;
   // External Disk

   TData *diskData = m_modMngr->GetDataObject(L"DiskDirectory");
   wchar_t externalPath[1024] = L"";

   dlg->externalcb->Enabled = false;
   if (diskData) {
      unsigned int size = SIZEOF_CHAR(externalPath);
      diskData->AsText(externalPath, size);
      if (DirectoryExists(externalPath)) {
         dlg->ExternalDir->Caption = externalPath;
         dlg->externalcb->Enabled = true;
	  }
   }

   wchar_t  txtNameConfig[100];
   memset(txtNameConfig, 0x00, sizeof(txtNameConfig));

   while (dlg->ShowModal() == mrOk) {

      UnicodeString strToLoad;

      int index = dlg->cfglistbox->ItemIndex;
      if (index == -1)
         ShowMsg(LoadText(CFG_NOTSELECTED), L"WARNING", MY_MB_OK);
      else {
         strToLoad = dlg->cfglistbox->Items->operator[](index);
         std::wstring tmp;
         if (dlg->localcb->Checked)
            tmp = std::wstring(dlg->LocalDir->Caption.c_str());
         else if ((dlg->externalcb->Enabled) && (dlg->externalcb->Checked))
            tmp = std::wstring(dlg->ExternalDir->Caption.c_str());

         tmp = tmp + L"\\" + strToLoad.c_str();
         fileName = _wcsdup(tmp.c_str());

         // guardamos en nombre de la cofig seleccionada para mas adelante instanciarlo como m_AppName
         // eliminamos la palbra ".xml" del nombre de la config
         wcstok(strToLoad.w_str(), L".");     // copiamos hasta el "."
         wcscpy( txtNameConfig, strToLoad.w_str());

         break;
      }
   }

   delete dlg;

   if (!fileName)
      return;

   if (!m_persistence->CheckFile(fileName)) {
      ShowMsg(LoadText(FILE_NOTVALID), L"WARNING", MY_MB_OK);
      return;
   }

   TData *data = m_modMngr->GetDataObject(LBL_STATE);
   int ret = -1;

   if (data->AsInt() == running) {
      wchar_t warntext[256];
      swprintf(warntext, L"%s\r\n %s", LoadText(CFG_SAVE_NOTICE), LoadText(STOP_MACHINE_NOTICE));
      ret = ShowMsg(warntext, L"WARNING", MY_MB_OKCANCEL);
   }
   else {
      ret = ShowMsg(LoadText(CFG_SAVE_NOTICE), L"WARNING", MY_MB_OKCANCEL);
   }

   if (ret != mrOk) {
      free(fileName);
      return;
   }

   //-- Cargamos la configuracion seleccionada

   if (m_gridIntf)
      m_gridIntf->SetEnabled(false);
   if (m_gridIntfUser)
      m_gridIntfUser->SetEnabled(false);
   if (m_classInt)
      m_classInt->SetEnabled(false);
   if (m_classIntExt)
      m_classIntExt->SetEnabled(false);
//   if( m_modMngr->m_viewIfc)
//      m_modMngr->m_viewIfc->SetEnabled(false);
//   if (m_timeInterface)
//      m_timeInterface->SetEnabled(false);


   LOG_INFO1( L"LOAD_CONFIGURATION_BUTTON");

   SplashLoad->BeginUpdate();
   SplashLoad->BasicProgramInfo->AutoLoad = true;
   SplashLoad->BasicProgramInfo->ProgramVersion->Text = "";
   SplashLoad->BasicProgramInfo->ProgramName->Text = "";
   SplashLoad->BasicProgramInfo->CopyRight = "";
   SplashLoad->EndUpdate();

   SplashLoad->Show();

   SplashLoad->BeginUpdate();
   SplashLoad->ProgressBar->Position = 20;
   SplashLoad->EndUpdate();


   // 1.- Stop Machine
   StopButtonClick(NULL);
   while ((data->AsInt() != configured) && (data->AsInt() != closed))
      Sleep(10);


   // 2.- Close Machine

   // Desactivar timer de control de maquina
   TimerUpdateMainScreen->Enabled = false;
   TimerControlMaquina->Enabled   = false;

   //Set m_cyclesReport pointer to NULL in mauxValvesCycles
   m_auxValveCycles->Reset();

   m_modMngr->GetModule()->Close();

   SplashLoad->BeginUpdate();
   SplashLoad->ProgressBar->Position = 60;
   SplashLoad->EndUpdate();

   if (m_persistence->ResetAndLoadConfiguration(fileName))                    {
      // 4.- Request configuration
      TData *data = m_modMngr->GetDataObject(LBL_STATE);
      // Close and Request Cfg
      while (data->AsInt() != closed)
         Sleep(10);

      // TODO: No está claro que esto aga falta aquí ya, pero no está de más, quitar cualquier mensaje antiguo
      Application->ProcessMessages();
      //request configuration and save when it arrvies
      m_modMngr->GetModule()->RequestCfg( true);
      SplashLoad->BeginUpdate();
      SplashLoad->ProgressBar->Position = 90;
      SplashLoad->EndUpdate();
   }

   free(fileName);

   // - Poner el nombre de la configuracion seleccionada como nombre de aplicacion/configuracion activo
   m_modMngr->SetApplicationName(txtNameConfig);


   // Activar timer de control de maquina
   TimerUpdateMainScreen->Enabled = true;
   TimerControlMaquina->Enabled   = true;

   // actulizar intro splash
   SplashLoad->BeginUpdate();
   SplashLoad->ProgressBar->Position = 100;
   SplashLoad->EndUpdate();

   SplashLoad->Hide();

   //Set ImageSignalChart
   std::list<TDataManager *>::iterator it = m_modMngr->DataManagerBegin();
   std::list<TDataManager *>::iterator itEnd = m_modMngr->DataManagerEnd();
   while ( it != itEnd)                                                       {
	  TImageSignalEmbededMngr* imageSignalEmbededMngr = dynamic_cast<TImageSignalEmbededMngr*>(*it);
	  if (imageSignalEmbededMngr)                                             {
		 imageSignalEmbededMngr->SetChart(Chart2);
		 break;
	  }
	  it++;
   }

   // apreto el boton de ajustes para salir a la pantalla principal una vez ccargada la nueva config
   BotonAjustesClick(NULL);

}
// ---------------------------------------------------------------------------

void __fastcall TFormLT::SaveCfgBtClick(TObject *Sender)                      {

   TCHAR directory[100];
   DWORD a = GetCurrentDirectory(100, directory);

   wchar_t* fileName = NULL;
   wchar_t name[1024];
   m_module->SendPartialCfg();

   if( !m_modMngr)
      return;

//   bool isUser = (m_modMngr->GetAccessLevel() < TechnicalService);
   bool isUser = false;

   // getting access
   // // 1.- Get current loading directory.
   TData *dirData = m_modMngr->GetDataObject(L"CfgDirectory");
   wchar_t fullPath[1024] = L"";
   wchar_t fullPathC[1024] = L"";

   if (dirData)                                                               {
      unsigned int size = SIZEOF_CHAR(fullPath);
      dirData->AsText(fullPath, size);

      bool relative = false;
      if ( fullPath[1] == ':' )
         relative = false;
      else
         relative = true;

      if ( relative)
         swprintf( fullPathC, L"%s\\%s",directory, fullPath);
      else
         wcscpy( fullPathC, fullPath);

      if (!DirectoryExists(fullPathC))                                        {
         ShowMsg(LoadText(L"CFG_DIRNOTEXIST"), L"WARNING", MY_MB_OK);
         if (isUser)
            return;
      }
   }

   if (isUser)                                                                {

      TSaveCfgDlg *dlg = new TSaveCfgDlg(Application);
      if (!dlg)
         return;

      dlg->BtnLoad->Visible      = false;
      dlg->localcb->Visible      = true;
      dlg->externalcb->Visible   = true;

      // External Disk
      TData *diskData = m_modMngr->GetDataObject(L"DiskDirectory");
      wchar_t externalPath[1024] = L"";

      dlg->externalcb->Enabled = false;
      if (diskData)                                                           {
         unsigned int size = SIZEOF_CHAR(externalPath);
         diskData->AsText(externalPath, size);
         if (DirectoryExists(externalPath))                                   {
            dlg->externalcb->Enabled = true;
         }
      }


      while (dlg->ShowModal() == mrOk)                                        {
         if ((IsRelativePath(dlg->newnamecfg->Text.c_str())) &&
             (!ValidateString(dlg->newnamecfg->Text.c_str())))
            ShowMsg(LoadText(NAME_NOTVALID), L"WARNING",MY_MB_OK);
         else                                                                 {
            std::wstring tmp;
            if (dlg->localcb->Checked)
               tmp = std::wstring(fullPathC);
            else if ((dlg->externalcb->Enabled) && (dlg->externalcb->Checked))
               tmp = std::wstring(externalPath);

            tmp = tmp + L"\\" + dlg->newnamecfg->Text.c_str();
            fileName = _wcsdup(tmp.c_str());
            break;
         }
      }

      delete dlg;
   }// MASTER
   else                                                                       {

      TSaveCfgDlg *dlg = new TSaveCfgDlg(Application);
      if (!dlg)
         return;

      dlg->BtnLoad->Visible      = true;
      dlg->localcb->Visible      = false;
      dlg->externalcb->Visible   = false;

      dlg->editDir->Text = fullPathC;

      if (dlg->ShowModal() == mrOk)                                           {

         std::wstring tmp;
         tmp = dlg->editDir->Text.c_str();
         tmp = tmp + L"\\" + dlg->newnamecfg->Text.c_str();
         fileName = _wcsdup(tmp.c_str());
      }
      delete dlg;
   }

   if (!fileName)
      return;

   swprintf(name, L"%s\0", fileName);


   free(fileName);
   if ( m_persistence)                                                        {
      // enviamos la configuracion completa embebida
      if ( m_module)
         m_module->SendEmbeddedCfgToSaveTo( name);
   }
   else                                                                       {
      LOG_WARN1( L"Local persistence doesn't exist");
      DISPATCH_EVENT( WARN_CODE, L"EV_REMOTE_PERSISTENCE", L"Local persistence doesn't exist", m_modMngr);
   }

}

// ---------------------------------------------------------------------------
TData* TFormLT::GetLoadImages()                                               {

   TCapturerMngr *capMngr = NULL;

   std::list<TDataManager*>::iterator it = this->m_modMngr->DataManagerBegin();
   while (it != m_modMngr->DataManagerEnd()) {
      if ((capMngr = dynamic_cast<TCapturerMngr*>(*it)) != NULL)
         break;
      it++;
   }
   if (capMngr) {
      it = capMngr->DataManagerBegin();
      TCapturer* cap = NULL;
      while (it != capMngr->DataManagerEnd()) {
         if ((cap = dynamic_cast<TCapturer*>(*it)) != NULL) {
            if (cap && cap->Enabled()) {
               TData* data = cap->GetDataObject(L"Show Window");
               if (data)
                  return data;
            }
         }
         it++;
      }
   }

   return NULL;
}

// ------------------------------------------------------------------------------
TData* TFormLT::GetSaverForm()                                                {

   TData *data = NULL;
   TSaver* saver = NULL;

   std::list<TDataManager*>::iterator it = this->m_modMngr->DataManagerBegin();
   while (it != m_modMngr->DataManagerEnd()) {
      if ((saver = dynamic_cast<TSaver*>(*it)) != NULL) {
         if (saver->Enabled()) {
            TData* data = saver->GetDataObject(L"Show Window");
            if (data)
               return data;
         }
      }
      it++;
   }

   return data;
}

// -----------------------------------------------------------------------------
TData* TFormLT::GetSaveDialogImages()                                         {

   TSaver *saver = NULL;

   std::list<TDataManager*>::iterator it = m_modMngr->DataManagerBegin();
   while (it != m_modMngr->DataManagerEnd()) {
      if ((saver = dynamic_cast<TSaver*>(*it)) != NULL) {
         if (saver && saver->Enabled()) {
            TData * data = saver->GetDataObject(L"Directory");
            if (data)
               return data;
         }
      }
      it++;
   }
   return NULL;
}

// -----------------------------------------------------------------------------
void __fastcall TFormLT::LoadImgBtClick(TObject *Sender)                      {

   TCHAR directory[100];
   DWORD a = GetCurrentDirectory(100, directory);

   TData* data = GetLoadImages();
   if (data) { // User
      TDataManager* parent = data->GetParent();
      if (parent) {
         TData* daMode = parent->GetDataObject(L"Mode");
         if (daMode) {
            if (m_modMngr && m_modMngr->GetAccessLevel() < TechnicalService)
               daMode->SetAsInt(1);
            else
               daMode->SetAsInt(0);
         }
      }

      if (data->AsInt() == 1) {
         data->SetAsInt(0);
         data->SetAsInt(1);
      }
      else
         data->SetAsInt(1);
   }

   else
      ShowMsg(LoadText(L"REVIEW_CFG_NOTICE"), L"WARNING", MY_MB_OK);

   SetCurrentDirectory( directory);
}

// ---------------------------------------------------------------------------
void __fastcall TFormLT::SaveImagesBtClick(TObject *Sender)                   {

   TCHAR directory[100];
   DWORD a = GetCurrentDirectory(100, directory);

   TData* data = GetSaverForm();
   if (!data) {
      return;
   }

   if (data) { // User
      TDataManager* parent = data->GetParent();
      if (parent) {
         TData* daMode = parent->GetDataObject(L"Mode");
         if (daMode) {
            if (m_modMngr && m_modMngr->GetAccessLevel() < TechnicalService)
               daMode->SetAsInt(1);
            else
               daMode->SetAsInt(0);
         }
      }
   }

   if (data->AsInt() == 1) {
      data->SetAsInt(0);
      data->SetAsInt(1);
   }
   else
      data->SetAsInt(1);

   SetCurrentDirectory( directory);
}

// ---------------------------------------------------------------------------
void __fastcall TFormLT::EventsBtChange(TObject *Sender)                      {

   if ((EventsBt->Color == clRed) || (EventsBt->Color == clYellow))           {
      // Tengo Alarma
      ColorEventTimer->Enabled = true;
//      Boton_Alarma->BringToFront();
//      Boton_NoAlarma->SendToBack();


   }
   else                                                                       {
      // No hay alarma
      ColorEventTimer->Enabled = false;
      m_EventColor = EventsBt->Color;
      Boton_NoAlarma->Visible = true;
      Boton_Alarma->Visible   = false;//      Boton_NoAlarma->BringToFront();
   }
}

// ---------------------------------------------------------------------------
void __fastcall TFormLT::ColorEventTimerTimer(TObject *Sender)                {


   // Hacer parpadear el panel de alarmas segun su nivel de alarma
   if ((EventsBt->Color == clRed) || (EventsBt->Color == clYellow))           {
      // hacemos parpadear el boton de alarma
      if (Boton_Alarma->Visible == false)                                     {
         Boton_Alarma->Visible   = true;
         Boton_NoAlarma->Visible = false;
      }
      else                                                                    {
         Boton_NoAlarma->Visible = true;
         Boton_Alarma->Visible   = false;
      }
   }
   else                                                                     {
      // dejamos de parpadear
      Boton_NoAlarma->Visible = true;
      Boton_Alarma->Visible = false;
   }



   // Hacer parpadear el panel de alarmas segun su nivel de alarma
   if ((EventsBt->Color == clRed) || (EventsBt->Color == clYellow))           {
      m_EventColor = EventsBt->Color;
      // Parpadeo del panel intercalando el panel blanco
      EventsBt->Color = clWhite;
   }
   else if (EventsBt->Color == clWhite)                                       {
      EventsBt->Color = m_EventColor;
   }
   else     {

      Boton_NoAlarma->Visible = true;
      Boton_Alarma->Visible = false;
      ColorEventTimer->Enabled = false;
   }

}

// ---------------------------------------------------------------------------
void __fastcall TFormLT::m_timerClassIntTimer(TObject *Sender)                {
   if (m_classInt)
	  m_classInt->RefreshItemsInListIfRunning();
   if (m_classIntExt)
	  m_classIntExt->RefreshItemsInListIfRunning();
   if (m_gridIntfUser)
      m_gridIntfUser->RefreshItemsInList();
}

// ---------------------------------------------------------------------------
void __fastcall TFormLT::m_timerProcessTimeTimer(TObject *Sender)             {
   if (m_processTimes == NULL)
      return;

   TPanel *panel = m_processTimes->GetPanel();
   if (panel && TabSheetAnalysis->Visible) {
      m_processTimes->RefreshTimes();
   }
}

//----------------------------------------------------------------------------
void TFormLT::LoadNumricKeyboard()                                            {

   if (keyboard->KeyboardType != ktNUMERIC)
      keyboard->KeyboardType = ktNUMERIC;
}

//----------------------------------------------------------------------------
void TFormLT::LoadKeyBoard()                                                  {

   if (keyboard->KeyboardType == ktNUMERIC)
      keyboard->KeyboardType = ktQWERTY;

   wchar_t fileName[1024];
   wchar_t CODE[3];
   CODE[0] = LANG_CODE[ACTIVE_LANG][1];
   CODE[1] = LANG_CODE[ACTIVE_LANG][2];
   CODE[2] = L'\0';
   swprintf(fileName, L".\\lang\\%s_KEYBOARD.txt\0", CODE);
   if ( FileExists(fileName) )
            keyboard->LoadKeybdLayout(fileName);
   else
      keyboard->LoadKeybdLayout(L".\\lang\\EN_KEYBOARD.txt");

   m_lastLang = ACTIVE_LANG;

}

//-----------------------------------------------------------------------------
void __fastcall TFormLT::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift) {

   if( Key == VK_F1)                                                          {
     if( Key == VK_F1 && m_modMngr->GetAccessLevel() >= TechnicalService)     {
       LoadImgBtClick(Sender);
     }
   }
   else                                                                       {
      if (m_visualizer)
         m_visualizer->FormKeyDown(Sender, Key, Shift);
      if (m_visualizer2)
         m_visualizer2->FormKeyDown(Sender, Key, Shift);
   }


}

//---------------------------------------------------------------------------
void __fastcall TFormLT::SaveCfgButtonClick(TObject *Sender)                  {

   if (m_module)
      m_module->SendCfg();
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::OnFormChange(TObject *Sender)                        {

   PostMessage( Application->MainForm->Handle, msg_paint_maingraph, NULL, NULL);
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::OnTimerSpeedButton(TObject *Sender)                  {

   MSG msg;
   if( PeekMessage( &msg, Handle, WM_LBUTTONUP, WM_LBUTTONUP, PM_REMOVE))
      DispatchMessage( &msg);

   if( sb)                                                                    {
      // aumentar velocidad de repetición
      if( m_timerSpeed->Interval > 20)                                        {
         m_timerSpeed->Interval /= 1.2;
         m_incrTimer        = 1;
         m_v_incrTimer      = 0;
      }
      else if (m_v_incrTimer < 25)                                            {
         ++m_v_incrTimer;
         m_incrTimer        = 5;
      }
      else                                                                    {
         m_incrTimer        = 10;
      }

      // llamar evento
      TShiftState Shift;
      event( sb, Controls::mbLeft, Shift, 0, 0);
   }
}


//---------------------------------------------------------------------------
void __fastcall TFormLT::OnButtonMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)                                    {
     // guardar evento y activar timer
   if( (sb = dynamic_cast<TAdvSmoothButton*>(Sender)) != NULL)                {
      event                      = sb->OnMouseDown;
      if( m_timerSpeed)
         m_timerSpeed->Enabled  = true;
   }
   if( sb)                                                                    {
      wchar_t name[256];
      wcsncpy( name, sb->Name.w_str(), SIZEOF_CHAR(name));
      name[SIZEOF_CHAR(name)-1]    = 0;


      // buscar objeto con el mismo nombre, sin las tres últimas letras
      if( wcslen( name) > 4)                                                  {
         wchar_t *sentido     = &name[wcslen(name)-3];
         int   val            = _wtoi( sentido+1);

         name[wcslen(name)-4] = 0;
         wchar_t           u[64];
         TEdit             *ed   = NULL;
         TAdvSmoothLabel   *lab  = NULL;

         if( dynamic_cast<TEdit*>(FindComponent( name)))                      {
            ed = (TEdit*)FindComponent( name);
            if (ed)
               wcscpy( u, ed->Text.w_str());
            else
               return;
         }
         else if( dynamic_cast<TAdvSmoothLabel*>(FindComponent( name)))       {
            lab = (TAdvSmoothLabel*)FindComponent( name);
            if ( lab)
               wcscpy( u, lab->Caption ? lab->Caption->Text.w_str() : L"");
            else
               return;
         }
         else
            return;

         wchar_t text[256];
         if( wcschr( u, L'.'))                                                {

            // --- con decimal
            int e;
            wchar_t d[256], tmp[256];

            memset(d, 0x0, sizeof(d));
            if (lab)
               swscanf(lab->Caption->Text.w_str(), L"%d.%s", &e, &d);
            else if (ed)
               swscanf(ed->Text.w_str(), L"%d.%s", &e, &d);
            int i;
            if (u[0] == L'-')
               i = e * pow(10.0, (int)wcslen(d)) - _wtoi(d);
            else
               i = e * pow(10.0, (int)wcslen(d)) + _wtoi(d);

            if (*sentido == L'A')
               i += (val*m_incrTimer);
            else
               i -= (val*m_incrTimer);

            swprintf(tmp, L"%%.%df", wcslen(d));
            swprintf(text, tmp, i / (float)pow(10.0, (int)(wcslen(d))));

            if( ed)
               ed->Text = text;
            else if(lab && lab->Caption)
               lab->Caption->Text = text;

         }else                                                                {
            // --- enteros
            int   i        = _wtoi(u);
            if( *sentido == 'A')
                  i  += (val*m_incrTimer);
               else
                  i  -= (val*m_incrTimer);

               if( ed)
                  ed->Text = i;
               else if(lab)
                  lab->Caption->Text = i;
         }
      }
   }
}

//-----------------------------------------------------------------------------
void __fastcall TFormLT::OnButtonMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y){
   if( m_timerSpeed)                                                          {
      m_timerSpeed->Enabled  = false;
      m_timerSpeed->Interval = 500;
   }
   sb                 = NULL;
   m_incrTimer        = 1;
   m_v_incrTimer      = 0;
}

//-----------------------------------------------------------------------------
void __fastcall TFormLT::m_timerHideInfoEventsTimer(TObject *Sender)          {
   if (InfoEvents)                                                            {
      InfoEvents->Visible = false;
   }
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::TimerUpdateMainScreenTimer(TObject *Sender)          {

   #ifndef _DEBUG
   try                                                                        {
   #endif
	  TimerUpdateMainScreen->Interval = 3000;
      // Update main form information
	  TZoneClassifier *zoneClass = m_modMngr ? m_modMngr->GetZoneClas() : NULL;
	  if( zoneClass )                                                         {
		 THistZone *histZone = zoneClass->GetHistZone();
		 if (histZone)                                                        {
			if (PanelDivision->Visible == true  && m_auxHistCatInstantaneous)
			   m_auxHistCatInstantaneous->PaintDivisionGraph( histZone);
			else if(PanelDivision2->Visible == true  && m_auxValveCycles)
			   m_auxValveCycles->PaintDivisionGraph( histZone);
		 }
	  }


	  // Update current type
      TPersistence *pers = GetPersistence();
      if( pers)                                                               {
         wchar_t activeType[256];
         int size = SIZEOF_CHAR(activeType);
         pers->GetActiveTypeConfig(activeType, size);

         if (wcslen(activeType) > 0)                                          {
            std::list<s_typeInfo> typeslist;
            ((TXmlManager*)pers)->getTypesInfo(typeslist);
			if ((typeslist.size() > 0))                                       {
               std::list<s_typeInfo>::iterator it = typeslist.begin();
               while (it != typeslist.end()) {
                  if (wcscmp((*it).m_name.c_str(), activeType) == 0)          {
                     break;
                  }
                  it++;
               }
            }
         }
      }

      // update graphs
	  if( Application && Application->MainForm )
		 PostMessage( Application->MainForm->Handle, msg_paint_maingraph, NULL, NULL);


   #ifndef _DEBUG
   }
   __except( EXCEPTION_EXECUTE_HANDLER)                                       {
      unsigned long code = GetExceptionCode();
      wchar_t slastCode[128];
      swprintf( slastCode, L"UpdateMainScreenTimer: %d", code);
      LOG_ERROR1(slastCode);
      DISPATCH_EVENT(ERROR_CODE, L"EV_EXCEPTION_CAPTURED:", slastCode, m_module->GetMngr());
   }
   #endif

}

//-----------------------------------------------------------------------------
void __fastcall TFormLT::EjectionWizardButtonClick(TObject *Sender)           {
   if( m_modMngr) {
      //Refresh warningValves and criticalValves TDatas
	  TZoneClassifier *zoneClass = m_modMngr ? m_modMngr->GetZoneClas() : NULL;
	  if( zoneClass )                                                         {
		 THistZone *histZone = zoneClass->GetHistZone();
		 if (histZone)                                                        {
			m_auxValveCycles->PaintDivisionGraph( histZone);
		 }
	  }
	  //
	  m_modMngr->ShowEjectorForm();
   }
}

//-----------------------------------------------------------------------------
void __fastcall TFormLT::LimitsValuesClick(TObject *Sender)                   {
   TControl *wc;
   if( (wc = dynamic_cast<TControl*>(Sender)) == NULL)
      return;

   wchar_t  _nam[256];
   wcscpy( _nam, wc->Name.c_str());
   int catId = _nam[wcslen(_nam)-1] - '0';
   if (catId >= 0 && catId <= 1)                                              {
      if (wcsncmp(_nam, L"limits", 6) == 0)                                   {
         if( catId == 0) m_showLimits0 = true;
         if( catId == 1) m_showLimits1 = true;
      }
      else if(wcsncmp(_nam, L"values", 6) == 0)                               {
         if( catId == 0) m_showLimits0 = false;
         if( catId == 1) m_showLimits1 = false;
      }
   }

   SetVisible(NULL);
}

//---------------------------------------------------------------------------
TPersistence *TFormLT::GetPersistence()                                       {
   return m_persistence;
}

//---------------------------------------------------------------------------
void TFormLT::TryShutDown( bool reboot)                                       {
   wchar_t appName[50];
   unsigned int count = SIZEOF_CHAR(appName);
   bool ok = false;
   if (m_modMngr)
      ok = m_modMngr->GetApplicationName(appName, count);
   wchar_t warntext[256];

//   if (m_modMngr && m_modMngr->GetAccessLevel() < Master)
    swprintf(warntext, L"%s %s %s", ok ? appName : LoadText(L"APPLICATION"), LoadText(L"SHUT_DOWN_NOTICE"), LoadText(L"ARE_YOU_SURE_NOTICE"));
//   else
//    swprintf(warntext, L"%s %s %s", ok ? appName : LoadText(L"APPLICATION"), LoadText(L"CLOSE__NOTICE"), LoadText(L"ARE_YOU_SURE_NOTICE"));

   int ret = ShowMsg(warntext, L"INFO", MY_MB_OKCANCEL);
   if (ret == 1)                                                              {
      CloseMachine();
      Sleep(500);
      ShutdownComputer( reboot);
   }
}

//-----------------------------------------------------------------------------
void __fastcall TFormLT::rebootProgramButonClick(TObject *Sender)             {

   //Stop the conveyor, to prevent errors
   //....
   wchar_t appName[50];
   unsigned int count = SIZEOF_CHAR(appName);
   bool ok = false;
   if (m_modMngr)
      ok = m_modMngr->GetApplicationName(appName, count);

   wchar_t warntext[256];


    swprintf(warntext, L"%s %s %s", ok ? appName : LoadText(L"APPLICATION"), LoadText(L"Se va a reiniciar el programa"), LoadText(L"ARE_YOU_SURE_NOTICE"));

   int ret = ShowMsg(warntext, L"INFO", MY_MB_OKCANCEL);


   if (ret == 1)                                                              {

      DISPATCH_EVENT(INFO_CODE, L"EV_TROUBLESHOOT_STOP_RUN", L"", m_module->GetMngr());
      LOG_INFO1( LoadText(L"EV_TROUBLESHOOT_STOP_RUN"));

      SplashLoad->BeginUpdate();
      SplashLoad->BasicProgramInfo->AutoLoad = true;
      SplashLoad->BasicProgramInfo->ProgramVersion->Text = "";
      SplashLoad->BasicProgramInfo->ProgramName->Text = "";
      SplashLoad->BasicProgramInfo->CopyRight = "";
      SplashLoad->EndUpdate();

      SplashLoad->Show();

      // Parar programa
      StopButtonClick(NULL);

      SplashLoad->BeginUpdate();
      SplashLoad->ProgressBar->Position = 20;
      SplashLoad->EndUpdate();

      // wait 5 seconds
      Sleep(10000);

      SplashLoad->BeginUpdate();
      SplashLoad->ProgressBar->Position = 40;
      SplashLoad->EndUpdate();

      // wait 5 seconds
      Sleep(10000);

      SplashLoad->BeginUpdate();
      SplashLoad->ProgressBar->Position = 60;
      SplashLoad->EndUpdate();

      // wait 5 seconds
      Sleep(5000);

      InitButtonClick(NULL);

      SplashLoad->BeginUpdate();
      SplashLoad->ProgressBar->Position = 80;
      SplashLoad->EndUpdate();

      // wait 2 seconds
      Sleep(5000);

      SplashLoad->BeginUpdate();
      SplashLoad->ProgressBar->Position = 100;
      SplashLoad->EndUpdate();
      SplashLoad->Hide();
   }
}



//---------------------------------------------------------------------------
void __fastcall TFormLT::
OnChartClick(TObject *Sender)                        {

	if(PanelDivision->Visible) {
		PanelDivision->Visible = false;
		PanelDivision2->Visible = true;
	}
	else {
		PanelDivision->Visible = true;
		PanelDivision2->Visible = false;
	}

   return;
//   if (Chart1->Visible)                                                       {
//      Chart1->Visible = false;
//
//
//      ChartLoad->Visible = true;
//
////      PanelStatus->Visible = true;
//	  PanelDivision->Visible = false;
////      PanelStatus->Visible = true;
//   }
//   else if ( ChartLoad->Visible)                                              {
//	  ChartLoad->Visible = false;
//	  Chart1->Visible = false;
////      PanelStatus->Visible = false;
//	  PanelDivision->Visible = true;
////      PanelStatus->Visible = false;
//   }
//   else                                                                       {
//	  ChartLoad->Visible = false;
//      // LyraTechs comentado de momento no quiero mostrar esta información:
//      Chart1->Visible = true;
//
//
////      PanelStatus->Visible = true;
//	  PanelDivision->Visible = false;
////      PanelStatus->Visible = true;
//   }
//
//   // Enabled interfaces.
//   if (m_prodgraphInterface)                                                  {
//      //firts disable, for refresh chart
//      m_prodgraphInterface->SetEnabled(false);
//      m_prodgraphInterface->SetEnabled(true);
//   }
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::BtnUsbClick(TObject *Sender)                         {

   // 1.- Open dialog for extract
   TExtractDevice* extractDevice = new TExtractDevice(this);
   if (extractDevice)                                                         {

      if ( extractDevice->ShowModal() == mrCancel)                            {
         delete extractDevice;
      }
   }
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::TimerQuickTestTimer(TObject *Sender)                 {
//   if (m_modMngr)
//      m_modMngr->QuickTest();

}


//---------------------------------------------------------------------------
void __fastcall TFormLT::BtnNewInfoProdClick(TObject *Sender)                 {

   if (ShowMsg(LoadText(L"MSG_END_BATCH"), L"INFO", MY_MB_OKCANCEL) == mrOk)  {

      wchar_t activeType[256];
      int size = SIZEOF_CHAR(activeType);
      m_persistence->GetActiveTypeConfig(activeType, size);
      if( m_modMngr) {
         if (wcslen(activeType) > 0) {
            std::list<s_typeInfo> typeslist;
            ((TXmlManager*)m_persistence)->getTypesInfo(typeslist);
            if ((typeslist.size() > 0))                                                {
               std::list<s_typeInfo>::iterator it = typeslist.begin();
               while (it != typeslist.end()) {
                  if (wcscmp((*it).m_name.c_str(), activeType) == 0)  {
                     m_modMngr->UpdateTypeInReport((wchar_t *) (*it).m_translation.c_str());
                     break;
                  }
                  it++;
               }
            }
         }
         m_modMngr->ReportIntervalAndInit( false);
      }
   }
}
//---------------------------------------------------------------------------
void __fastcall TFormLT::InitButtonClick(TObject *Sender)                     {

   if (m_module)                                                              {
      InitButton->Enabled = false;
      StopButton->Enabled = false;
      m_module->Init();
   }

}

//---------------------------------------------------------------------------
void __fastcall TFormLT::StopButtonClick(TObject *Sender)                     {
   if (m_module)                                                              {
      InitButton->Enabled = false;
      StopButton->Enabled = false;
      m_module->Stop();
   }
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::TimerCargaTimer(TObject *Sender)                     {
   TimerCarga->Enabled = false;
   if( m_modMngr->SaveCarga())                                                {

      s_prodTrace prodT;
      m_modMngr->GetProdTrace( prodT);

      TimerCarga->Interval = prodT.interval*1000;

      wchar_t nFile[MAX_PATH];
      wchar_t nFile2[MAX_PATH];

      bool secondPathEmpy = false;

      if(prodT.dateInFileName)                                                {
         snwprintf(nFile, MAX_PATH, L"%s\\%s_%s", prodT.path, FormatDateTime("yyyymmdd", Now()).c_str(), prodT.fileName);

         if ( wcscmp( prodT.path2, L""))                                      {
            snwprintf(nFile2,MAX_PATH, L"%s\\%s_%s", prodT.path2, FormatDateTime("yyyymmdd", Now()).c_str(), prodT.fileName);
         }
         else
            secondPathEmpy = true;

      }
      else                                                                    {
         snwprintf(nFile, MAX_PATH, L"%s\\%s", prodT.path, prodT.fileName);
         if ( wcscmp( prodT.path2, L""))                                      {
            snwprintf(nFile2, MAX_PATH, L"%s\\%s", prodT.path2, prodT.fileName);
         }
         else
            secondPathEmpy = true;
      }


      wchar_t text[28];
      TZoneClassifier *zoneClass = m_modMngr ? m_modMngr->GetZoneClas() : NULL;
      if( zoneClass )                                                         {
         THistZone *histZone = zoneClass->GetHistZone();
         if (histZone)                                                        {
            const THistCatInstantaneous  *histInst = histZone->GetInstantaneous();
            if(histInst)                                                      {
               histZone->GetLoad( text, SIZEOF_CHAR( text));
               if ( !FileExists(nFile) )                                      {
                  FILE *fitx= _wfopen(nFile,L"a+");
                  if( fitx)                                                   {
                     wchar_t txt[1024];
                     swprintf( txt, L"%s", LoadText(L"CARGA_HEADER"));
                     for( int i = 0; i < histInst->GetNumCats(); ++i)         {
                        wchar_t catTxt[32];
                        swprintf( catTxt, L";CatId %d", i);
                        wcscat( txt, catTxt);
                     }
                     wcscat( txt, L"\n");
                     fwprintf( fitx, txt);
                     fclose(fitx);
                  }
               }
               //Se repite para copiar en la segunda ruta ( second path)
               if ( !FileExists(nFile2) && !secondPathEmpy)                   {
                  FILE *fitx= _wfopen(nFile2,L"a+");
                  if( fitx)                                                   {
                     wchar_t txt[1024];
                     swprintf( txt, L"%s", LoadText(L"CARGA_HEADER"));
                     for( int i = 0; i < histInst->GetNumCats(); ++i)         {
                        wchar_t catTxt[32];
                        swprintf( catTxt, L";CatId %d", i);
                        wcscat( txt, catTxt);
                     }
                     wcscat( txt, L"\n");
                     fwprintf( fitx, txt);
                     fclose(fitx);
                  }
               }
               char txt[1024];
               char *dateTime = TAux::WSTR_TO_STR(FormatDateTime( "yyyy/mm/dd hh:nn:ss", Now()).w_str()),
                    *load     = TAux::WSTR_TO_STR(text);

               float relativeLoad = histZone->CalcRelativeLoad();

               sprintf( txt, "%s;%s;%05.1f", dateTime, load, ( relativeLoad > 0 ? ((relativeLoad * 10 +.5)/10.) : 0.0 ));
               for( int i = 0; i < histInst->GetNumCats(); ++i)               {
                  char catTxt[16];
                  sprintf( catTxt, ";%05.1f", histInst->GetTantPerThousandCat( i) / 10.0);
                  strcat( txt, catTxt);
               }
               strcat( txt, "\n");
               char *fileName = TAux::WSTR_TO_STR(nFile);
               FILE *fitx = fopen(fileName ,"a+");
               if( fitx)                                                      {
                  fprintf( fitx, txt);
                  fclose(fitx);
               }
               //Se repite para copiar en la segunda ruta ( second path)
               if ( !secondPathEmpy)                                          {
                  char *fileName2 = TAux::WSTR_TO_STR(nFile2);
                  FILE *fitx2 = fopen(fileName2 ,"a+");
                  if( fitx2)                                                  {
                     fprintf( fitx2, txt);
                     fclose(fitx2);
                  }
               }

               delete [] dateTime;
               delete [] load;
               delete [] fileName;
            }
         }
      }
   }
   TimerCarga->Enabled = true;
}

//-----------------------------------------------------------------------------
void __fastcall TFormLT::InfoProdBtnClick(TObject *Sender)                    {
   if( m_modMngr)
      m_modMngr->ShowFileAccess();
}


//-----------------------------------------------------------------------------
void __fastcall TFormLT::ReturnbtnClick(TObject *Sender)                      {
   if (!m_modMngr)
      return;

      //Save configuration
   if ( m_module)
      m_module->SendCfg();

  // PanelEjection->Visible = false ;
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::OnStatusButtonTimer(TObject *Sender)                 {

   if (m_modMngr)                                                             {
	  m_modMngr->CheckButtonStartState();
   }
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::Timer_HardLockCheckTimer(TObject *Sender)            {
  /*
  wchar_t buffer[100];
  swprintf(buffer,L"Timer_HardLockCheckTimer attempt %d of 5",(m_checkHardlockAttempts+1));
  LOG_INFO1(buffer);
  THardlockChecker::CheckDevnodes();
  s_sharedPermissions permissions = THardlockChecker::GetPermissions();
  if ((permissions.machineHardlock == false && permissions.machineHardlockNeeded == true) ||
  	  (permissions.specialHASPhardLock == 0  && permissions.machineHardlockNeeded == false)) {
  	 /*
  	  if(  m_checkHardlockAttempts == 4 )                                     {
  		 //ToDo: si se conecta el hardlock de master o servicio tecnico,
        //entonces despachar un evento que incluya el numero de hardlock, Tambien al desconectar
  		 Timer_HardLockCheck->Enabled = false;
  		 m_checkHardlockAttempts      = 0;
  		 ShowMsg(LoadText(L"ERROR_NO_HARDLOCK"), L"ERROR", MY_MB_OK);
  		 LOG_INFO1(L"Hardlock not detected");
  		 #ifdef _DEBUG
  			// stop machine
  			if (m_module)
  			   m_module->Stop();
  			exit(EXIT_FAILURE);
  		 #else
  			// Close and shut down computer
  			CloseMachine();
  			ShutdownComputer();
  		 #endif
  	  }
  	  else                                                                    {
  		 m_checkHardlockAttempts++;
  	  }
  *//*
  }
  else                                                                       {
  	  //Timer_HardLockCheck->Enabled = false;
  	  m_checkHardlockAttempts      = 0;
  	  LOG_INFO1(L"Timer_HardLockCheckTimer Checked");
  	  if (m_modMngr)
  		 m_modMngr->UpdateAccessLevel( false);
  }
  */
}





// ----------------- Lyra Techs new interface -------------------------------


//---------------------------------------------------------------------------
void __fastcall TFormLT::BotonMarchaCintaClick(TObject *Sender)               {

   // Verificar estado del boton de marcha cinta
   if (BotonMarchaCinta->Down==true)                                          {
      // Actulizar boton de paro cinta
      BotonParoCinta->Down=False;

      // Actualizar etiquetas estado maquina
      if (BotonMarchaAlimentador->Down==False)                                {
         EstadoAlimentacionParada->Visible=true;
         EstadoMaquinaTodoOk->Visible=false;
      }
      else                                                                    {
         EstadoAlimentacionParada->Visible=true;
         EstadoMaquinaTodoOk->Visible=false;
      }

      EstadoMaquinaParada->Visible=false;

      // Acción arrancar cinta desde TwinCat
      if (m_modMngr->GetElectricCabinet()->ArrancaCinta()== false)            {
         BotonMarchaCinta->Down=false;
         BotonParoCinta->Down=true;
         //BotonParoCintaClick(NULL);

         // Actualizar etiquetas estado maquina
         EstadoMaquinaParada->Visible=true;
         EstadoMaquinaTodoOk->Visible=false;
         EstadoAlimentacionParada->Visible=false;
      }
   }
   else                                                                       {
      // si se pulsa y ya esta pulsado no cambiamos el estado del boton
      BotonMarchaCinta->Down=true;
   }
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::BotonParoCintaClick(TObject *Sender)                 {

  // Verificar estado del boton de paro cinta
  if (BotonParoCinta->Down==true)                                             {

      // Actualizar boton de marcha cinta
      BotonMarchaCinta->Down=False;

      //Actualizar boton alimentador ya que no puede estar en marcha si se para cinta
      BotonMarchaAlimentador->Down=False;
      BotonParoAlimentador->Down=True;

      // Actualizar etiquetas estado maquina
      EstadoMaquinaParada->Visible=true;
      EstadoMaquinaTodoOk->Visible=false;
      EstadoAlimentacionParada->Visible=false;


      // Acción parar cinta desde TwinCat
      m_modMngr->GetElectricCabinet()->PararCinta();

      //Accion parar alimentador ya que no puede estar en marcha si se para cinta

      // Acción parar alimentador
      m_modMngr->GetElectricCabinet()->PararAlimentacion();
  }
  else                                                                        {
      // si se pulsa y ya esta pulsado no cambiamos el estado del boton
      BotonParoCinta->Down=true;
  }

}

//---------------------------------------------------------------------------
void __fastcall TFormLT::BotonMarchaAlimentadorClick(TObject *Sender)         {

  // Verificar estado del boton de marcha alimentador
  if (BotonMarchaAlimentador->Down==true)                                     {
	  // Solo permito arrancar la alimentación si esta la cinta en marcha para evitar atascos
      if (BotonMarchaCinta->Down==true)                                       {

         // Actulizar boton de paro alimentador
         BotonParoAlimentador->Down=False;

		 // Actualizar etiquetas estado maquina
         EstadoMaquinaTodoOk->Visible=true;
         EstadoAlimentacionParada->Visible=false;
         EstadoMaquinaParada->Visible=false;

         // Acción arrancar alimentador desde TwinCat
         m_modMngr->GetElectricCabinet()->ArrancaAlimentacion();
      }
      else                                                                    {
		 // si se pulsa y no esta la cinta en marcha no permitimos
         BotonMarchaAlimentador->Down=false;
      }
  }
  else                                                                        {
      // si se pulsa y ya esta pulsado no cambiamos el estado del boton
	  BotonMarchaAlimentador->Down=true;
  }
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::BotonParoAlimentadorClick(TObject *Sender)           {

   // Verificar estado del boton de paro alimentador
   if (BotonParoAlimentador->Down==true)                                      {
      // Actulizar boton de marcha alimentador
      BotonMarchaAlimentador->Down=False;

      // Actualizar etiquetas estado maquina
      EstadoAlimentacionParada->Visible=true;
      EstadoMaquinaParada->Visible=false;
      EstadoMaquinaTodoOk->Visible=false;

      // Acción parar alimentador desde TwinCat
      m_modMngr->GetElectricCabinet()->PararAlimentacion();
   }
   else                                                                       {
      // si se pulsa y ya esta pulsado no cambiamos el estado del boton
      BotonParoAlimentador->Down=true;
   }

}

//---------------------------------------------------------------------------
void __fastcall TFormLT::BotonAjustesClick(TObject *Sender)                   {

   bool passwordOk = false;
   //  Si pulso el boton de ajustes y no estaba en el menu ajustes, entro al menu de ajustes
   if (   m_menuAjustes == false)                                             {
      if(m_modMngr->GetAccessLevel() == e_accessLevel::Master)                {
      passwordOk = true;
   }
   else                                                                       {
      TFormLogIn *logIn = new TFormLogIn(this, m_modMngr);
      int result        = logIn->ShowModal();
      UnicodeString label = logIn->GetLabel();

      while  ((result != mrOk) && (result != mrCancel) )
         result = logIn->ShowModal();

      if( result == mrOk)                                                     {
         passwordOk = logIn->IsClaveOk();
         if(passwordOk && _wcsicmp (label.c_str(), MASTER_LBL) == 0)          {
            m_modMngr->SetAccessLevel(e_accessLevel::Master);
            LOG_INFO1(L"Se ha cambiado el nivel de acceso de la app a MASTER");
         }
      }
      delete logIn;

      }
   }

   if(passwordOk)                                                             {

      // Si no soy master notifico que se ha entrado en la config
      if (m_modMngr->GetAccessLevel() == e_accessLevel::User)                 {
         LOG_INFO1(L"Un USER con clave ha entrado en la configuracion");
         // TODO: Ocultar funciones dentro de la config que no queremos que toque el USER NORMAL
      }
      else                                                                    {
         // - Si soy MASTER muestro funciones

		 // mostrar barra de opciones del visualizador
		 if (m_visualizer)
			m_visualizer->ShowOptionsPanel();
		 if (m_visualizer2)
			m_visualizer2->ShowOptionsPanel();
         // mostrar boton configuración de color
         // Aplicacion para plastico
         /*
         BotonConfigColor->Visible = true;
         */
      }


      // Dejo el boton anclado para que se cambie el icono del boton
      BotonAjustes->Down=true;

      PanelAjustes->Show();
      PanelStatus->Hide();

      m_menuAjustes = true;  // estado del menu: estoy dentro

   }
   // Si pulso el boton de ajustes y  estaba en el menu ajustes, salgo del menu de ajustes
   else                                                                       {

      // Si no soy master notifico que se ha salido de la config
      if (m_modMngr->GetAccessLevel() != e_accessLevel::Master)               {
         LOG_INFO1(L"Un USER con clave ha salido de la configuracion");

         // TODO: Ocultar funciones dentro de la config que no queremos que toque el USER NORMAL
      }

      // Dejo el boton desanclado para que se cambie el icono del boton
      BotonAjustes->Down=false;

      PanelStatus->Show();
      PanelAjustes->Hide();
      m_menuAjustes = false;  // estado del menu: estoy fuera

      // bloqueo visualizacion de controles especiales siempre al salir de ajustes
      VisualizarControlesEspeciales(0);

      // ocultar boton configuración de color
      BotonConfigColor->Visible = false;
   }

   SetVisible(NULL);

}

//---------------------------------------------------------------------------
void __fastcall TFormLT::BotonEncendidoApagadoClick(TObject *Sender)          {


   if (m_modMngr)                                                             {
      // Si estoy en modo de acceso MASTER el  boton de reiniciar devuelve a modo USER
      if (m_modMngr-> GetAccessLevel() == e_accessLevel::Master )             {

         wchar_t warntext[256];
         swprintf(warntext, L"Va a salir del modo de acceso MASTER, %s" , LoadText(L"ARE_YOU_SURE_NOTICE"));
         int ret = ShowMsg(warntext, L"INFO", MY_MB_OKCANCEL);
         if (ret == 1)                                                        {
            m_modMngr->SetAccessLevel(e_accessLevel::User);
            LOG_INFO1(L"Se ha cambiado el nivel de acceso de la app a USER");
            return;
         }
      }
   }

   // Apagar ordenaador
  // TryShutDown();

   // Reiniciar ordenador
   TryShutDown(true);
}


//---------------------------------------------------------------------------
void __fastcall TFormLT::BotonAlarmaClick(TObject *Sender)                    {


   if (m_panelAlarma == false)                                                {
      PanelAlarms->Show();
      PanelAlarms->BringToFront();
      EventsBtClick(Sender);
      m_panelAlarma = true;
   }
   else                                                                     {
      PanelAlarms->Hide();
      PanelAlarms->SendToBack();
      EventsBtClick(Sender);
      m_panelAlarma = false;
   }
}

//---------------------------------------------------------------------------
 void __fastcall TFormLT::sbKeyBoard1Click(TObject *Sender)                    {



   m_modMngr->LaunchKeyboard();


//   if (keyboard->Visible==false)                                              {
//      keyboard->Show();
//      keyboard->Visible=true;
//   }
//   else                                                                       {
//      keyboard->Hide();
//      keyboard->Visible=false;
//   }

}


//---------------------------------------------------------------------------
void __fastcall TFormLT::rebootComputerButtonClick(TObject *Sender)
{
   // Reiniciar ordenador
   TryShutDown(true);
}


//---------------------------------------------------------------------------
void __fastcall TFormLT::Img_logo_LyraTechsDblClick(TObject *Sender)          {

   // Login con codigo para acceder a funciones especiales
   // if (code == ....)
   VisualizarControlesEspeciales(1);
}

//---------------------------------------------------------------------------
void TFormLT::VisualizarControlesEspeciales(int nivelAcesso)                  {

   if (nivelAcesso && m_menuAjustes )                                         {

      BotonesSoftware->Show();

      TabSheeExtCfg->TabVisible = true;
      TabSheetAnalysis->TabVisible = true;

      // mostrar barra de opciones del visualizador
	  //  m_visualizer->ShowOptionsPanel();
      //  m_visualizer2->ShowOptionsPanel();

   }
   else                                                                       {

      BotonesSoftware->Hide();

      TabSheeExtCfg->TabVisible = false;
      TabSheetAnalysis->TabVisible = false;

	  // esconder barra de opciones del visualizador
	  if (m_visualizer)
		m_visualizer->HideOptionsPanel();
	  if (m_visualizer2)
		m_visualizer2->HideOptionsPanel();
   }

   return;
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::TimerControlMaquinaTimer(TObject *Sender)            {


   if (m_modMngr)                                                             {

      //--  Poner el nombre de la APP como nombre de configuración activa
      wchar_t appName[100];
      unsigned int count = SIZEOF_CHAR(appName);
      bool ok = false;
      ok = m_modMngr->GetApplicationName(appName, count);
      // Si hemos podido leer nombre de la aplicacion
      if (ok)                                                                 {
        TAttrib *attr = m_modMngr->GetAttrib(cAttribNames[ATTR_NAME]);
        if (attr) {
             attr->Set(appName);
        }
      }


      //--  Verificar el modelo de maquina configurado para el nombre de la aplicación

      int machineModel = m_modMngr->GetMachineModel();

//      if (machineModel==HYPERION)                                             {
//         Img_LogoHYPERION->Show();
//         Logo_TEALABS->Hide(__declspec(uuid("{C9D26B16-8BFB-49DC-B86A-428799ED0B55}")));
//      }
//      else if (machineModel==TEALABS)                                         {
//         Img_LogoHYPERION->Hide();
//         Logo_TEALABS->Show();
//      }

      if (machineModel==GRAPE_SORTER)                                         {
         Image_Logo_GrapeSorter->Show();
//         Logo_TEALABS->Hide();
      }
      else                                                                    {

      }

      // Proteccion por si no hay Electric Cabinet en cfg
      if (m_modMngr->GetElectricCabinet())                                    {

         // Verificar estado de Marcha/Paro
         bool estadoMarcha = m_modMngr->GetElectricCabinet()->EstadoMarcha();

         if (!estadoMarcha)                                                      {
            //Actuar solo si ha cambiado el estado
            if ( !(estadoMarcha == m_prevEstadoMarcha))                          {
               // -- Maquina parada

               // Poner protección de expulsiones
               if (m_modMngr->GetRejectMode() == 1)                              {   //Si la expulsion esta configurada en modo produccion
                  TDataManager *root = m_modMngr->GetRootMngr();
                  if (root)                                                      {
                     TData *da = root->GetDataObject(L"LTAIS\\RealTime\\EnableEjection");
                     if (da)                                                     {
                        da->SetAsInt(0);      // disable ejection
                     }
                  }
               }
               // Desactivo los botones del panel control de maquina ya que dependen del estado de TwinCat
               BotonParoCinta->Down=true;
               BotonParoCintaClick(NULL);
            }
         }
         else                                                                    {
            //Actuar solo si ha cambiado el estado
            if ( !(estadoMarcha == m_prevEstadoMarcha))                          {
               // -- Maquina en marcha

               // Quitar protección de expulsiones
               if (m_modMngr->GetRejectMode() == 1)                              { //Si la expulsion esta configurada en modo produccion
                  TDataManager *root = m_modMngr->GetRootMngr();
                  if (root)                                                      {
                     TData *da = root->GetDataObject(L"LTAIS\\RealTime\\EnableEjection");
                     if (da)                                                     {
                        da->SetAsInt(1);     // enable ejection
                     }
                  }
               }

               // Desactivo los botones del panel control de maquina ya que dependen del estado de TwinCat
               BotonMarchaCinta->Down=true;
               BotonParoCinta->Down=false;

               // Actualizar etiquetas estado maquina
               EstadoAlimentacionParada->Visible=true;
               EstadoMaquinaParada->Visible=false;
               EstadoMaquinaTodoOk->Visible=false;
             }

         }

         m_prevEstadoMarcha = estadoMarcha;
      }

      // Version aplicacion para plastico:
      /*
      // -- Update panel PanelInfoProduccion for counters data
      TDataManager *mn = m_modMngr->GetManagerObject(L"LTAIS\\EjectorMan\\Reporters\\ContadorPorCategoria");
      TCounterByCategory *counterByCategory = dynamic_cast<TCounterByCategory*>(mn);

      if(mn)                                                                  {
         std::string text;

          text =  counterByCategory->GetPercentagesByCategoryAsString();
          MemoInfoClasificacion->Text = AuxStrings::utf8_to_utf16(text).c_str();
          MemoInfoClasificacion->Font->Color = clBlack;

          std::string text2;
          text2 =  counterByCategory->GetPercentagesByCategoryAsString2();
          MemoInfoClasificacion2->Text = AuxStrings::utf8_to_utf16(text2).c_str();
          MemoInfoClasificacion2->Font->Color = clBlack;

          m_pieSeries->Clear();
          int accepted = (int)(counterByCategory->GetPercentAccepted() + 0.5);
          int rejected = (int)(counterByCategory->GetPercentRejected() + 0.5);
          m_pieSeries->Add(accepted, "Aceptado", clGreen);  // Sector "Aceptado" con color verde
          m_pieSeries->Add(rejected, "Rechazado", clRed);  // Sector "Rechazado" con color rojo
          Chart3->Repaint();
      }
      */

   }
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::BotonConfigColorClick(TObject *Sender)               {

   // comprobamos si estamos en el manu ajustes si no no dejamos hacer nada.
   if (!m_menuAjustes)                                                        {
      return;
   }

   PanelAjusteColor->BringToFront();
   //deshabilito botones del panel de controles que no se pueden tocar
   BotonAjustes->Enabled= false;
   BotonConfigColor->Enabled= false;

   if ( m_formAdjustColorAux ==NULL)                                          {
      // Crear formulario de ajuste de color
      std::list<TDataManager*>::iterator it;
      for ( it= m_modMngr->DataManagerBegin(); it!= m_modMngr->DataManagerEnd();it++) {
         TDataManager *mn = *it;
		 TColorAjustFormAux *colorAjustFormAux = dynamic_cast<TColorAjustFormAux*>(mn);
         if(colorAjustFormAux)                                                {
			m_formAdjustColorAux = colorAjustFormAux;
			m_formAdjustColorAux->Init(PanelAjusteColor);
            break;
         }
      }
    //m_formAdjustColorAux =  new TColorAjustFormAux(PanelAjusteColor, m_modMngr);
   }

   if(m_formAdjustColorAux && PanelAjusteColor) {
	   m_formAdjustColorAux->Init(PanelAjusteColor);
   }
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::BotonCerrarPanelClick(TObject *Sender)               {

   PanelAjusteColor->SendToBack();

   //habilito botones del panel de controles
   BotonAjustes->Enabled= true;
   // Aplicacion para plastico
   /*
   BotonConfigColor->Enabled= true;
   */


}

//---------------------------------------------------------------------------
bool __fastcall TFormLT::CheckLicense()                                       {
   //Check license

   TDateTime t0;

   FileAge(L"LyraTechsArtificialIntelligence.exe", t0);

   System::Word AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond;

   DecodeDateTime(t0, AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond);

   int days0  = AYear * 365 + AMonth * 30 + ADay + DAYS_TO_EXPIRE_LICENSE;

   int year, month, day;

   bool ok = false;

   if(CHECK_LICENSE_ONLINE == 1)
      ok = GetServerTime(year, month, day);
   else                                                                       {
      TDateTime t = TDateTime::CurrentDate();
      DecodeDateTime(t, AYear, AMonth, ADay, AHour, AMinute, ASecond, AMilliSecond);
      year = AYear;
      month = AMonth;
      day = ADay;
      ok = true;
   }

   int days = year * 365 + month * 30 + day;

   if(DAYS_TO_EXPIRE_LICENSE != 0)                                            {
      if(!ok || days > days0 || DAYS_TO_EXPIRE_LICENSE == -1)                 {
         // DISPATCH_EVENT(ERROR_CODE, L"LICENSE_EXPIRED", L"", m_module->GetMngr());
         wchar_t buff[256];
         snwprintf( buff, 256, L"Licence expired : %d", days - days0 );
         LOG_INFO1( buff );
         return false;
      }
      else                                                                    {
         wchar_t buff[256];
         snwprintf( buff, 256, L"Days left before the license expires : %d", days0 - days );
         LOG_INFO1( buff );
      }
   }

   return true;
}

//---------------------------------------------------------------------------
bool __fastcall TFormLT::GetServerTime(int &year, int &month, int &day)       {

   UnicodeString s;

   try                                                                        {
      s = IdHTTP1->Get("http://worldtimeapi.org/api/timezone/europe/madrid");
   } catch (...)                                                              {
      return false;
   }

   std::list<std::wstring> tokens;
   TAux::ExtractTokens(s.c_str(), tokens, L",");

   if(tokens.size() < 2)                                                      {
      return false;
   }

   std::list<std::wstring>::iterator it = tokens.begin();
   std::wstring token = *it;
   std::advance(it, 2);
   token = *it;
   tokens.clear();

   TAux::ExtractTokens(token.c_str(), tokens, L":");
   it = tokens.begin();
   std::advance(it, 1);
   token = *it;
   token.erase(std::remove(token.begin(), token.end(), L'\"'), token.end());
   token.resize(10);
   tokens.clear();

   TAux::ExtractTokens(token.c_str(), tokens, L"-");
   if(tokens.size() != 3)                                                     {
      return false;
   }

   std::list<std::wstring>::iterator it2 = tokens.begin();
   token = *it2;
   year  = _wtoi( token.c_str()) ;
   std::advance(it2, 1);
   token = *it2;
   month =  _wtoi( token.c_str());
   std::advance(it2, 1);
   token = *it2;
   day   =  _wtoi( token.c_str());

   return true;
}

//---------------------------------------------------------------------------
void __fastcall TFormLT::TimerCheckLicenseTimer(TObject *Sender)              {
   TimerCheckLicense->Enabled = false;
   while(!CheckLicense())                                                     {
      // Bloquear la interfaz de usuario
      PanelFrontalInterfazUsuario->Enabled = false;
      // Parar la la aplicacion
      StopButtonClick(NULL);
      // Mostrar mensaje de licencia caducada y dejar el mensaje delante siempre
      MessageBox(NULL, L"           SU LICENCIA DE SOFTWARE NO ES VÁLIDA.\nContactar con servicio técnico de LYRA TECHS SOLUTIONS", LoadText(L"ERROR"), MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
   }
}




void __fastcall TFormLT::PanelInfoProduccionClick(TObject *Sender) {

   if(PanelInfoProduccion->Visible)                                           {
      PanelInfoProduccion->Visible = false;
   }
   else                                                                       {
      PanelInfoProduccion->Visible = true;
   }

}
//---------------------------------------------------------------------------


void __fastcall TFormLT::BotonResetDatosClick(TObject *Sender)                {

   if (m_modMngr)                                                             {
      // -- Lookfor Tdata "reset" from TCounterByCategory manager
      TData *tdata = m_modMngr->GetDataObject(L"LTAIS\\EjectorMan\\Reporters\\ContadorPorCategoria\\Reset");

      if(tdata)                                                               {
         tdata->SetAsInt(1);
         ::Sleep(100);
         tdata->SetAsInt(0);
      }
   }
}
//---------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//---------------------------------------------------------------------------


