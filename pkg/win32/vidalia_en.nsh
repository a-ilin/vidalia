;; $Id$
;; 
;; Vidalia installer and bundle English translation
;; 
;; All translations should be derived from this file.
;;
;; To translate this file:
;;   
;;    1) Copy this file (vidalia_en.nsh), creating a new file with "en"
;;       replaced by the two-letter language code of your translated language.
;;       For example, if you are creating a German translation, your new file
;;       would be named "vidalia_de.nsh".
;;
;;    2) Replace all ${LANG_ENGLISH} tags with whatever language you are
;;       translating this file into. For example, if you are creating a German
;;       translation, all ${LANG_ENGLISH} tags would become ${LANG_GERMAN}.
;;
;;    3) Translate the quoted strings. You can move string replacement tags
;;       (e.g., ${BUNDLE_NAME}) around within the quoted strings, but you 
;;       should not alter the tags themselves.
;;
;; Your resulting file should use a non-Unicode character encoding, since NSIS 
;; does not support Unicode.
;; 


; Bundle
LangString BundleSetupCaption         ${LANG_ENGLISH} "${BUNDLE_NAME} setup"
LangString BundleWelcomeText          ${LANG_ENGLISH} "This wizard will guide you through the installation of Tor, Vidalia, Polipo, and Torbutton.\r\n\r\nTor is a system for using the Internet anonymously, helping you anonymize Web browsing and publishing, instant messaging, IRC, and more. Vidalia is a GUI that helps you control, monitor, and configure Tor.\r\n\r\nPolipo is a caching web proxy that helps make web browsing over Tor more responsive.\r\n\r\nTorbutton is a Firefox extension that allows you to quickly enable or disable anonymous web browsing.\r\n\r\nIf you have previously installed Tor, Vidalia, Polipo, or Firefox, please make sure they are not running before continuing this installation.\r\n\r\n$_CLICK"
LangString BundleWelcomeTitle         ${LANG_ENGLISH} "Welcome to the ${BUNDLE_NAME} Setup Wizard"
LangString BundleLinkText             ${LANG_ENGLISH} "${TOR_NAME} installation documentation"
LangString BundleFinishText           ${LANG_ENGLISH} "Installation is complete.\r\nPlease see http://tor.eff.org/docs/tor-doc-win32.html to learn how to configure your applications to use Tor.\r\n\r\nIf you installed Torbutton, you will need to restart Firefox."
LangString BundleRunNow               ${LANG_ENGLISH} "Run installed components now"

; Vidalia
LangString VidaliaGroupDesc           ${LANG_ENGLISH} "Vidalia is a GUI that helps you control, monitor, and configure Tor."
LangString VidaliaUninstDesc          ${LANG_ENGLISH} "Remove ${VIDALIA_DESC}."
LangString VidaliaSetupCaption        ${LANG_ENGLISH} "${VIDALIA_NAME} setup"
LangString VidaliaWelcomeText         ${LANG_ENGLISH} "This wizard will guide you through the installation of Vidalia, a GUI that helps you control, monitor, and configure Tor.\r\n\r\n$_CLICK"
LangString VidaliaWelcomeTitle        ${LANG_ENGLISH} "Welcome to the ${VIDALIA_NAME} Setup Wizard"
LangString VidaliaLinkText            ${LANG_ENGLISH} "${VIDALIA_NAME} homepage"
LangString VidaliaAppDesc             ${LANG_ENGLISH} "Vidalia is a GUI that helps you control, monitor, and configure Tor."
LangString VidaliaStartup             ${LANG_ENGLISH} "Run At Startup"
LangString VidaliaStartupDesc         ${LANG_ENGLISH} "Automatically run ${VIDALIA_NAME} at startup."
LangString VidaliaShortcuts           ${LANG_ENGLISH} "Add to Start Menu"
LangString VidaliaShortcutsDesc       ${LANG_ENGLISH} "Add ${VIDALIA_NAME} to your Start menu."
LangString VidaliaRunNow              ${LANG_ENGLISH} "Run ${VIDALIA_NAME} now"

; Tor
LangString TorGroupDesc               ${LANG_ENGLISH} "Tor is a system for using the Internet anonymously, helping you anonymize Web browsing and publishing, instant messaging, IRC, and more."
LangString TorUninstDesc              ${LANG_ENGLISH} "Remove ${TOR_DESC}."
LangString TorAppDesc                 ${LANG_ENGLISH} "Install ${TOR_DESC}."
LangString TorAskOverwriteTorrc       ${LANG_ENGLISH} "You already have a Tor configuration file.$\r$\nDo you want to overwrite it with the default sample configuration file?"
LangString TorDocumentation           ${LANG_ENGLISH} "Documentation"
LangString TorDocumentationDesc       ${LANG_ENGLISH} "Install ${TOR_NAME} documentation."
LangString TorShortcuts               ${LANG_ENGLISH} "Add to Start Menu"
LangString TorShortcutsDesc           ${LANG_ENGLISH} "Add ${TOR_NAME} to your Start menu."

; Polipo
LangString PolipoGroupDesc           ${LANG_ENGLISH} "Polipo is a caching web proxy that helps make web browsing over Tor more responsive."
LangString PolipoUninstDesc          ${LANG_ENGLISH} "Remove ${POLIPO_DESC}."
LangString PolipoAppDesc             ${LANG_ENGLISH} "Install ${POLIPO_DESC}."
LangString PolipoShortcuts           ${LANG_ENGLISH} "Add to Start Menu"
LangString PolipoShortcutsDesc       ${LANG_ENGLISH} "Add ${POLIPO_NAME} to your Start menu."

; Torbutton
LangString TorbuttonGroupDesc         ${LANG_ENGLISH} "Torbutton is a Firefox extension that allows you to quickly enable or disable anonymous web browsing."
LangString TorbuttonUninstDesc        ${LANG_ENGLISH} "Remove ${TORBUTTON_DESC}."
LangString TorbuttonAppDesc           ${LANG_ENGLISH} "Install ${TORBUTTON_DESC}."
LangString TorbuttonAddToFirefox      ${LANG_ENGLISH} "Add to Firefox"
LangString TorbuttonAddToFirefoxDesc  ${LANG_ENGLISH} "Add the ${TORBUTTON_DESC} extension to Firefox."
LangString TorbuttonFirefoxNotFound   ${LANG_ENGLISH} "Firefox was not found on your system. Not installing Torbutton."

; Miscellaneous Strings
LangString AppData                    ${LANG_ENGLISH} "Application Data"
LangString AppDataUninstDesc          ${LANG_ENGLISH} "Remove saved application data and configuration files."

