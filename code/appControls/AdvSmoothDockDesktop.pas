unit AdvSmoothDockDesktop;

interface

uses
  AdvSmoothDock, Classes;

type
  TAdvSmoothDockDesktop = class(TAdvSmoothDock)
  public
    procedure AlignDesktop(Align: TAdvSmoothDockAlignDesktop); override;
    property DockOnDeskTop;
    property DockVisible;
  end;

procedure Register;

implementation

procedure Register;
begin
  RegisterComponents('TMS SmoothControls', [TAdvSmoothDockDesktop]);
end;

{ TAdvSmoothDockDesktop }

procedure TAdvSmoothDockDesktop.AlignDesktop(Align: TAdvSmoothDockAlignDesktop);
begin
  inherited;
end;


end.
