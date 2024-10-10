[Setup]
AppName=Video2X Qt6
AppVersion=6.0.0-beta.2
AppId={{07D36A37-9C89-4ECD-9866-621351EB0886}
ArchitecturesInstallIn64BitMode=x64compatible
DefaultDirName={commonpf}\Video2X Qt6
DefaultGroupName=Video2X Qt6
DisableProgramGroupPage=yes
OutputBaseFilename=video2x-qt6-windows-installer
Compression=lzma
SolidCompression=yes
UninstallDisplayIcon=assets\video2x.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &Desktop shortcut"; GroupDescription: "Additional icons:"
Name: "startmenuicon"; Description: "Create a &Start Menu shortcut"; GroupDescription: "Additional icons:"

[Files]
Source: "video2x-qt6\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs

[Icons]
Name: "{userdesktop}\Video2X Qt6"; Filename: "{app}\video2x-qt6.exe"; Tasks: desktopicon
Name: "{userprograms}\Video2X Qt6\Video2X Qt6"; Filename: "{app}\video2x-qt6.exe"; Tasks: startmenuicon

[Run]
Filename: "{app}\video2x-qt6.exe"; Description: "Launch Video2X Qt6"; Flags: nowait postinstall skipifsilent
