// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'AdvSmoothListBoxDragDrop.pas' rev: 22.00

#ifndef AdvsmoothlistboxdragdropHPP
#define AdvsmoothlistboxdragdropHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <ActiveX.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <ShlObj.hpp>	// Pascal unit
#include <Types.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Advsmoothlistboxdragdrop
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TEnumFormats;
class PASCALIMPLEMENTATION TEnumFormats : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	_di_IDataObject FDataObject;
	_di_IEnumFORMATETC FEnumerator;
	tagFORMATETC FFormatEtc;
	bool FValid;
	int FCount;
	Classes::TStringList* FFiles;
	void __fastcall SetDataObject(_di_IDataObject Value);
	int __fastcall SomeInt(System::Word Format);
	System::UnicodeString __fastcall SomeText(System::Word Format);
	bool __fastcall SomeFiles(Classes::TStringList* &Files);
	int __fastcall GetAspect(void);
	void __fastcall SetAspect(int value);
	System::Word __fastcall GetcfFormat(void);
	void __fastcall SetcfFormat(System::Word value);
	int __fastcall GetlIndex(void);
	void __fastcall SetlIndex(int value);
	int __fastcall GetTymed(void);
	void __fastcall SetTymed(int value);
	
public:
	__fastcall TEnumFormats(_di_IDataObject DataObject);
	__fastcall virtual ~TEnumFormats(void);
	bool __fastcall Reset(void);
	bool __fastcall Next(void);
	bool __fastcall HasFormat(System::Word ClipFormat);
	unsigned __fastcall Handle(int Tymed);
	unsigned __fastcall GlobalHandle(void);
	bool __fastcall HasText(void);
	bool __fastcall HasFile(void);
	bool __fastcall HasRTF(void);
	bool __fastcall HasCol(void);
	bool __fastcall HasURL(void);
	System::UnicodeString __fastcall Text(void);
	System::UnicodeString __fastcall RTF(void);
	int __fastcall Col(void);
	__property int Count = {read=FCount, nodefault};
	__property _di_IDataObject DataObject = {read=FDataObject, write=SetDataObject};
	__property bool Valid = {read=FValid, nodefault};
	__property tagFORMATETC FormatEtc = {read=FFormatEtc};
	__property int Aspect = {read=GetAspect, write=SetAspect, nodefault};
	__property System::Word Format = {read=GetcfFormat, write=SetcfFormat, nodefault};
	__property int Index = {read=GetlIndex, write=SetlIndex, nodefault};
	__property int Medium = {read=GetTymed, write=SetTymed, nodefault};
};


#pragma option push -b-
enum TDropFormat { dfText, dfFile, dfCol, dfRTF, dfURL };
#pragma option pop

typedef System::Set<TDropFormat, TDropFormat::dfText, TDropFormat::dfURL>  TDropFormats;

class DELPHICLASS TListBoxDropTarget;
class PASCALIMPLEMENTATION TListBoxDropTarget : public System::TInterfacedObject
{
	typedef System::TInterfacedObject inherited;
	
public:
	HRESULT __stdcall DragEnter(const _di_IDataObject DataObj, int grfKeyState, const Types::TPoint pt, int &dwEffect);
	HRESULT __stdcall DragOver(int grfKeyState, const Types::TPoint pt, int &dwEffect);
	HRESULT __stdcall DragLeave(void);
	HRESULT __stdcall Drop(const _di_IDataObject DataObj, int grfKeyState, const Types::TPoint pt, int &dwEffect);
	
private:
	_di_IDataObject FData;
	bool FOk;
	bool FAcceptText;
	bool FAcceptFiles;
	bool FAcceptCols;
	bool FAcceptURLs;
	TDropFormats FDropFormats;
	
public:
	__fastcall TListBoxDropTarget(void);
	virtual void __fastcall DropText(const Types::TPoint &pt, System::UnicodeString s);
	virtual void __fastcall DropCol(const Types::TPoint &pt, int col);
	virtual void __fastcall DropRTF(const Types::TPoint &pt, System::UnicodeString s);
	virtual void __fastcall DropFiles(const Types::TPoint &pt, Classes::TStrings* files);
	virtual void __fastcall DropURL(const Types::TPoint &pt, System::UnicodeString s);
	virtual void __fastcall DragMouseMove(System::TObject* Source, const Types::TPoint &pt, bool &Allow, TDropFormats DropFormats);
	virtual void __fastcall DragMouseEnter(void);
	virtual void __fastcall DragMouseLeave(void);
	__property bool AcceptText = {read=FAcceptText, write=FAcceptText, nodefault};
	__property bool AcceptFiles = {read=FAcceptFiles, write=FAcceptFiles, nodefault};
	__property bool AcceptCols = {read=FAcceptCols, write=FAcceptCols, nodefault};
	__property bool AcceptURLs = {read=FAcceptURLs, write=FAcceptURLs, nodefault};
	__property TDropFormats DropFormats = {read=FDropFormats, nodefault};
public:
	/* TObject.Destroy */ inline __fastcall virtual ~TListBoxDropTarget(void) { }
	
private:
	void *__IDropTarget;	/* IDropTarget */
	
public:
	#if defined(MANAGED_INTERFACE_OPERATORS)
	operator _di_IDropTarget()
	{
		_di_IDropTarget intf;
		GetInterface(intf);
		return intf;
	}
	#else
	operator IDropTarget*(void) { return (IDropTarget*)&__IDropTarget; }
	#endif
	
};


class DELPHICLASS TListBoxDropSource;
class PASCALIMPLEMENTATION TListBoxDropSource : public System::TInterfacedObject
{
	typedef System::TInterfacedObject inherited;
	
private:
	bool FNoAccept;
	
protected:
	virtual void __fastcall DragDropStop(void);
	
public:
	__fastcall TListBoxDropSource(void);
	HRESULT __stdcall QueryContinueDrag(BOOL fEscapePressed, int grfKeyState);
	HRESULT __stdcall GiveFeedback(int dwEffect);
	virtual void __fastcall CurrentEffect(int dwEffect);
	virtual void __fastcall QueryDrag(void);
public:
	/* TObject.Destroy */ inline __fastcall virtual ~TListBoxDropSource(void) { }
	
private:
	void *__IDropSource;	/* IDropSource */
	
public:
	#if defined(MANAGED_INTERFACE_OPERATORS)
	operator _di_IDropSource()
	{
		_di_IDropSource intf;
		GetInterface(intf);
		return intf;
	}
	#else
	operator IDropSource*(void) { return (IDropSource*)&__IDropSource; }
	#endif
	
};


class DELPHICLASS TSourceDataObject;
class PASCALIMPLEMENTATION TSourceDataObject : public System::TInterfacedObject
{
	typedef System::TInterfacedObject inherited;
	
private:
	System::UnicodeString textdata;
	System::UnicodeString rtfdata;
	int scolidx;
	
public:
	__fastcall TSourceDataObject(const System::UnicodeString stxt, const System::UnicodeString srtf, int sidx);
	HRESULT __stdcall GetData(const tagFORMATETC &formatetc, /* out */ tagSTGMEDIUM &medium);
	HRESULT __stdcall GetDataHere(const tagFORMATETC &formatetc, /* out */ tagSTGMEDIUM &medium);
	HRESULT __stdcall QueryGetData(const tagFORMATETC &formatetc);
	HRESULT __stdcall SetData(const tagFORMATETC &formatetc, tagSTGMEDIUM &medium, BOOL fRelease);
	HRESULT __stdcall GetCanonicalFormatEtc(const tagFORMATETC &formatetcIn, /* out */ tagFORMATETC &formatetcOut);
	HRESULT __stdcall EnumFormatEtc(int dwDirection, /* out */ _di_IEnumFORMATETC &enumFormatEtc);
	HRESULT __stdcall DAdvise(const tagFORMATETC &formatetc, int advf, const _di_IAdviseSink advSink, /* out */ int &dwConnection);
	HRESULT __stdcall DUnadvise(int dwConnection);
	HRESULT __stdcall EnumDAdvise(/* out */ _di_IEnumSTATDATA &enumAdvise);
public:
	/* TObject.Destroy */ inline __fastcall virtual ~TSourceDataObject(void) { }
	
private:
	void *__IDataObject;	/* IDataObject */
	
public:
	#if defined(MANAGED_INTERFACE_OPERATORS)
	operator _di_IDataObject()
	{
		_di_IDataObject intf;
		GetInterface(intf);
		return intf;
	}
	#else
	operator IDataObject*(void) { return (IDataObject*)&__IDataObject; }
	#endif
	
};


class DELPHICLASS TSourceEnumFormatEtc;
class PASCALIMPLEMENTATION TSourceEnumFormatEtc : public System::TInterfacedObject
{
	typedef System::TInterfacedObject inherited;
	
protected:
	int FIndex;
	bool FHasCol;
	
public:
	HRESULT __stdcall Next(int CountRequested, /* out */ void *FormatEtcArray, System::PLongInt PCountFetched);
	HRESULT __stdcall Skip(int count);
	HRESULT __stdcall Reset(void);
	HRESULT __stdcall Clone(/* out */ _di_IEnumFORMATETC &enumFmt);
public:
	/* TObject.Create */ inline __fastcall TSourceEnumFormatEtc(void) : System::TInterfacedObject() { }
	/* TObject.Destroy */ inline __fastcall virtual ~TSourceEnumFormatEtc(void) { }
	
private:
	void *__IEnumFORMATETC;	/* IEnumFORMATETC */
	
public:
	#if defined(MANAGED_INTERFACE_OPERATORS)
	operator _di_IEnumFORMATETC()
	{
		_di_IEnumFORMATETC intf;
		GetInterface(intf);
		return intf;
	}
	#else
	operator IEnumFORMATETC*(void) { return (IEnumFORMATETC*)&__IEnumFORMATETC; }
	#endif
	
};


typedef System::StaticArray<tagFORMATETC, 20> TFormatEtcArray;

typedef TFormatEtcArray *PFormatEtcArray;

//-- var, const, procedure ---------------------------------------------------
static const System::ShortInt deNone = 0x0;
static const System::ShortInt deMove = 0x2;
static const System::ShortInt deCopy = 0x1;
static const System::ShortInt deLink = 0x4;
static const unsigned deScroll = 0x80000000;
static const System::ShortInt ddGet = 0x1;
static const System::ShortInt ddSet = 0x2;
static const System::ShortInt tsGlobal = 0x1;
static const System::ShortInt tsFile = 0x2;
static const System::ShortInt tsStream = 0x4;
static const System::ShortInt tsStorage = 0x8;
static const System::ShortInt tsGDI = 0x10;
static const System::ShortInt tsMetafilePict = 0x20;
static const System::ShortInt tsEnhMetafile = 0x40;
static const System::ShortInt tsNull = 0x0;
extern PACKAGE void __fastcall SetRTFAware(bool value);
extern PACKAGE int __fastcall StandardEffect(Classes::TShiftState Keys, int DefEffect);
extern PACKAGE HRESULT __fastcall StartTextDoDragDrop(TListBoxDropSource* DropSource, System::UnicodeString stxt, System::UnicodeString srtf, int dwOKEffects, int &dwEffect);
extern PACKAGE HRESULT __fastcall StartColDoDragDrop(TListBoxDropSource* DropSource, int column, int dwOKEffects, int &dwEffect);

}	/* namespace Advsmoothlistboxdragdrop */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Advsmoothlistboxdragdrop;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// AdvsmoothlistboxdragdropHPP
