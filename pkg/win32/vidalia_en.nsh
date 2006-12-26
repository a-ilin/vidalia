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
LangString BundleWelcomeText          ${LANG_ENGLISH} "This wizard will guide you through the installation of Tor, Vidalia, Privoxy, and Torbutton.\r\n\r\nIf you have previously installed Tor, Vidalia, Privoxy, or Firefox, please make sure they are not running before continuing this installation.\r\n\r\n$_CLICK"
LangString BundleWelcomeTitle         ${LANG_ENGLISH} "Welcome to the ${BUNDLE_NAME} Setup Wizard"
LangString BundleLinkText             ${LANG_ENGLISH} "${TOR_NAME} installation documentation"
LangString BundleFinishText           ${LANG_ENGLISH} "Installation is complete.\r\nPlease see http://tor.eff.org/docs/tor-doc-win32.html to learn how to configure your applications to use Tor.\r\n\r\nIf you installed Torbutton, you will need to restart Firefox."
LangString BundleRunNow               ${LANG_ENGLISH} "Run installed components now"

; Vidalia
LangString VidaliaGroupDesc           ${LANG_ENGLISH} "Install ${VIDALIA_DESC}."
LangString VidaliaSetupCaption        ${LANG_ENGLISH} "${VIDALIA_NAME} setup"
LangString VidaliaWelcomeText         ${LANG_ENGLISH} "This wizard will guide you through the installation of ${VIDALIA_DESC}.\r\n\r\n$_CLICK"
LangString VidaliaWelcomeTitle        ${LANG_ENGLISH} "Welcome to the ${VIDALIA_NAME} Setup Wizard"
LangString VidaliaLinkText            ${LANG_ENGLISH} "${VIDALIA_NAME} homepage"
LangString VidaliaAppDesc             ${LANG_ENGLISH} "Install ${VIDALIA_DESC}."
LangString VidaliaStartup             ${LANG_ENGLISH} "Run At Startup"
LangString VidaliaStartupDesc         ${LANG_ENGLISH} "Automatically run ${VIDALIA_NAME} at startup."
LangString VidaliaShortcuts           ${LANG_ENGLISH} "Add to Start Menu"
LangString VidaliaShortcutsDesc       ${LANG_ENGLISH} "Add ${VIDALIA_NAME} to your Start menu."
LangString VidaliaRunNow              ${LANG_ENGLISH} "Run ${VIDALIA_NAME} now"

; Tor
LangString TorGroupDesc               ${LANG_ENGLISH} "Install ${TOR_DESC}."
LangString TorAppDesc                 ${LANG_ENGLISH} "Install ${TOR_DESC}."
LangString TorAskOverwriteTorrc       ${LANG_ENGLISH} "You already have a Tor configuration file.$\r$\nDo you want to overwrite it with the default sample configuration file?"
LangString TorOpenSSL                 ${LANG_ENGLISH} "OpenSSL ${OPENSSL_APPVERSION}"
LangString TorOpenSSLDesc             ${LANG_ENGLISH} "Install ${OPENSSL_DESC}."
LangString TorDocumentation           ${LANG_ENGLISH} "Documentation"
LangString TorDocumentationDesc       ${LANG_ENGLISH} "Install ${TOR_NAME} documentation."
LangString TorShortcuts               ${LANG_ENGLISH} "Add to Start Menu"
LangString TorShortcutsDesc           ${LANG_ENGLISH} "Add ${TOR_NAME} to your Start menu."

; Privoxy
LangString PrivoxyGroupDesc           ${LANG_ENGLISH} "Install ${PRIVOXY_DESC}"
LangString PrivoxyAppDesc             ${LANG_ENGLISH} "Install ${PRIVOXY_DESC}."
LangString PrivoxyShortcuts           ${LANG_ENGLISH} "Add to Start Menu"
LangString PrivoxyShortcutsDesc       ${LANG_ENGLISH} "Add ${PRIVOXY_NAME} to your Start menu."
LangString PrivoxyStartup             ${LANG_ENGLISH} "Run At Startup"
LangString PrivoxyStartupDesc         ${LANG_ENGLISH} "Automatically run ${PRIVOXY_NAME} at startup."

; Torbutton
LangString TorbuttonGroupDesc         ${LANG_ENGLISH} "Install ${TORBUTTON_DESC}."
LangString TorbuttonAppDesc           ${LANG_ENGLISH} "Install ${TORBUTTON_DESC}."
LangString TorbuttonAddToFirefox      ${LANG_ENGLISH} "Add to Firefox"
LangString TorbuttonAddToFirefoxDesc  ${LANG_ENGLISH} "Add the ${TORBUTTON_DESC} extension to Firefox."
LangString TorbuttonFirefoxNotFound   ${LANG_ENGLISH} "Firefox was not found on your system. Not installing Torbutton."

