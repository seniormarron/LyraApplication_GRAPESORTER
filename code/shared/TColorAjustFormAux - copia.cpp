//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
            Lyra Techs Artificial Intelligence Software
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
   File:                      TColorAjustForm.cpp
   Compilation group:         LyraTechsArtificial Intelligence.exe
   Subgroup                   shared
   Version:                   1.0
   Author:                    Daniel Ivorra
   Date Created:              02/01/2023
   Date Last Modification:
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#include <vcl.h>
#pragma hdrstop

#include "TColorAjustFormAux.h"
#include "TMyMachineModuleMngr.h"

#pragma package(smart_init)
#pragma link "AdvSmoothButton"
#pragma link "AdvSmoothToggleButton"

const int columns = 16;
const int rows    = 9;

// -- Definición de visualización de colores en hexadecimal del panel de ajuste de colores
const System::UnicodeString colors[144] = {"$00101010", "$00202020", "$002F2F2F", "$003F3F3F", "$004F4F4F", "$005F5F5F", "$006F6F6F", "$007F7F7F", "$008F8F8F", "$009F9F9F", "$00AFAFAF", "$00BFBFBF", "$00CFCFCF", "$00DFDFDF", "$00EFEFEF", "$00FFFFFF",
                                           "$000C004C", "$0000104C", "$00002D4C", "$0000494C", "$00004C33", "$00004C16", "$00074C00", "$00234C00", "$00404C00", "$004C3C00", "$004C2000", "$004C0300", "$004C0019", "$004C0036", "$0046004C", "$0029004C",
                                           "$00120070", "$00001770", "$00004170", "$00006C70", "$0000704B", "$00007021", "$000A7000", "$00347000", "$005E7000", "$00705800", "$00702E00", "$00700400", "$00700025", "$0070004F", "$00660070", "$003C0070",
                                           "$00180094", "$00001F94", "$00005694", "$00008E94", "$00009462", "$0000942B", "$000D9400", "$00449400", "$007C9400", "$00947400", "$00943D00", "$00940600", "$00940031", "$00940068", "$00870094", "$004F0094",
                                           "$001D00B8", "$000026B8", "$00006BB8", "$0000B0B8", "$0000B87A", "$0000B835", "$0010B800", "$0055B800", "$0099B800", "$00B89100", "$00B84C00", "$00B80700", "$00B8003D", "$00B80082", "$00A700B8", "$006200B8",
                                           "$002300DB", "$00002EDB", "$000080DB", "$0000D2DB", "$0000DB92", "$0000DB40", "$0013DB00", "$0065DB00", "$00B7DB00", "$00DBAD00", "$00DB5A00", "$00DB0800", "$00DB0049", "$00DB009B", "$00C800DB", "$007500DB",
                                           "$002900FF", "$000035FF", "$000095FF", "$0000F4FF", "$0000FFA9", "$0000FF4A", "$0016FF00", "$0075FF00", "$00D5FF00", "$00FFC900", "$00FF6900", "$00FF0A00", "$00FF0054", "$00FF00B4", "$00E800FF", "$008900FF",
                                           "$00765CFF", "$005C7EFF", "$005CBBFF", "$005CF8FF", "$005CFFC8", "$005CFF8B", "$006AFF5C", "$00A7FF5C", "$00E4FF5C", "$00FFDC5C", "$00FF9F5C", "$00FF625C", "$00FF6075", "$00FF5CCF", "$00F05CFF", "$00B35CFF",
                                           "$00BFB2FF", "$00B2C2FF", "$00B2DFFF", "$00B2FCFF", "$00B2FFE5", "$00B2FFC9", "$00B9FFB2", "$00D6FFB2", "$00F2FFB2", "$00FFEFB2", "$00FFD2B2", "$00FFB8A9", "$00FFB5B2", "$00FFB2E9", "$00F8B2FF", "$00DBB2FF"};

// -- Valores de H por celdas filas y columnas: Primera fila grises siguientes colores
const int h[144]   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 62,125, 187, 250, 312, 375, 437, 500, 562, 625, 687, 750, 812, 875, 937,
0, 62,125, 187, 250, 312, 375, 437, 500, 562, 625, 687, 750, 812, 875, 937,
0, 62,125, 187, 250, 312, 375, 437, 500, 562, 625, 687, 750, 812, 875, 937,
0, 62,125, 187, 250, 312, 375, 437, 500, 562, 625, 687, 750, 812, 875, 937,
0, 62,125, 187, 250, 312, 375, 437, 500, 562, 625, 687, 750, 812, 875, 937,
0, 62,125, 187, 250, 312, 375, 437, 500, 562, 625, 687, 750, 812, 875, 937,
0, 62,125, 187, 250, 312, 375, 437, 500, 562, 625, 687, 750, 812, 875, 937,
0, 62,125, 187, 250, 312, 375, 437, 500, 562, 625, 687, 750, 812, 875, 937};

//// -- Valores de I por celdas filas y columnas: Primera fila grises siguientes colores
//const int i[144]   = {0, 62, 125, 187, 250, 312, 375, 437, 500, 562, 625, 687, 750, 812, 875, 937,
//111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111,
//222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222, 222,
//333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333,
//444, 444, 444, 444, 444, 444, 444, 444, 444, 444, 444, 444, 444, 444, 444, 444,
//555, 555, 555, 555, 555, 555, 555, 555, 555, 555, 555, 555, 555, 555, 555, 555,
//666, 666, 666, 666, 666, 666, 666, 666, 666, 666, 666, 666, 666, 666, 666, 666,
//777, 777, 777, 777, 777, 777, 777, 777, 777, 777, 777, 777, 777, 777, 777, 777,
//888, 888, 888, 888, 888, 888, 888, 888, 888, 888, 888, 888, 888, 888, 888, 888};

// -- Valores de I por celdas filas y columnas: Primera fila grises siguientes colores
const int i[144]   = {0, 254, 511, 765, 1023, 1277, 1535, 1789, 2047, 2301, 2559, 2813, 3071, 3325, 3583, 3837,
500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800,
1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800, 1800,
2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500,
3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000,
3500, 3500, 3500, 3500, 3500, 3500, 3500, 3500, 3500, 3500, 3500, 3500, 3500, 3500, 3500, 3500};

// Saturation pattern
// Primera fila    S[0-300]     // Estos valores se colocan en configuracion por TData , para las categorias Blanco, Negro, Gris
// Resto de filas  S[300-1000]  // Estos valores se colocan en configuracion por TData, para las ctegorias Azul, Verde, Rojo, etc... (colores)



//---------------------------------------------------------------------------
TColorAjustFormAux::TColorAjustFormAux(const wchar_t*name, TMyMachineModuleMngr *modMngr) :
	TDataManager(name, modMngr)                                                {

//   // Tdtas max y min S para primera fila de grises
//   m_daSMinFilaGris    =  new TData( this, L"MinS_grises",  &m_sMinFilaGris ,   0,    0, 999,   NULL , FLAG_NORMAL, L"Minimo valor de S para fila de grises"  );
//   m_daSMaxFilaGris    =  new TData( this, L"MaxS_grises",  &m_sMaxFilaGris ,   300,  0, 999,   NULL , FLAG_NORMAL, L"Maximo valor de S para fila de grises"  );
//   // Tdtas max y min S para resto de filas de colores
//   m_daSMinFilaColor   =  new TData( this, L"MinS_colores", &m_sMinFilaColor ,   301,  0, 999,   NULL , FLAG_NORMAL, L"Minimo valor de S para fila de colores"  );
//   m_daSMaxFilaColor   =  new TData( this, L"MaxS_colores", &m_sMaxFilaColor ,   999, 0, 999,   NULL , FLAG_NORMAL, L"Maximo valor de S para fila de colores"  );


	m_modMngr = modMngr;
}
//---------------------------------------------------------------------------

void __fastcall TColorAjustFormAux::Init(TPanel *panel) {
	Panel1 = dynamic_cast<TPanel*>(panel->FindChildControl(L"PanelFormAjustColor"));
	if(!Panel1)
	   return;

	for (int i = 0; i < Panel1->ControlCount; ++i)
	{
		TAdvSmoothToggleButton *celda = dynamic_cast<TAdvSmoothToggleButton*>(Panel1->Controls[i]);
		if(celda) {
			TColor color = StringToColor(colors[celda->TabOrder]);
			celda->BorderInnerColor = color;
			celda->Color = color;
			celda->ColorDisabled = color;
			celda->ColorDown = color;
			celda->BevelColorDown = clLime;
			celda->BevelWidth = 0;
			celda->OnClick = C1Click;
			celda->OnDblClick = DblClick;
		}
	}

	m_comboBox = dynamic_cast<TComboBox*>(panel->FindChildControl(L"ColorAdjustComboBox"));

	this->InitComboBox();
}

void __fastcall TColorAjustFormAux::InitComboBox() {
	if(!m_comboBox)
		return;

	m_preclassificMngrs.clear();
	m_ltBeltDiscrMngrs.clear();
	m_comboBox->Clear();

	std::list<TDataManager *>::iterator it = m_modMngr->DataManagerBegin();
	std::list<TDataManager *>::iterator itEnd = m_modMngr->DataManagerEnd();
	while ( it != itEnd)                                                       {
	  TPreclassificMngr* preclassMngr = dynamic_cast<TPreclassificMngr*>(*it);
	  if (preclassMngr && preclassMngr->Enabled()) {
		 m_preclassificMngrs.push_back(preclassMngr);
		 m_comboBox->AddItem(preclassMngr->GetLangName(), 0);
	  }

	  LTBeltDiscr_RGBHSI* beltDiscrMngr = dynamic_cast<LTBeltDiscr_RGBHSI*>(*it);
	  if (beltDiscrMngr && beltDiscrMngr->Enabled()) {
		 m_ltBeltDiscrMngrs.push_back(beltDiscrMngr);
	  }
	  it++;
   }

   m_comboBox->OnChange = onChange;

   m_comboBox->ItemIndex = 0;

   onChange(NULL);
}

//------------------------------------------------------------------------------
TColorAjustFormAux::~TColorAjustFormAux()                                 {


}

void __fastcall TColorAjustFormAux::C1Click(TObject *Sender) {

	//Update HSI values of preclass
	TPreclassificMngr *preclassificMngr = m_preclassificMngrs[m_comboBox->ItemIndex];
	if(!preclassificMngr)
		return;
	TPreclassific *preclass = preclassificMngr->GetPreclassificNode(1);
	if(!preclass)
		return;
	TRange* iAvg = dynamic_cast<TRange*>(preclass->GetManagerObject(L"IAvg"));
	TRange* hAvg = dynamic_cast<TRange*>(preclass->GetManagerObject(L"HAvg"));
	TRange* sAvg = dynamic_cast<TRange*>(preclass->GetManagerObject(L"SAvg"));

	if(!iAvg || !hAvg || !sAvg) {
	   //TODO MESSAGE ERROR
	   return;
	}
   // Cambiar el aspecto del boton si se pulsa o despulsa
	TAdvSmoothToggleButton *celda = dynamic_cast<TAdvSmoothToggleButton*>(Sender);
	if ( celda)                                                                {
	  if (celda->Down)                                                        {
		 celda->BevelWidth = 8;
	  }
	  else                                                                    {
		 celda->BevelWidth = 0;
	  }

	  //Toogle or Untoogle buttons in function of HSI values

		int hMin = 0;
		int hMax = 10000;
//		int sMin = 10000;
//		int sMax = 0;
		int iMin = 40950;
		int iMax = 0;

		GetMinMax(hMin, hMax ,/* sMin, sMax,*/ iMin, iMax);

		if(!iAvg->Enabled()) {
		   iMin = 0;
		   iMax = 40950;
		}
		if(!hAvg->Enabled()) {
		   hMin = 0;
		   hMax = 10000;
		}
		if(!sAvg->Enabled()) {
//		   sMin = 0;
//		   sMax = 10000;
		}

		for (int index = 0; index < Panel1->ControlCount; ++index) {
			TAdvSmoothToggleButton *celdai = dynamic_cast<TAdvSmoothToggleButton*>(Panel1->Controls[index]);
			if(celdai && !celdai->Down) {


			/**/
			int hMini = hMin;
			int iMini = i[celdai->TabOrder];
//			int sMini = sMin;
			int hMaxi = hMax;
			int iMaxi = i[celdai->TabOrder];
//			int sMaxi = sMax;

			//S
			if(celdai->TabOrder < columns) {
//			   sMini = m_sMinFilaGris;
//			   sMaxi = m_sMaxFilaGris;
			} else {
//			   sMini = m_sMinFilaColor;
//			   sMaxi = m_sMaxFilaColor;
			}

			//Hmax
			if(celdai->TabOrder < columns) {
			   hMaxi = (h[columns + 1] - 1);
			} else if(celdai->TabOrder % columns == columns - 1) {
			   hMini = h[celdai->TabOrder];
			   hMaxi = 1000;
			} else {
			   hMini = h[celdai->TabOrder];
			   hMaxi = h[celdai->TabOrder + 1] - 1;
			}
			//Imax
			if(celdai->TabOrder < columns) {
			   if(celdai->TabOrder == (columns - 1)) {
				   iMaxi = 4095;
			   }
			   else {
				   iMaxi = (i[celdai->TabOrder + 1] - 1);
			   }
			}
			else if((celdai->TabOrder / columns) == (rows - 1)) {
			   iMaxi = 4095;
			} else {
			   iMaxi = (i[celdai->TabOrder + columns] - 1);
			}

			if(celdai->TabOrder < columns) {
				if((iMini >= iMin) &&
				   (iMaxi <= iMax) /*&&
				   (sMini >= sMin) &&
				   (sMaxi <= sMax)*/ ){
					  celdai->Down = true;
					  celdai->BevelWidth = 8;
				}
			}
			else {
				 if((hMini >= hMin) &&
				   (hMaxi <= hMax) &&
				   (iMini >= iMin) &&
				   (iMaxi <= iMax) /*&&
				   (sMini >= sMin) &&
				   (sMaxi <= sMax) */ ){
					  celdai->Down = true;
					  celdai->BevelWidth = 8;
				}
			}

		}
	}

	if(hAvg->Enabled()) {
		hAvg->SetMinMax(hMin, hMax, NULL);
	}
	if(iAvg->Enabled()) {
		iAvg->SetMinMax(iMin, iMax, NULL);
	}
	if(sAvg->Enabled()) {
//		sAvg->SetMinMax(sMin, sMax, NULL);
	}

	//Update HSI values of BeltDiscr

	for(int i = 0; i < m_ltBeltDiscrMngrs.size(); i++) {


		LTBeltDiscr_RGBHSI *ltBeltDiscrMngr = m_ltBeltDiscrMngrs[m_comboBox->ItemIndex];
		if(!ltBeltDiscrMngr || !ltBeltDiscrMngr->Enabled())
			continue;

		if(wcscmp(ltBeltDiscrMngr->GetLangName(), preclassificMngr->GetLangName()) != 0)
		   continue;


		TDataManager* ranges = ltBeltDiscrMngr->GetManagerObject(L"Ranges");
		if(!ranges)
			continue;
		TData* daIMin = (ranges->GetDataObject(L"I_Min"));
		TData* daIMax = (ranges->GetDataObject(L"I_Max"));
		TData* daHMin = (ranges->GetDataObject(L"h_Min"));
		TData* daHMax = (ranges->GetDataObject(L"h_Max"));

		if(!daIMin || !daIMax || !daHMin || !daHMax) {
		   //TODO MESSAGE ERROR
		   continue;
		}

		daIMin->SetAsInt(iAvg->Min());
		daIMax->SetAsInt(iAvg->Max());
		daHMin->SetAsInt(hAvg->Min());
		daHMax->SetAsInt(hAvg->Max());
	}

   }

}

void __fastcall TColorAjustFormAux::DblClick(TObject *Sender) {
	UpAllButtons();
	C1Click(Sender);
}

void __fastcall TColorAjustFormAux::GetMinMax(int &hMin, int &hMax,/* int &sMin, int &sMax,*/ int &iMin, int &iMax) {
	hMin = 0;
	hMax = 10000;

	iMin = 40950;
	iMax = 0;

//	sMin = 10000;
//	sMax = 0;

	for (int index = 0; index < Panel1->ControlCount; ++index) {
		TAdvSmoothToggleButton *celdai = dynamic_cast<TAdvSmoothToggleButton*>(Panel1->Controls[index]);
		if(celdai && celdai->Down) {
			//S
			if(celdai->TabOrder < columns) {
//			   if((m_sMinFilaGris) <= sMin)sMin = m_sMinFilaGris;
//			   if((m_sMaxFilaGris) >= sMax)sMax = m_sMaxFilaGris;
			} else {
//			   if((m_sMinFilaColor) <= sMin)sMin = m_sMinFilaColor;
//			   if((m_sMaxFilaColor) >= sMax)sMax = m_sMaxFilaColor;
			}

			//H
			if(celdai->TabOrder < columns) {
			   //if((h[columns + 1] - 1) >= hMax)hMax = (h[columns + 1] - 1);
			} else if(celdai->TabOrder % columns == columns - 1) {
			   if(h[celdai->TabOrder] <= hMin) hMin = h[celdai->TabOrder];
			   if(1000 >= hMax) hMax = 1000;
			} else {
			   if(h[celdai->TabOrder] <= hMin) hMin = h[celdai->TabOrder];
			   if((h[celdai->TabOrder + 1] - 1) >= hMax) hMax = h[celdai->TabOrder + 1] - 1;
			}

			//Imin
			if(i[celdai->TabOrder] <= iMin) iMin = i[celdai->TabOrder];
			//Imax
			if(celdai->TabOrder < columns) {
			   if(celdai->TabOrder == (columns - 1)) {
				   if(4095 >= iMax) iMax = 4095;
			   }
			   else {
				   if((i[celdai->TabOrder + 1] - 1) >= iMax) iMax = (i[celdai->TabOrder + 1] - 1);
			   }
			}
			else if((celdai->TabOrder / columns) == (rows - 1)) {
			   if((4095) >= iMax) iMax = 4095;
			} else {
			   if((i[celdai->TabOrder + columns] - 1) >= iMax) iMax = (i[celdai->TabOrder + columns] - 1);
			}
		}
	}
}

//---------------------------------------------------------------------------

void __fastcall TColorAjustFormAux::onChange(TObject *Sender){

	UpAllButtons();

	TPreclassificMngr *preclassificMngr = m_preclassificMngrs[m_comboBox->ItemIndex];

	TPreclassific *preclass = NULL;

	std::list<TDataManager *>::iterator it = preclassificMngr->DataManagerBegin();
	std::list<TDataManager *>::iterator end = preclassificMngr->DataManagerEnd();

	while (it != end) {
	   preclass = dynamic_cast<TPreclassific*>(*it);
	   if(preclass) {
		  break;
	   }
	   it++;
	}

	if(!preclass) {
	   return;
	}

	TRange* iAvg = dynamic_cast<TRange*>(preclass->GetManagerObject(L"IAvg"));
	TRange* hAvg = dynamic_cast<TRange*>(preclass->GetManagerObject(L"HAvg"));
	TRange* sAvg = dynamic_cast<TRange*>(preclass->GetManagerObject(L"SAvg"));

	if(!iAvg || !hAvg || !sAvg) {
	   //TODO MESSAGE ERROR
	   return;
	}

	int hMin = hAvg->Min();
	int hMax = hAvg->Max();

	int iMin = iAvg->Min();
	int iMax = iAvg->Max();

	int sMin = sAvg->Min();
	int sMax = sAvg->Max();

	if(!hAvg->Enabled()) {
	   hMin = 0;
	   hMax = 10000;
	}

	if(!iAvg->Enabled()) {
	   iMin = 0;
	   iMax = 40950;
	}

	if(!sAvg->Enabled()) {
	   sMin = 0;
	   sMax = 10000;
	}
	//Toogle or Untoogle buttons in function of HSI values
	for (int index = 0; index < Panel1->ControlCount; ++index) {
		TAdvSmoothToggleButton *celda = dynamic_cast<TAdvSmoothToggleButton*>(Panel1->Controls[index]);
		if(celda) {
			int indexTab = celda->TabOrder;

			if(indexTab < columns) {
				if(i[indexTab] >= iMin &&
					i[indexTab] <= iMax /*&&
				  	m_sMinFilaGris >= sMin &&
					m_sMaxFilaGris <= sMax*/) {
					celda->Down = true;
					celda->BevelWidth = 8;
				}
			} else {
				if(h[indexTab] >= hMin &&
				   h[indexTab] <= hMax &&
				   i[indexTab] >= iMin &&
				   i[indexTab] <= iMax /*&&
				   m_sMinFilaColor >= sMin &&
				   m_sMaxFilaColor <= sMax*/) {
					celda->Down = true;
					celda->BevelWidth = 8;

				}

			}
		}
	}
}

//
void __fastcall TColorAjustFormAux::UpAllButtons() {
	for (int i = 0; i < Panel1->ControlCount; ++i)
	{
		TAdvSmoothToggleButton *celda = dynamic_cast<TAdvSmoothToggleButton*>(Panel1->Controls[i]);
		if(celda) {
			celda->Down = false;
			celda->BevelWidth = 0;
		}
	}
}

void __fastcall TColorAjustFormAux::BotonCerrarClick(TObject *Sender)            {

}












//-----------------------------------------------------------------------------
/*
   Lyra Techs Intellectual Property Copyright@2021.
*/
//-----------------------------------------------------------------------------
