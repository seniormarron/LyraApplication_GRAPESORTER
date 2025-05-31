//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TAuxProcessTime.h
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef TAuxProcessTimeH
#define TAuxProcessTimeH
//---------------------------------------------------------------------------

#include <vcl.h>
#include "TModuleMngr.h"


class TAuxProcessTime : public TDataManagerControl                            {

   protected:
    TModuleMngr  *m_manager;
    TListView     *m_listView,   ///< listView sere processes time will be shown
                  *m_listView3;
    TPanel        *m_panel;
    TButton       *m_startBt,
                  *m_resetBt;
    bool          m_started;
    TTimer        *m_timer;      ///< timer for refreshing list with process times ( it is not created to avoid traffic of messages
    int           m_reset;

   public:
    TAuxProcessTime( TPanel *panel, TTimer *timer,  const wchar_t *name, TModuleMngr *mn);
    virtual ~TAuxProcessTime();

    virtual bool MyInit();
    virtual bool MyStop();
    virtual bool MyClose();
    bool RefreshTimes();
    TPanel  *GetPanel()                                                       {
      return m_panel;
    }
   protected:
    void SetCalcProcessTimes( TDataManager *mn, bool value);
    void SetCalcProcessTimesRec( TDataManager *mn, bool value, int *pContador);
    void __fastcall OnStartClick(TObject *Sender);
    void __fastcall OnResetClick(TObject *Sender);
    void StartTimes();
    void StopTimes();
};

#endif




















//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
