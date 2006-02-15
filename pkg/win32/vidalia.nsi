;-------------------------------------------
; vidalia.nsi
;
;  Copyright (c) 2006, Matt Edman, Justin Hipple           
;  See LICENSE for licensing information 
;-------------------------------------------
!include "MUI.nsh"

;---------------------------------
; Global Definitions
!define APPVERSION "0.0.4"
!define PRODVERSION "0.0.4.0" ; Unfortunately, product version must be x.x.x.x
!define APPNAME "TorCP"
!define APPEXEC "torcp.exe"
!define APPDESCRIPTION "Tor Control Panel ${APPVERSION}"
!define AUTHOR "Matt Edman"
!define INSTALLFILE "..\releases\${APPVERSION}\torcp_${APPVERSION}_install.exe"
!packhdr header.dat "upx --best header.dat"


;--------------------------------
; Basic Installer Information
Name "${APPNAME}"
Caption "${APPDESCRIPTION} Setup"
BrandingText "${APPDESCRIPTION}"
OutFile "${INSTALLFILE}"
CRCCheck on

VIAddVersionKey "ProductName" "${APPNAME}"
VIAddVersionKey "Comments" "${APPDESCRIPTION}"
VIAddVersionKey "CompanyName" "${AUTHOR}"
VIAddVersionKey "LegalTrademarks" "${AUTHOR}"
VIAddVersionKey "LegalCopyright" "© ${AUTHOR}"
VIAddVersionKey "FileVersion" "${APPVERSION}"
VIAddVersionKey "FileDescription" "${APPDESCRIPTION}"
VIProductVersion "${PRODVERSION}"

OutFile "${INSTALLFILE}"

InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKCU "Software" "${APPNAME}"

SetOverWrite ifnewer
AutoCloseWindow false
ShowInstDetails show
XPStyle on


;--------------------------------
; Install Types
!ifndef NOINSTTYPES ; only if not defined
  InstType "Full"
  InstType "Base"
  ;InstType /COMPONENTSONLYONCUSTOM
!endif


;--------------------------------
; MUI Options
!define MUI_WELCOMEPAGE_TITLE "Welcome to the ${APPNAME} Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of ${APPDESCRIPTION}.\r\n\r\n$_CLICK"
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\win-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\win-uninstall.ico"
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_HEADERIMAGE
!define MUI_FINISHPAGE_RUN "notepad.exe"
!define MUI_FINISHPAGE_RUN_TEXT "View README"
!define MUI_FINISHPAGE_RUN_PARAMETERS $INSTDIR\README

;--------------------------------
; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
Page custom SelectTorDirectory ValidateTorDirectory
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"


;-----------------------------------
; Put the InstallOptions file first
ReserveFile "ioFile.ini"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
  
Function .onInit
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "ioFile.ini"
FunctionEnd

;--------------------------------
; Required Components
Section "${APPNAME} (required)" TorCP
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR    
  File "..\src\Release\${APPEXEC}"
  File "..\README"
  File "..\CHANGELOG"
  File "..\LICENSE"
  File "..\TODO"

  ; Check if these files exist before trying to overwrite them
  SetOutPath $SYSDIR
  IfFileExists $SYSDIR\mfc70.dll +2 0
    File "..\dll\mfc70.dll"
  IfFileExists $SYSDIR\msvcp70.dll +2 0
    File "..\dll\msvcp70.dll"
  IfFileExists $SYSDIR\msvcr70.dll +2 0
    File "..\dll\msvcr70.dll"  

  ; Write the installation path into the registry
  WriteRegStr HKCU SOFTWARE\${APPNAME} "Install_Dir" "$INSTDIR"

  ; Write the location of Tor's executable
  !insertmacro MUI_INSTALLOPTIONS_READ $R0 "ioFile.ini" "Field 3" "State"
  WriteRegStr HKCU SOFTWARE\${APPNAME} "TorPath" $R0
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "Tor Control Panel"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
  WriteUninstaller "uninstall.exe" 
SectionEnd


;--------------------------------
; Check to see if tor.exe exists in selected directory
Function ValidateTorDirectory
  !insertmacro MUI_INSTALLOPTIONS_READ $R0 "ioFile.ini" "Field 3" "State"
  IfFileExists $R0\tor.exe found 0
    MessageBox MB_ICONEXCLAMATION|MB_YESNO "Tor was not found in the specified directory. Continue anyway?" IDYES found IDNO notfound
notfound:    
      Abort
found:    
FunctionEnd


;--------------------------------
; Start Menu Shortcuts
; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts" Shortcuts
  SectionIn 1 2
  CreateDirectory "$SMPROGRAMS\${APPNAME}"
  CreateShortCut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\${APPEXEC}" "" "$INSTDIR\${APPEXEC}" 0  
  CreateShortCut "$SMPROGRAMS\${APPNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd


;--------------------------------
; Run At Startup
Section "Run At Startup" RunAtStartup
  SectionIn 1 2
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}" "$INSTDIR\${APPEXEC}"
SectionEnd


;--------------------------------
; Select location where Tor is
Function SelectTorDirectory
  !insertmacro MUI_HEADER_TEXT "Select the location of Tor's executable" "Choose the folder in which Tor is currently installed"
  !insertmacro MUI_INSTALLOPTIONS_WRITE "ioFile.ini" "Field 3" "State" "$PROGRAMFILES\Tor"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "ioFile.ini"
FunctionEnd



;--------------------------------
; Uninstaller
Section "Uninstall"  
  ; Remove registry keys
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Run\${APPNAME}"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
  DeleteRegKey HKCU "Software\${APPNAME}"

  ; Remove files and uninstaller
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${APPNAME}\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\${APPNAME}"
  RMDir /r "$INSTDIR"
SectionEnd


!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${TorCP} "${APPNAME} Executable"
  !insertmacro MUI_DESCRIPTION_TEXT ${Shortcuts} "Add Shortcuts to Start Menu"
  !insertmacro MUI_DESCRIPTION_TEXT ${RunAtStartup} "Automatically start ${APPNAME} at startup"
!insertmacro MUI_FUNCTION_DESCRIPTION_END
