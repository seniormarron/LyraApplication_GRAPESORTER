//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TAuxProcessTime.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#pragma hdrstop

#include "TAuxProcessTime.h"
#include "TModuleMngr.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TAuxProcessTime::TAuxProcessTime( TPanel *panel, TTimer *timer, const wchar_t *name, TModuleMngr *mn)
   : TDataManagerControl( name, mn)                                           {

   m_manager   = mn;
   m_panel     = panel;
   m_timer     = timer;

   int MARGIN_WIDTH  = 90;
   int MARGIN_HEIGHT = 20;
   int MARGIN_TOP    = 10;
   int MARGIN_LEFT   = 10;

   if ( m_panel ==NULL)
      return;
   //start button
   m_startBt = new TButton(m_panel);
   m_startBt->SetParentComponent(m_panel);
   m_startBt->Name = "Start";
   m_startBt->Width = MARGIN_WIDTH;
   m_startBt->Height = MARGIN_HEIGHT;
   m_startBt->Left = MARGIN_LEFT;
   m_startBt->Top = MARGIN_TOP;
   m_startBt->OnClick = OnStartClick;
   m_startBt->Visible = true;
   m_startBt->Font->Size =  10;

   //stop button
   m_resetBt = new TButton(m_panel);
   m_resetBt->SetParentComponent(m_panel);
   m_resetBt->Name = "Reset";
   m_resetBt->Width = MARGIN_WIDTH;
   m_resetBt->Height = MARGIN_HEIGHT;
   m_resetBt->Left = m_panel->Width - m_resetBt->Width - 10;
   m_resetBt->Top = MARGIN_TOP;
   m_resetBt->OnClick = OnResetClick;
   m_resetBt->Visible = true;
   m_resetBt->Font->Size =  10;
   m_resetBt->Enabled   = false;

   //process times list
   m_listView   = new TListView(panel); // The owner will clean this up.
   m_listView->Top   =  m_resetBt->Top + m_resetBt->Height + 10;
   m_listView->Parent = panel;
   //m_listView->Align = alClient;
   m_listView->Height   = ( m_panel->Height - m_listView->Top)*3/4 ;
   m_listView->Width    = m_panel->Width - 20;
   m_listView->Left     = MARGIN_LEFT;
   m_listView->ViewStyle = vsReport;
   m_listView->Font->Size = 10;

   TListColumn  *newColumn;
   newColumn = m_listView->Columns->Add();
   newColumn->Width   = 200;
   newColumn->Caption = "Process";

   newColumn = m_listView->Columns->Add();
   newColumn->Width   = 150;
   newColumn->Caption = "Last Time ( ms)";

   newColumn = m_listView->Columns->Add();
   newColumn->Width   = 150;
   newColumn->Caption = "Max Time ( ms)";

   newColumn = m_listView->Columns->Add();
   newColumn->Width   = 150;
   newColumn->Caption = "Thread";

   //delays
   m_listView3          = new TListView(panel);
   m_listView3->Top     = m_listView->Top + m_listView->Height + 10;
   m_listView3->Parent  = panel;
   //m_listView->Align = alClient;
   m_listView3->Height  =  m_panel->Height + m_listView3->Top;
   m_listView3->Width   = m_panel->Width - 20;
   m_listView3->Left    = 10;
   m_listView3->ViewStyle = vsReport;
   m_listView3->Font->Size =  10;

   newColumn = m_listView3->Columns->Add();
   newColumn->Width   = 200;
   newColumn->Caption = "Capture Last Delay (ms)";

   newColumn = m_listView3->Columns->Add();
   newColumn->Width   = 200;
   newColumn->Caption = "Max Delay ( ms)";

   TListItem *item   = m_listView3->Items->Add();
   item->Caption     =  L"0.0";

   //add subitem of time
   item->SubItems->Add( 0);

   m_started   = false;
   m_reset = false;

}

//------------------------------------------------------------------------------

TAuxProcessTime::~TAuxProcessTime()                                           {

}

//------------------------------------------------------------------------------

/**
   We overwrite MyClose method to avoid problems when refreshing pointers to deleted objects
*/

bool TAuxProcessTime::MyClose()                                                {
   if ( m_timer)
      m_timer->Enabled = false;
   return TDataManagerControl::MyClose();

}

//------------------------------------------------------------------------------

bool TAuxProcessTime::MyStop()                                                {

   StopTimes();
   return TDataManagerControl::MyStop();

}

//------------------------------------------------------------------------------

bool TAuxProcessTime::MyInit()                                                {
   if ( m_timer)
      m_timer->Enabled = false;

   return TDataManagerControl::MyInit();

}

//------------------------------------------------------------------------------

void __fastcall TAuxProcessTime::OnStartClick(TObject *Sender)                {

   if ( m_started==false)                                                     {
      StartTimes();
   }
   else                                                                       {
      StopTimes();
   }
}

//------------------------------------------------------------------------------

void TAuxProcessTime::StartTimes()                                             {

      m_listView->Items->Clear();
      SetCalcProcessTimes( m_manager, true);
      m_timer->Enabled = true;
      m_started = true;
      m_startBt->Caption   = L"Stop";
      m_resetBt->Enabled   = true;

}
//------------------------------------------------------------------------------

void TAuxProcessTime::StopTimes()                                                  {

      m_timer->Enabled = false;
      SetCalcProcessTimes( m_manager, false);
      m_started = false;
      m_startBt->Caption = L"Start";
      m_resetBt->Enabled   = false;

}
//------------------------------------------------------------------------------

void __fastcall TAuxProcessTime::OnResetClick(TObject *Sender)                {


   m_reset    = 2; //nest time refreshtimes is executed, reset will be required
   //we set it to 2, because if RefreshTimes is executing when we push reset button, some
   //times could not be reseted. SO we do it twice

}

//------------------------------------------------------------------------------

bool  TAuxProcessTime::RefreshTimes( )                           {

   if ( m_state.state != running)
      return false;

   //refresh process times
   for ( int i=0; i < m_listView->Items->Count; i++)                          {
      TListItem *item = m_listView->Items->Item[i];
      TProcess *p =  (TProcess*)item->Data;
      if ( p)                                                                 {
         //set time
         wchar_t time[32];
         unsigned int size = SIZEOF_CHAR( time);
         float t = p->GetProcessTime();
         swprintf( time, L"%.2f", t);
         item->SubItems->Strings[0] = time;
         t = p->GetMaxProcessTime( m_reset);
         swprintf( time, L"%.2f", t);
         item->SubItems->Strings[1] = time;
      }

   }


   //refresh delays
   if ( m_manager)                                                            {
         wchar_t time[32];
         swprintf( time,L"%.2f", m_manager->GetDelayTime());
         m_listView3->Items->Item[0]->Caption = time;
         swprintf( time,L"%.2f", m_manager->GetMaxDelayTime( m_reset));
         m_listView3->Items->Item[0]->SubItems->Strings[0] = time;
   }

   if ( m_reset > 0)
      m_reset--;

   return true;

}

//------------------------------------------------------------------------------

/**
   This method looks for  processes and inserts name and pointer to time TData the list
*/

void TAuxProcessTime::SetCalcProcessTimes( TDataManager *mn, bool value)      {


   std::list<TDataManager*>::iterator i;
   int   contador = 0;
   if ( mn==NULL)
      return;

   SetCalcProcessTimesRec( mn, value, &contador);


}


//------------------------------------------------------------------------------


void TAuxProcessTime::SetCalcProcessTimesRec( TDataManager *mn, bool value, int *pContador)   {


   TProcess *p = dynamic_cast<TProcess*>(mn);
   if ( p && p->Enabled())                                                 {
      //añadir a la lista
      TData *data = p->GetDataObject( LBL_ENABLE_PROCESS_TIME);
      if (data )     {
            data->SetAsInt( value);
            if ( value)                                                       {
               //si value es true, ademas creamos entradas en el ListView

                  TListItem *item = m_listView->Items->Add();
                  item->Caption  =  data->GetParent()->GetLangName();
                  item->Data     =  p;    //store pointer to tdata

                  //add subitem of time
                  item->SubItems->Add( 0);
                  //add subitem of maxtime
                  item->SubItems->Add( 0);
                  (*pContador)++;


                  data =  p->GetDataObject( LBL_THREAD);
                  if ( data)                                                     {

                     //add subitem of time
                     item->SubItems->Add( data->AsInt());
                  }



         }

      }
   }

   std::list<TDataManager*>::iterator i;
   for ( i= mn->DataManagerBegin(); i!= mn->DataManagerEnd();i++)             {
         //seguir buscando
         SetCalcProcessTimesRec( *i, value, pContador);
   }
}


//
//void TAuxProcessTime::SetCalcProcessTimesRec( TDataManager *mn, bool value, int *pContador)   {
//
//   std::list<TDataManager*>::iterator i;
//   for ( i= mn->DataManagerBegin(); i!= mn->DataManagerEnd();i++)             {
//      TProcess *p = dynamic_cast<TProcess*>(*i);
//      if ( p && p->Enabled())                                                 {
//         //añadir a la lista
//         TData *data = p->GetDataObject( LBL_ENABLE_PROCESS_TIME);
//         if (data )     {
//            data->SetAsInt( value);
//            if ( value)                                                       {
//               //si value es true, ademas creamos entradas en el ListView
//               data =  p->GetDataObject( LBL_PROCESS_TIME);
//               if ( data)                                                     {
//
//                  TListItem *item = m_listView->Items->Add();
//                  item->Caption  =  data->GetParent()->GetName();
//                  item->Data     =  data;    //store pointer to tdata
//
//                  //add subitem of time
//                  item->SubItems->Add( 0);
//                  //add subitem of maxtime
//                  item->SubItems->Add( 0);
//                  (*pContador)++;
//               }
//
//               else                                                           {
//                  //dispatch event
//               }
//               data = p->GetDataObject(  LBL_THREAD);
//               if ( data && data->AsInt())                                    {
//                  TData *data2 = p->GetDataObject(  LBL_THREAD_TIME);
//                  if ( data2)             {
//                     TListItem *item = m_listView2->Items->Add();
//                     item->Caption  =  data2->GetParent()->GetName();
//                     item->Data     =  data2;    //store pointer to tdata
//
//                     //add subitem of time
//                     item->SubItems->Add( 0);
//                  }
//               }
//
//            }
//         }
//         //seguir buscando
//         SetCalcProcessTimesRec( *i, value, pContador);
//
//      }
//   }
//}



























//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------






