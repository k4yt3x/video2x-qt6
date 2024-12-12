[Setup]
AppName=Video2X Qt6
AppVersion=6.2.0
AppId={{07D36A37-9C89-4ECD-9866-621351EB0886}
ArchitecturesInstallIn64BitMode=x64compatible
DefaultDirName={commonpf}\Video2X Qt6
DefaultGroupName=Video2X Qt6
DisableProgramGroupPage=yes
OutputBaseFilename=video2x-qt6-windows-amd64-installer
Compression=lzma2/max
SolidCompression=yes
UninstallDisplayIcon=assets\video2x.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &Desktop shortcut"; GroupDescription: "Additional icons:"
Name: "startmenuicon"; Description: "Create a &Start Menu shortcut"; GroupDescription: "Additional icons:"

[Files]
Source: "video2x-qt6\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs
Source: "VC_redist.x64.exe"; DestDir: "{tmp}"; Flags: dontcopy

[Icons]
Name: "{userdesktop}\Video2X Qt6"; Filename: "{app}\video2x-qt6.exe"; Tasks: desktopicon
Name: "{userprograms}\Video2X Qt6\Video2X Qt6"; Filename: "{app}\video2x-qt6.exe"; Tasks: startmenuicon

[Run]
Filename: "{tmp}\VC_redist.x64.exe"; Parameters: "/passive /norestart"; Check: NeedsVCRuntime; Flags: waituntilterminated
Filename: "{app}\video2x-qt6.exe"; Description: "Launch Video2X Qt6"; Flags: nowait postinstall skipifsilent

[Code]
function NeedsVCRuntime(): Boolean;
begin
  Result := not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64');
  if not Result then
    Result := not FileExists(ExpandConstant('{win}\System32\vcruntime140_threads.dll'));
  if Result then
    ExtractTemporaryFile('VC_redist.x64.exe');
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    if not FileExists(ExpandConstant('{win}\System32\vulkan-1.dll')) then
    begin
      MsgBox('Warning: Vulkan runtime (vulkan-1.dll) was not found on this system, ' +
             'which is required for Video2X to run.' + #13#10 + #13#10 +
             'If your system supports Vulkan, please try updating your GPU driver to the latest version. ',
             mbInformation, MB_OK);
    end;
  end;
end;