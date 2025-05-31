//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
                  Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TLoadCfgDlg.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              15/03/2021
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TLoadCfgDlg.h"
#include "stdio.h"
#include "definesLang.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TLoadCfgDlg *LoadCfgDlg;

//---------------------------------------------------------------------------
__fastcall TLoadCfgDlg::TLoadCfgDlg(TComponent* Owner, TPersistence *persistence, bool user)
   : TForm(Owner)                                                             {

   m_user        = user;
   m_persistence = persistence;
   this->LabelSelect->Caption    = LoadText( L"Select");
   // No muestro estos controles de momento solo contemplamos carpeta local siempre, no en externa en usb
   this->localcb->Caption        = LoadText( L"HomeFolder");
   this->localcb->Visible=false;
   // No muestro estos controles de momento solo contemplamos carpeta local siempre, no en externa en usb
   this->externalcb->Caption     = LoadText( L"ExternalDisk");
   this->externalcb->Visible=false;
   this->okbt->Caption           = LoadText( L"Load");
   this->cancelbt->Caption       = LoadText( L"Cancel");
   this->ButtonDelete->Caption   = LoadText( L"Delete");
   this->infolb->Caption         = LoadText( L"SelectConf");

   PopupParent = Application->MainForm;
}

//------------------------------------------------------------------------------


void   TLoadCfgDlg::SetLoadCfg(wchar_t* folder)                               {
   cfglistbox->Clear();
   //update list box
   int cont = 0;
   TSearchRec    m_sr;

   // Find the path
   wchar_t        fPath[2048];
   if ( wcscmp( folder, L"")!=0)
      swprintf( fPath, L"%s\\*.xml", folder );
   else
      swprintf( fPath, L"*.xml" );

   cfglistbox->Clear();
   unsigned int len = 0;

   wchar_t        fPathFile[2048];
   if( FindFirst( fPath, faArchive, m_sr) == 0 )                              {
      do                                                                      {
         swprintf( fPathFile, L"%s\\%s", folder, m_sr.Name.c_str());
         if ( m_persistence==NULL || m_persistence->ValidateMachineCodeInFile( fPathFile))   {
//         if ( m_persistence==NULL)   {


            cont++;

            cfglistbox->AddItem(m_sr.Name.c_str(),0);

            if (wcslen( m_sr.Name.c_str() ) > len )
               len = wcslen( m_sr.Name.c_str());
         }
      }while ((FindNext( m_sr ) == 0) && (cont<2000));
   }
   if (len > 0)
      cfglistbox->ScrollWidth = (len )* cfglistbox->Font->Size;


   FindClose( m_sr);

}

//------------------------------------------------------------------------------


void __fastcall TLoadCfgDlg::FormDestroy(TObject *Sender)
{
   cfglistbox->Clear();
}
//------------------------------------------------------------------------------

void __fastcall TLoadCfgDlg::localcbClick(TObject *Sender)
{
   if (localcb->Checked)   {
      externalcb->Checked  = false;
      SetLoadCfg( LocalDir->Caption.c_str());
   }
   else
      if (!externalcb->Checked)
         localcb->Checked  = true;
}
//---------------------------------------------------------------------------
void __fastcall TLoadCfgDlg::externalcbClick(TObject *Sender)
{
   if (externalcb->Checked)   {
      localcb->Checked     = false;
      SetLoadCfg( ExternalDir->Caption.c_str());
   }
   else if (!localcb->Checked)
         externalcb->Checked  = true;

}
//---------------------------------------------------------------------------

void __fastcall TLoadCfgDlg::FormShow(TObject *Sender)
{

   if ( m_user)                                                               {
      BtnLoad->Visible = false;
       // No muestro estos controles de momento solo contemplamos carpeta local siempre, no en externa en usb
 //     localcb->Visible = true;
 //     externalcb->Visible = true;
   }
   else                                                                       {
      BtnLoad->Visible = true;
      localcb->Visible = false;
      externalcb->Visible = false;
   }

   SetLoadCfg( LocalDir->Caption.c_str());
}
//---------------------------------------------------------------------------


void __fastcall TLoadCfgDlg::ButtonDeleteClick(TObject *Sender)               {

   int index = cfglistbox->ItemIndex;
   if ( index < 0)
      return;
   // Confirmacion de borrado

   if( MessageBox(NULL, LoadText( L"DeleteConfigConfirm"), LoadText(L"WARNING"), MB_YESNO) == IDYES)    {
      UnicodeString strToLoad = cfglistbox->Items->operator[](cfglistbox->ItemIndex);
      std::wstring tmp;

      if (m_user)                                                             {
         if ( localcb->Checked)
            tmp = std::wstring(LocalDir->Caption.c_str());
         else if ((externalcb->Enabled) && (externalcb->Checked))
            tmp = std::wstring(ExternalDir->Caption.c_str());
      }
      else
         tmp = std::wstring(LocalDir->Caption.c_str());


      tmp = tmp + L"\\" + strToLoad.c_str();
      //borrar fichero
      DeleteFile( tmp.c_str());

      //volver a cargar
      if (externalcb->Checked)                                                {
          SetLoadCfg( ExternalDir->Caption.c_str());
      }
      else if (localcb->Checked)                                              {
         SetLoadCfg( LocalDir->Caption.c_str());
      }

   }
}
//---------------------------------------------------------------------------

void __fastcall TLoadCfgDlg::BtnLoadClick(TObject *Sender)
{
   TOpenDialog *dlg  = new TOpenDialog(this);
   dlg->Options << ofNoChangeDir;

   dlg->Filter = "Configuration files (*.xml)|*.XML";
   dlg->InitialDir = LocalDir->Caption;
   if( !dlg->Execute())                                                       {
      delete dlg;
      return;
   }

   LocalDir->Caption  = ExtractFileDir(dlg->FileName);
   SetLoadCfg( ExtractFileDir(dlg->FileName).c_str());
   delete dlg;
}
//---------------------------------------------------------------------------





//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------



