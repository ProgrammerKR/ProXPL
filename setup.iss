; Script generated for ProXPL Project
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "ProXPL"
#define MyAppVersion "0.2.0"
#define MyAppPublisher "ProXPL Team"
#define MyAppURL "https://github.com/ProgrammerKR/ProXPL"
#define MyAppExeName "proxpl.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{D13F2113-1B21-4608-8756-748924080123}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
PrivilegesRequired=admin
OutputDir=output
OutputBaseFilename=ProXPL_Installer_v{#MyAppVersion}
SetupIconFile=assets\icon.ico
SolidCompression=yes
WizardStyle=modern
LicenseFile=doc\LICENSE.txt

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "addtopath"; Description: "Add ProXPL to the system PATH environment variable"; GroupDescription: "System Integration"; Flags: unchecked

[Files]
; Binaries
Source: "bin\{#MyAppExeName}"; DestDir: "{app}\bin"; Flags: ignoreversion
; Libraries
Source: "lib\*"; DestDir: "{app}\lib"; Flags: ignoreversion recursesubdirs createallsubdirs
; Documentation
Source: "doc\*"; DestDir: "{app}\doc"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; Tasks: desktopicon

[Registry]
Root: HKA; Subkey: "Software\Classes\.prox"; ValueType: string; ValueName: ""; ValueData: "ProXPLSourceFile"; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\.pxpl"; ValueType: string; ValueName: ""; ValueData: "ProXPLSourceFile"; Flags: uninsdeletevalue
Root: HKA; Subkey: "Software\Classes\ProXPLSourceFile"; ValueType: string; ValueName: ""; ValueData: "ProXPL Source File"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\Classes\ProXPLSourceFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\{#MyAppExeName},0"
Root: HKA; Subkey: "Software\Classes\ProXPLSourceFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\{#MyAppExeName}"" ""%1"""

[Code]
const
  EnvironmentKey = 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment';

procedure EnvAddPath(Path: string);
var
  Paths: string;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'Path', Paths) then
    Paths := '';

  if Pos(';' + Path + ';', ';' + Paths + ';') = 0 then
  begin
    Paths := Paths + ';' + Path;
    RegWriteStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'Path', Paths);
  end;
end;

procedure EnvRemovePath(Path: string);
var
  Paths: string;
  P: Integer;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'Path', Paths) then exit;

  P := Pos(';' + Path + ';', ';' + Paths + ';');
  if P = 0 then
  begin
    if Pos(Path + ';', Paths) = 1 then P := 1
    else if Pos(';' + Path, Paths) = Length(Paths) - Length(Path) then P := Length(Paths) - Length(Path);
  end;

  if P > 0 then
  begin
    Delete(Paths, P - 1, Length(Path) + 1);
    RegWriteStringValue(HKEY_LOCAL_MACHINE, EnvironmentKey, 'Path', Paths);
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep = ssPostInstall) and IsTaskSelected('addtopath') then
  begin
    EnvAddPath(ExpandConstant('{app}\bin'));
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then
  begin
    EnvRemovePath(ExpandConstant('{app}\bin'));
  end;
end;
