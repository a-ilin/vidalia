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
!define PRODVERSION "${APPVERSION}.0" ; Product version must be x.x.x.x
!define APPNAME "Vidalia"
!define APPEXEC "vidalia.exe"
!define APPDESCRIPTION "${APPNAME} ${APPVERSION}"
!define AUTHOR "Matt Edman, Justin Hipple"
!define INSTALLFILE "vidalia-${APPVERSION}.exe"


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
SetCompressor /SOLID lzma
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
!define MUI_FINISHPAGE_RUN "$INSTDIR\${APPEXEC}"
!define MUI_FINISHPAGE_RUN_TEXT "Run Vidalia now"


;--------------------------------
; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Required Components
Section "${APPNAME} (required)" Vidalia
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR    
  File "..\..\bin\${APPEXEC}"
  File "..\..\bin\mingwm10.dll"
  File "..\..\AUTHORS"
  File "..\..\CHANGELOG"
  File "..\..\LICENSE"
  File "..\..\COPYING"
  File "..\..\README"

  ; Write the installation path into the registry
  WriteRegStr HKCU SOFTWARE\${APPNAME} "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME} ${APPVERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
  WriteUninstaller "uninstall.exe" 
SectionEnd

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
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${APPNAME}" '"$INSTDIR\${APPEXEC}"'
SectionEnd


;--------------------------------
; Uninstaller
Section "Uninstall"  
  ; Remove registry keys
  DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Run\" "${APPNAME}"
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
  !insertmacro MUI_DESCRIPTION_TEXT ${Vidalia} "${APPNAME} Executable"
  !insertmacro MUI_DESCRIPTION_TEXT ${Shortcuts} "Add Shortcuts to Start Menu"
  !insertmacro MUI_DESCRIPTION_TEXT ${RunAtStartup} "Automatically start ${APPNAME} at startup"
!insertmacro MUI_FUNCTION_DESCRIPTION_END
