program Demo2;

uses
  Forms,
  UDemo2 in 'UDemo2.pas' {Form446};

{$R *.res}

begin
  Application.Initialize;
  
  Application.CreateForm(TForm446, Form446);
  Application.Run;
end.
