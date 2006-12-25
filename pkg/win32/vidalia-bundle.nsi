;---------------------------------
; $Id: $
; 
; Vidalia/Tor/Privoxy Bundle Installer
; See BUNDLE_LICENSE for licensing information
;---------------------------------
!include "MUI.nsh"

SetCompressor /SOLID lzma
!packhdr header.dat "upx --best header.dat"

;---------------------------------
; Global definitions
!define SF_SELECTED   1
!define SECTION_OFF   0xFFFFFFFE

!define VIDALIA_NAME        "Vidalia"
!define VIDALIA_EXEC        "vidalia.exe"
!define VIDALIA_APPVERSION  "0.0.10"
!define VIDALIA_DESC        "${VIDALIA_NAME} ${VIDALIA_APPVERSION}"
!define VIDALIA_UNINST      "uninstall.exe"

!define TOR_NAME            "Tor"
!define TOR_EXEC            "tor.exe"
!define TOR_APPVERSION      "0.1.2.4-alpha"
!define TOR_DESC            "${TOR_NAME} ${TOR_APPVERSION}"
!define TOR_UNINST          "Uninstall.exe"

!define PRIVOXY_NAME        "Privoxy"
!define PRIVOXY_EXEC        "privoxy.exe"
!define PRIVOXY_APPVERSION  "3.0.6"
!define PRIVOXY_DESC        "${PRIVOXY_NAME} ${PRIVOXY_APPVERSION}"
!define PRIVOXY_UNINST      "privoxy_uninstall.exe"

!define TORBUTTON_NAME      "Torbutton"
!define TORBUTTON_APPVERSION "1.0.4-fx+tb"
!define TORBUTTON_DESC      "${TORBUTTON_NAME} ${TORBUTTON_APPVERSION}"

!define OPENSSL_NAME        "OpenSSL"
!define OPENSSL_APPVERSION  "0.9.8d"
!define OPENSSL_DESC        "${OPENSSL_NAME} ${OPENSSL_APPVERSION}"

!define BUNDLE_NAME         "Vidalia Bundle"
!define BUNDLE_APPVERSION   "${TOR_APPVERSION}-${VIDALIA_APPVERSION}"
!define BUNDLE_REVISION     "1"
!define BUNDLE_PRODVERSION  "${VIDALIA_APPVERSION}.${BUNDLE_REVISION}" ; Product version must be x.x.x.x
!define BUNDLE_DESC         "${BUNDLE_NAME} ${BUNDLE_APPVERSION}"
!define INSTALLFILE         "vidalia-bundle-${BUNDLE_APPVERSION}.exe"

;--------------------------------
; Installer file details
VIAddVersionKey "ProductName"     "${BUNDLE_NAME}"
VIAddVersionKey "Comments"        "${BUNDLE_DESC}"
VIAddVersionKey "FileVersion"     "${BUNDLE_APPVERSION}"
VIAddVersionKey "FileDescription" "${BUNDLE_DESC}"
VIProductVersion "${BUNDLE_PRODVERSION}"

;--------------------------------
; Basic installer information
Name            "${BUNDLE_NAME}"
Caption         "$(BundleSetupCaption)"
BrandingText    "${BUNDLE_DESC} (Rev. ${BUNDLE_REVISION})"
OutFile         "${INSTALLFILE}"
InstallDir      "$PROGRAMFILES"
SetOverWrite    ifnewer
AutoCloseWindow false
ShowInstDetails show
CRCCheck        on
XPStyle         on

;--------------------------------
; MUI Options
!define MUI_WELCOMEPAGE_TITLE "$(BundleWelcomeTitle)"
!define MUI_WELCOMEPAGE_TEXT  "$(BundleWelcomeText)"
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\win-install.ico"
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp"
!define MUI_HEADERIMAGE
!define MUI_FINISHPAGE_TEXT "$(BundleFinishText)"
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION CustomFinishFn
!define MUI_FINISHPAGE_RUN_TEXT "$(BundleRunNow)"
!define MUI_FINISHPAGE_LINK "$(BundleLinkText)"
!define MUI_FINISHPAGE_LINK_LOCATION  "http://tor.eff.org/docs/tor-doc-win32.html"

;--------------------------------
; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;--------------------------------
; Available languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Farsi"
!insertmacro MUI_LANGUAGE "Finnish"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "PortugueseBR"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "SimpChinese"
!include "vidalia_en.nsh"
!include "vidalia_de.nsh"
!include "vidalia_es.nsh"
!include "vidalia_fa.nsh"
!include "vidalia_fi.nsh"
!include "vidalia_fr.nsh"
!include "vidalia_pl.nsh"
!include "vidalia_pt-br.nsh"
!include "vidalia_ru.nsh"
!include "vidalia_zh-cn.nsh"

;--------------------------------
; Install types
!ifndef NOINSTTYPES ; only if not defined
  InstType "Full"
  InstType "Base"
  ;InstType /COMPONENTSONLYONCUSTOM
!endif



;--------------------------------
; Tor
Var configdir
Var configfile
var bInstallTor
SectionGroup "!${TOR_DESC}" TorGroup
    ;--------------------------------
    ; Tor application binaries
    Section "${TOR_NAME}" Tor
    ;Files that have to be installed for tor to run and that the user
    ;cannot choose not to install
       SectionIn 1 2
       SetOutPath "$INSTDIR\Tor"
       File "tor\${TOR_APPVERSION}\tor.exe"
       File "tor\${TOR_APPVERSION}\tor-resolve.exe"
       File "tor\${TOR_APPVERSION}\${TOR_UNINST}"
       WriteIniStr "$INSTDIR\Tor\Tor Website.url" "InternetShortcut" "URL" "http://tor.eff.org"

       StrCpy $configfile "torrc"
       StrCpy $configdir $APPDATA\Tor
       SetOutPath $configdir

       ;If there's already a torrc config file, ask if they want to
       ;overwrite it with the new one.
       IfFileExists "$configdir\torrc" "" endiftorrc
          MessageBox MB_ICONQUESTION|MB_YESNO "$(TorAskOverwriteTorrc)" IDNO yesreplace
          Delete $configdir\torrc
          Goto endiftorrc
         yesreplace:
          StrCpy $configfile "torrc.sample"
       endiftorrc:
       File /oname=$configfile "tor\${TOR_APPVERSION}\torrc.sample"

       ; Write the uninstall keys for Windows
       WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tor" "DisplayName" "${TOR_DESC}"
       WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tor" "UninstallString" '"$INSTDIR\Tor\${TOR_UNINST}"'
       WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tor" "NoModify" 1
       WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tor" "NoRepair" 1
       
       IntOp $bInstallTor 0 + 1
    SectionEnd

    ;--------------------------------
    ; OpenSSL binaries
    Section "$(TorOpenSSL)" TorOpenSSL
       SectionIn 1 2
       SetOutPath "$INSTDIR\Tor"
       File "tor\${TOR_APPVERSION}\libcrypto.a"
       File "tor\${TOR_APPVERSION}\libssl.a"
    SectionEnd

    ;--------------------------------
    ; Tor documentation
    Section "$(TorDocumentation)" TorDocs
       SectionIn 1
       SetOutPath "$INSTDIR\Tor\Documents"
       File "tor\${TOR_APPVERSION}\Documents\*.*"
    SectionEnd

    ;--------------------------------
    ; Tor Start menu shortcuts
    Section "$(TorShortcuts)" TorShortcuts
      SectionIn 1
        SetShellVarContext all ; (Add to "All Users" Start Menu if possible)
        SetOutPath "$INSTDIR\Tor"
        IfFileExists "$SMPROGRAMS\Tor\*.*" "" +2
           RMDir /r "$SMPROGRAMS\Tor"
        
        CreateDirectory "$SMPROGRAMS\Tor"
        CreateShortCut "$SMPROGRAMS\Tor\Tor.lnk" "$INSTDIR\Tor\tor.exe"
        CreateShortCut "$SMPROGRAMS\Tor\Torrc.lnk" "Notepad.exe" "$configdir\torrc"
        CreateShortCut "$SMPROGRAMS\Tor\Tor Website.lnk" "$INSTDIR\Tor\Tor Website.url"
        CreateShortCut "$SMPROGRAMS\Tor\Uninstall.lnk" "$INSTDIR\Tor\${TOR_UNINST}"
        
        IfFileExists "$INSTDIR\Tor\Documents\*.*" "" endifdocs
          CreateDirectory "$SMPROGRAMS\Tor\Documents"
          CreateShortCut "$SMPROGRAMS\Tor\Documents\Tor Manual.lnk" "$INSTDIR\Tor\Documents\tor-reference.html"
          CreateShortCut "$SMPROGRAMS\Tor\Documents\Tor Documentation.lnk" "$INSTDIR\Tor\Documents"
          CreateShortCut "$SMPROGRAMS\Tor\Documents\Tor Specification.lnk" "$INSTDIR\Tor\Documents\tor-spec.txt"
        endifdocs:
    SectionEnd
SectionGroupEnd


;--------------------------------
; Vidalia
var bInstallVidalia
SectionGroup "${VIDALIA_DESC}" VidaliaGroup
    ;--------------------------------
    ; Vidalia application binaries
    Section "${VIDALIA_NAME}" Vidalia
      SectionIn 1 2

      ; Set output path to the installation directory.
      SetOutPath "$INSTDIR\Vidalia"
      File "Vidalia\${VIDALIA_APPVERSION}\${VIDALIA_EXEC}"
      File "Vidalia\${VIDALIA_APPVERSION}\${VIDALIA_UNINST}"
      File "Vidalia\${VIDALIA_APPVERSION}\mingwm10.dll"
      File "Vidalia\${VIDALIA_APPVERSION}\README"
      File "Vidalia\${VIDALIA_APPVERSION}\CHANGELOG"
      File "Vidalia\${VIDALIA_APPVERSION}\LICENSE"
      File "Vidalia\${VIDALIA_APPVERSION}\COPYING"
      File "Vidalia\${VIDALIA_APPVERSION}\AUTHORS"
      File "BUNDLE_LICENSE"
      
      ; Include a prebuilt GeoIP cache
      SetOutPath "$APPDATA\Vidalia"
      File "Vidalia\${VIDALIA_APPVERSION}\geoip-cache"

      ; Tor gets installed to $INSTDIR\Tor, so let's remember it
      ; But first, we have to replace all the '\'s with '\\'s in the $INSTDIR
      ; Does NSIS make this easy by providing the ability to replace substrings? No.
      Push $INSTDIR
      Push "\"
      Push "\\"
      Call StrRep
      Pop $R0 ; contains the modified version of $INSTDIR

      SetShellVarContext current
      IfFileExists "$APPDATA\Vidalia" EndIfConf
        CreateDirectory "$APPDATA\Vidalia"
      EndIfConf:
      WriteINIStr "$APPDATA\Vidalia\vidalia.conf" Tor TorExecutable "$R0\\Tor\\${TOR_EXEC}"
      SetShellVarContext all

      ; Write the uninstall keys for Windows  
      WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vidalia" "DisplayName" "${VIDALIA_DESC}"
      WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vidalia" "UninstallString" '"$INSTDIR\Vidalia\${VIDALIA_UNINST}"'
      WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vidalia" "NoModify" 1
      WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vidalia" "NoRepair" 1
      
      IntOp $bInstallVidalia 0 + 1
    SectionEnd

    ;--------------------------------
    ; Vidalia Start menu shortcuts
    Section "$(VidaliaShortcuts)" VidaliaShortcuts
      SectionIn 1
      SetShellVarContext all ; (Add to "All Users" Start Menu if possible)
      CreateDirectory "$SMPROGRAMS\Vidalia"
      CreateShortCut "$SMPROGRAMS\Vidalia\Uninstall.lnk" "$INSTDIR\Vidalia\uninstall.exe" "" "$INSTDIR\Vidalia\${VIDALIA_UNINST}" 0
      CreateShortCut "$SMPROGRAMS\Vidalia\Vidalia.lnk" "$INSTDIR\Vidalia\${VIDALIA_EXEC}" "" "$INSTDIR\Vidalia\${VIDALIA_EXEC}" 0
    SectionEnd

    ;--------------------------------
    ; Run Vidalia at startup
    Section "$(VidaliaStartup)" VidaliaStartup
      SectionIn 1
      WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${VIDALIA_NAME}" '"$INSTDIR\Vidalia\${VIDALIA_EXEC}"'
    SectionEnd    
SectionGroupEnd


;--------------------------------
; Privoxy
var bInstallPrivoxy
SectionGroup "${PRIVOXY_DESC}" PrivoxyGroup
    ;--------------------------------
    ; Privoxy application binaries
    Section "${PRIVOXY_NAME}" Privoxy
        SectionIn 1 2
        ; add files / whatever that need to be installed here.
        SetOutPath "$INSTDIR\Privoxy"
        File /r Privoxy\${PRIVOXY_APPVERSION}\*.*

        WriteRegStr HKEY_CLASSES_ROOT "PrivoxyActionFile\shell\open\command" "" 'Notepad.exe "%1"'
        WriteRegStr HKEY_CLASSES_ROOT ".action" "" "PrivoxyActionFile"
        WriteRegStr HKEY_CLASSES_ROOT "PrivoxyFilterFile\shell\open\command" "" 'Notepad.exe "%1"'
        WriteRegStr HKEY_CLASSES_ROOT ".filter" "" "PrivoxyFilterFile"
       
        ; Write the installation path into the registry
        WriteRegStr HKCU SOFTWARE\Privoxy "Install_Dir" "$INSTDIR"

        ; Write the uninstall keys for Windows
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "DisplayName" "${PRIVOXY_DESC}"
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "UninstallString" '"$INSTDIR\Privoxy\${PRIVOXY_UNINST}"'
        WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "NoModify" 1
        WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Privoxy" "NoRepair" 1
               
       
        IntOp $bInstallPrivoxy 0 + 1
    SectionEnd

    ;--------------------------------
    ; Privoxy Start menu shortcuts
    Section "$(PrivoxyShortcuts)" PrivoxyShortcuts
        SectionIn 1
        SetShellVarContext all ; (Add to "All Users" Start Menu if possible)
        RMDir /r "$SMPROGRAMS\Privoxy"
        CreateDirectory "$SMPROGRAMS\Privoxy"
        CreateShortCut "$SMPROGRAMS\Privoxy\Privoxy.lnk" "$INSTDIR\Privoxy\privoxy.exe"
        WriteINIStr "$SMPROGRAMS\Privoxy\Web-based Configuration.url" "InternetShortcut" "URL" "http://config.privoxy.org/"
        CreateShortCut "$SMPROGRAMS\Privoxy\Web-based Feedback.lnk" "$INSTDIR\Privoxy\doc\user-manual\contact.html"
        CreateDirectory "$SMPROGRAMS\Privoxy\Edit Config"
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Main Configuration.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\config.txt"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Default Actions.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\default.action"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\User Actions.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\user.action"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Filters.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\default.filter"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Edit Config\Trust list.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\trust.txt"'
        CreateDirectory "$SMPROGRAMS\Privoxy\Documentation"
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\User Manual.lnk" "$INSTDIR\Privoxy\doc\user-manual\index.html"
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\Frequently Asked Questions.lnk" "$INSTDIR\Privoxy\doc\faq\index.html"
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\Credits.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\AUTHORS.txt"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\License.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\LICENSE.txt"'
        CreateShortCut "$SMPROGRAMS\Privoxy\Documentation\ReadMe file.lnk" "Notepad.exe" '"$INSTDIR\Privoxy\README.txt"'
        WriteINIStr "$SMPROGRAMS\Privoxy\Documentation\Web Site.url" "InternetShortcut" "URL" "http://privoxy.org/"
        CreateShortCut "$SMPROGRAMS\Privoxy\Privoxy Uninstall.lnk" "$INSTDIR\Privoxy\${PRIVOXY_UNINST}"
    SectionEnd
 
    ;--------------------------------
    ; Run Privoxy at startup
    Section "$(PrivoxyStartup)" PrivoxyStartup
      SectionIn 1
      CreateShortCut "$SMSTARTUP\Privoxy.lnk" "$INSTDIR\Privoxy\privoxy.exe" "" "" 0 SW_SHOWMINIMIZED
    SectionEnd     
SectionGroupEnd

;--------------------------------
; Torbutton
SectionGroup "${TORBUTTON_DESC}" TorbuttonGroup
  Section "${TORBUTTON_NAME}" Torbutton
    SectionIn 1 2
    SetOutPath "$INSTDIR\Torbutton"
    File torbutton\${TORBUTTON_APPVERSION}\torbutton-${TORBUTTON_APPVERSION}.xpi
  SectionEnd

  Section "$(TorbuttonAddToFirefox)" TorbuttonAddToFirefox
    SectionIn 1 2
    ReadRegStr $1 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\firefox.exe" "Path"
    StrCmp $1 "" FirefoxNotFound 0 ; if Path is empty or null, then skip to an error, otherwise proceed
	  Exec '"$1firefox.exe" -install-global-extension "$INSTDIR\Torbutton\torbutton-${TORBUTTON_APPVERSION}.xpi"'
  	Goto TorbuttonInstalled
    FirefoxNotFound:
	  MessageBox MB_OK|MB_ICONSTOP "$(TorbuttonFirefoxNotFound)"
    TorbuttonInstalled:
  SectionEnd
SectionGroupEnd

;--------------------------------
; Functions
Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Function .onSelChange
  Push $0

  ; Check if the Torbutton option was unchecked
  SectionGetFlags ${Torbutton} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 0 0 SelChangeDone SelChangeDone

  ; Uncheck the "Add to Firefox" box
  SectionGetFlags ${TorbuttonAddToFirefox} $0
  IntOp $0 $0 & ${SECTION_OFF}
  SectionSetFlags ${TorbuttonAddToFirefox} $0
  
  SelChangeDone:
  Pop $0
FunctionEnd

Function CustomFinishFn
    IntCmp $bInstallVidalia 1 run_vidalia check_tor check_tor
    run_vidalia:
        Exec '"$INSTDIR\Vidalia\${VIDALIA_EXEC}"'
        goto check_privoxy
    
    check_tor:
    IntCmp $bInstallTor 1 run_tor check_privoxy check_privoxy
    run_tor:
        Exec '"$INSTDIR\Tor\${TOR_EXEC}"'
    
    check_privoxy:
    IntCmp $bInstallPrivoxy 1 run_privoxy done done
    run_privoxy:
        SetOutPath "$INSTDIR\Privoxy"
        Exec '"$INSTDIR\Privoxy\${PRIVOXY_EXEC}"'
    done:
FunctionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${Tor} "$(TorAppDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorGroup} "$(TorGroupDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorOpenSSL} "$(TorOpenSSLDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorDocs} "$(TorDocumentationDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorShortcuts} "$(TorShortcutsDesc)"

  !insertmacro MUI_DESCRIPTION_TEXT ${Vidalia} "$(VidaliaAppDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${VidaliaGroup} "$(VidaliaGroupDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${VidaliaStartup} "$(VidaliaStartupDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${VidaliaShortcuts} "$(VidaliaShortcutsDesc)"

  !insertmacro MUI_DESCRIPTION_TEXT ${Privoxy} "$(PrivoxyAppDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${PrivoxyGroup} "$(PrivoxyGroupDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${PrivoxyStartup} "$(PrivoxyStartupDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${PrivoxyShortcuts} "$(PrivoxyShortcutsDesc)"

  !insertmacro MUI_DESCRIPTION_TEXT ${Torbutton} "$(TorbuttonAppDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorbuttonGroup} "$(TorbuttonGroupDesc)"
  !insertmacro MUI_DESCRIPTION_TEXT ${TorbuttonAddToFirefox} "$(TorbuttonAddToFirefoxDesc)"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function StrRep
 
  ;Written by dirtydingus 2003-02-20 04:30:09 
  ; USAGE
  ;Push String to do replacement in (haystack)
  ;Push String to replace (needle)
  ;Push Replacement
  ;Call StrRep
  ;Pop $R0 result	
 
  Exch $R4 ; $R4 = Replacement String
  Exch
  Exch $R3 ; $R3 = String to replace (needle)
  Exch 2
  Exch $R1 ; $R1 = String to do replacement in (haystack)
  Push $R2 ; Replaced haystack
  Push $R5 ; Len (needle)
  Push $R6 ; len (haystack)
  Push $R7 ; Scratch reg
  StrCpy $R2 ""
  StrLen $R5 $R3
  StrLen $R6 $R1
loop:
  StrCpy $R7 $R1 $R5
  StrCmp $R7 $R3 found
  StrCpy $R7 $R1 1 ; - optimization can be removed if U know len needle=1
  StrCpy $R2 "$R2$R7"
  StrCpy $R1 $R1 $R6 1
  StrCmp $R1 "" done loop
found:
  StrCpy $R2 "$R2$R4"
  StrCpy $R1 $R1 $R6 $R5
  StrCmp $R1 "" done loop
done:
  StrCpy $R3 $R2
  Pop $R7
  Pop $R6
  Pop $R5
  Pop $R2
  Pop $R1
  Pop $R4
  Exch $R3
	
FunctionEnd

