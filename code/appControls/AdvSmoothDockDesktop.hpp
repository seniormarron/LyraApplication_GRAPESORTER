// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'AdvSmoothDockDesktop.pas' rev: 22.00

#ifndef AdvsmoothdockdesktopHPP
#define AdvsmoothdockdesktopHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <AdvSmoothDock.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Advsmoothdockdesktop
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TAdvSmoothDockDesktop;
class PASCALIMPLEMENTATION TAdvSmoothDockDesktop : public Advsmoothdock::TAdvSmoothDock
{
	typedef Advsmoothdock::TAdvSmoothDock inherited;
	
public:
	virtual void __fastcall AlignDesktop(Advsmoothdock::TAdvSmoothDockAlignDesktop Align);
	__property DockOnDeskTop = {default=0};
	__property DockVisible = {default=0};
public:
	/* TAdvSmoothDock.Create */ inline __fastcall virtual TAdvSmoothDockDesktop(Classes::TComponent* AOwner) : Advsmoothdock::TAdvSmoothDock(AOwner) { }
	/* TAdvSmoothDock.Destroy */ inline __fastcall virtual ~TAdvSmoothDockDesktop(void) { }
	
public:
	/* TWinControl.CreateParented */ inline __fastcall TAdvSmoothDockDesktop(HWND ParentWindow) : Advsmoothdock::TAdvSmoothDock(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Advsmoothdockdesktop */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Advsmoothdockdesktop;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// AdvsmoothdockdesktopHPP
