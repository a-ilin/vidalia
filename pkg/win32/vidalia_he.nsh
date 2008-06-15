;; $Id$
;; 
;; Vidalia installer and bundle Hebrew translation
;;
;; Your resulting file should use a non-Unicode character encoding, since NSIS 
;; does not support Unicode.
;; 

;; This file is encoded in the Windows-1255 codepage.

; Bundle
LangString BundleSetupCaption         ${LANG_HEBREW} "התקנת ${BUNDLE_NAME}"
LangString BundleWelcomeText          ${LANG_HEBREW} "אשף זה ינחה אותך בעת התקנת Tor, Vidalia, Privoxy ו-Torbutton.\r\n\r\nTor היא מערכת לשימוש אנונימי באינטרנט, לגלישה ופרסום אתרי רשת, תקשרות מסרים מידיים, IRC ועוד. Vidalia היא ממשק גראפי לשליטה, ניטור והגדרת Tor.\r\n\r\nPrivoxy הוא שרת פרוקסי מסנן המגן על פרטיותך ועוזר להסיר פרסומות וחלונות קופצים.\r\n\r\nTorbutton היא הרחבת Firefox המאפשרת להפעיל ולכבות גלישה אנונימית במהירות.\r\n\r\nאם התקנת Tor, Vidalia, Privoxy או Firefox בעבר, אנא ודא שהם לא פעילים לפני המשך ההתקנה.\r\n\r\n$_CLICK"
LangString BundleWelcomeTitle         ${LANG_HEBREW} "ברוכים הבאים לאשף התקנת ${BUNDLE_NAME}"
LangString BundleLinkText             ${LANG_HEBREW} "מסמכי התקנה של ${TOR_NAME}"
LangString BundleFinishText           ${LANG_HEBREW} "ההתקנה הסתיימה.\r\nאנא ראה http://tor.eff.org/docs/tor-doc-win32.html על מנת ללמוד כיצד להגדיר את התוכנות לשימוש ב-Tor.\r\n\r\n אם התקנת Torbutton יש צורך להתחיל מחדש את Firefox."
LangString BundleRunNow               ${LANG_HEBREW} "הרץ רכיבים מותקנים כעת"

; Vidalia
LangString VidaliaGroupDesc           ${LANG_HEBREW} "Vidalia הינו ממשק גרפי המאפשר שליטה, ניטור והגדרת Tor."
LangString VidaliaUninstDesc          ${LANG_HEBREW} "הסר ${VIDALIA_DESC}."
LangString VidaliaSetupCaption        ${LANG_HEBREW} "התקנת ${VIDALIA_NAME}"
LangString VidaliaWelcomeText         ${LANG_HEBREW} "אשף זה ידריך אותך במהלך התקנת Vidalia, ממשק גרפי העוזר בשליטה, ניטור והגדרת Tor.\r\n\r\n$_CLICK"
LangString VidaliaWelcomeTitle        ${LANG_HEBREW} "ברוכים הבאים לאשף התקנת ${VIDALIA_NAME}"
LangString VidaliaLinkText            ${LANG_HEBREW} "עמוד הבית של ${VIDALIA_NAME}"
LangString VidaliaAppDesc             ${LANG_HEBREW} "Vidalia הינו ממשק גרפי המאפשר שליטה, ניטור והגדרת Tor."
LangString VidaliaStartup             ${LANG_HEBREW} "הרץ בעת אתחול"
LangString VidaliaStartupDesc         ${LANG_HEBREW} "הרץ ${VIDALIA_NAME} אוטומטית בעת אתחול."
LangString VidaliaShortcuts           ${LANG_HEBREW} "הוסף לתפריט התחלה"
LangString VidaliaShortcutsDesc       ${LANG_HEBREW} "הוסף את ${VIDALIA_NAME} לתפריט התחלה."
LangString VidaliaRunNow              ${LANG_HEBREW} "הרץ את ${VIDALIA_NAME} כעת"

; Tor
LangString TorGroupDesc               ${LANG_HEBREW} "Tor היא מערכת לשימוש אנונימי באינטרנט, לגלישה ופרסום אתרי רשת, תקשרות מסרים מידיים, IRC ועוד."
LangString TorUninstDesc              ${LANG_HEBREW} "הסר ${TOR_DESC}."
LangString TorAppDesc                 ${LANG_HEBREW} "התקן ${TOR_DESC}."
LangString TorAskOverwriteTorrc       ${LANG_HEBREW} "קובץ הגדרות Tor נמצא.$\r$\nהאם ברצונך להחליפו בקובץ הגדרות ברירת המחדל?"
LangString TorDocumentation           ${LANG_HEBREW} "מסמכים"
LangString TorDocumentationDesc       ${LANG_HEBREW} "התקן מסמכי ${TOR_NAME}."
LangString TorShortcuts               ${LANG_HEBREW} "הוסף לתפריט התחלה"
LangString TorShortcutsDesc           ${LANG_HEBREW} "הוסף את ${TOR_NAME} לתפריט התחלה."

; Privoxy
LangString PrivoxyGroupDesc           ${LANG_HEBREW} "Privoxy הינו שרת פרוקסי מסנן המגן על פרטיותך ועוזר בהסרת פרסומות וחלונות קופצים."
LangString PrivoxyUninstDesc          ${LANG_HEBREW} "הסר ${PRIVOXY_DESC}."
LangString PrivoxyAppDesc             ${LANG_HEBREW} "התקן ${PRIVOXY_DESC}."
LangString PrivoxyShortcuts           ${LANG_HEBREW} "הוסף לתפריט התחלה"
LangString PrivoxyShortcutsDesc       ${LANG_HEBREW} "הוסף את ${PRIVOXY_NAME} לתפריט התחלה."
LangString PrivoxyStartup             ${LANG_HEBREW} "הרץ בעת אתחול"
LangString PrivoxyStartupDesc         ${LANG_HEBREW} "הרץ ${PRIVOXY_NAME} אוטומטית בעת אתחול."

; Torbutton
LangString TorbuttonGroupDesc         ${LANG_HEBREW} "Torbutton הינה הרחבת Firefox המאפשרת הפעלה וכיבוי מהיר של גלישה אנונימית."
LangString TorbuttonUninstDesc        ${LANG_HEBREW} "הסר ${TORBUTTON_DESC}."
LangString TorbuttonAppDesc           ${LANG_HEBREW} "התקן ${TORBUTTON_DESC}."
LangString TorbuttonAddToFirefox      ${LANG_HEBREW} "הוסף ל-Firefox"
LangString TorbuttonAddToFirefoxDesc  ${LANG_HEBREW} "הוסף את הרחבת ${TORBUTTON_DESC} ל-Firefox."
LangString TorbuttonFirefoxNotFound   ${LANG_HEBREW} "Firefox לא נמצא במערכת. לא מתקין Torbutton."

; Miscellaneous Strings
LangString AppData                    ${LANG_HEBREW} "מידע תוכנה"
LangString AppDataUninstDesc          ${LANG_HEBREW} "הסר מידע תוכנה שמור וקבצי הגדרה."
LangString LanguageCode               ${LANG_HEBREW} "he"
