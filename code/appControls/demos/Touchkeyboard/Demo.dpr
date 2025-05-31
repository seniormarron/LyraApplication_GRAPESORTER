program Demo;

uses
  Forms,
  UDemo in 'UDemo.pas' {Form171};

{$R *.res}

begin
  Application.Initialize;
  
  Application.CreateForm(TForm171, Form171);
  Application.Run;
end.
