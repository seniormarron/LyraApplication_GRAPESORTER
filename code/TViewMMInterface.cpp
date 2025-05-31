//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TViewMMInterface.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#pragma hdrstop

#include "TViewMMInterface.h"

#include "TDataMngControl.h"
#include "AdvSmoothButton.hpp"
#include "AdvSmoothGauge.hpp"
#include "AdvSmoothTrackBar.hpp"

//---------------------------------------------------------------------------

#pragma package(smart_init)


TViewMMInterface::TViewMMInterface(    TDataManager *mngr, TForm *view )
   : TViewInterface( mngr, view)                                              {

}

TViewMMInterface::~TViewMMInterface()                                       {
};



bool   TViewMMInterface::RefreshInterface ( TNode *node)                      {

   if (!node)
      return true;

   if ( !TViewInterface::RefreshInterface( node))
      return false;

   TData *tdata = dynamic_cast<TData *>(node);

   if (!tdata)
      return true;

   TNotifyEvent   e;
   TComponent    *c;
   wchar_t  txt[1024];
   bool enabled;

//      const wchar_t *name = _name ? _name : tdata->GetName();
   wchar_t name[32];
   wcscpy(name, _name ? _name : tdata->GetName());


   int            flags    = tdata ? tdata->GetFlags(): 0;
   bool           readOnly = tdata ? tdata->GetRdOnly(): (flags & FLAG_RDONLY);
   enabled        = !readOnly;

   if( !m_view)
      return false;

   for( int n = -1; n < 5; ++n)                                            {
      wchar_t _nam[256];
      if( n == -1)
         wcscpy( _nam, name);
      else
         swprintf( _nam, L"%s_%d", name, n);

      if( (c = m_view->FindComponent( _nam)) == NULL)
         continue;
      if( n == -1)
         n  = 99;

      if( dynamic_cast<TAdvSmoothGauge*>(c))                               {
         TAdvSmoothGauge &g = *(TAdvSmoothGauge *)c;
         g.Value = tdata->AsInt();
         g.Enabled = enabled;
      }
      else if( dynamic_cast<TAdvSmoothTrackBar*>(c))                       {
         TAdvSmoothTrackBarValueChanged e;
         TAdvSmoothTrackBar &tb = *(TAdvSmoothTrackBar *)c;
         e                    = tb.OnPositionChanged;
         tb.OnPositionChanged = NULL;
         int val = (tdata->AsInt());
         if (val > tb.Maximum)
            tb.Position = tb.Maximum;
         else
            tb.Position = val;

         tb.Enabled           = true;
         tb.OnPositionChanged = e;
      }
   }


   if ( tdata && tdata->AreYou( L"Init") )                                    {
      if ( tdata->AsInt())
      return true;

   }

   if ( tdata && tdata->AreYou( L"Stop") )                                    {
      if ( tdata->AsInt())
      return true;
   }

   //check ready faulty
   if ( tdata && tdata->AreYou( LBL_STATE_FAULTY))                            {
      //ToDo
      return true;
   }

  // TSpeedButton *initButton = dynamic_cast<TSpeedButton*>(m_view->FindComponent( "initButton"));
   TButton *initButton = dynamic_cast<TButton*>(m_view->FindComponent( "initButton"));

   if (!initButton)
      return true;

//   TSpeedButton *stopButton = dynamic_cast<TSpeedButton*>(m_view->FindComponent( "StopButton"));
   TButton *stopButton = dynamic_cast<TButton*>(m_view->FindComponent( "StopButton"));

   if (!stopButton)
      return true;

   //accion a efectuar cuando se pulsa el botón ChangeState, dependiendo
   //del estado de la máquina

   if  (!tdata->AreYou(LBL_STATE))
      return true;

   TNotifyEvent   e1  = initButton->OnClick;
   initButton->OnClick     = NULL;

   TNotifyEvent   e2  = stopButton->OnClick;
   stopButton->OnClick     = NULL;

   switch( tdata->AsInt())                                              {
         case 0: //closed
            initButton->Enabled = false;
            stopButton->Enabled = false;
            break;

         case 1: //configured
            initButton->Enabled = true;
            stopButton->Enabled = false;
            break;

         case 2: //inited
            initButton->Enabled = true;
            stopButton->Enabled = false;
            break;

         case 3:  //running
            initButton->Enabled = false;
            stopButton->Enabled = true;
            break;

   }
   initButton->OnClick  = e1;
   stopButton->OnClick  = e2;

   return true;
}

//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------

