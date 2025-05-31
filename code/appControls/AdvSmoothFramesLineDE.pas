{**************************************************************************}
{ TAdvSmoothFramesLine DESIGN TIME EDITOR                                    }
{ for Delphi & C++Builder                                                  }
{                                                                          }
{ written by TMS Software                                                  }
{            copyright � 2009                                              }
{            Email : info@tmssoftware.com                                  }
{            Web : http://www.tmssoftware.com                              }
{                                                                          }
{ The source code is given as is. The author is not responsible            }
{ for any possible damage done due to the use of this code.                }
{ The component can be freely used in any application. The complete        }
{ source code remains property of the author and may not be distributed,   }
{ published, given or sold in any form as such. No parts of the source     }
{ code can be included in any other component or application without       }
{ written authorization of the author.                                     }
{**************************************************************************}

unit AdvSmoothFramesLineDE;

interface

{$I TMSDEFS.INC}

uses
  Classes, AdvSmoothFramesLine,
  {$IFDEF DELPHI6_LVL}
  DesignIntf, DesignEditors
  {$ELSE}
  DsgnIntf
  {$ENDIF}
  ;

type
  TAdvSmoothFramesLineEditor = class(TDefaultEditor)
  public
    function GetVerb(index:integer):string; override;
    function GetVerbCount:integer; override;
    procedure ExecuteVerb(Index:integer); override;
  end;


implementation

uses
  Forms, Windows, Controls , AdvSmoothStyles, Dialogs, AdvStyleIF;

procedure TAdvSmoothFramesLineEditor.ExecuteVerb(Index: integer);
var
  psf: TAdvSmoothStyleForm;
  style: TTMSStyle;
begin
  inherited;
  if (Index = 0) then
  begin
    psf := TAdvSmoothStyleForm.Create(Application);
    if psf.ShowModal = mrOK then
    begin
      //ShowMessage(inttostr(psf.RadioGroup1.ItemIndex));
      style := tsOffice2003Blue;
      case psf.RadioGroup1.ItemIndex of
      1: style := tsOffice2003Olive;
      2: style := tsOffice2003Silver;
      3: style := tsOffice2003Classic;
      4: style := tsOffice2007Luna;
      5: style := tsOffice2007Obsidian;
      6: style := tsOffice2007Silver;
      7: style := tsWindowsXP;
      8: style := tsWindowsVista;
      9: style := tsWindows7;
      10: style := tsTerminal;
      end;
      if (Component is TAdvSmoothFramesLine) then
         (Component as TAdvSmoothFramesLine).SetComponentStyle(style);
         Designer.Modified;
    end;
    psf.Free;
  end;
end;

function TAdvSmoothFramesLineEditor.GetVerb(index: integer): string;
begin
  if index = 0 then
    Result := 'Styles';
end;

function TAdvSmoothFramesLineEditor.GetVerbCount: integer;
begin
  Result := 1;
end;



end.







