/*
**  $Id$
**
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QTextCodec>
#include <stdlib.h>

#define WXL_NAMESPACE                  "http://schemas.microsoft.com/wix/2006/localization"
#define WXL_ELEMENT_ROOT               "WixLocalization"
#define WXL_ELEMENT_MESSAGE            "String"
#define WXL_ATTR_MESSAGE_ID            "Id"
#define WXL_ATTR_LANGUAGE              "LCID"
#define WXL_ATTR_TRANSLATION_TYPE      "Culture"
#define WXL_ATTR_OVERRIDABLE           "Overridable"

/** We need to provide an element with the LCID for this locale
 * that is used in the WiX Product definition. */
QString
culture_lcid(const QString &culture)
{
  /* For now character encoding focused, not generally locale / dialect aware. */
  QString lcid = "0";
  if(!culture.compare("en", Qt::CaseInsensitive))
    lcid = "1033";
  else if(!culture.compare("cs", Qt::CaseInsensitive))
    lcid = "1029";
  else if(!culture.compare("de", Qt::CaseInsensitive))
    lcid = "1031";
  else if(!culture.compare("es", Qt::CaseInsensitive))
    lcid = "1034";
  else if(!culture.compare("fa", Qt::CaseInsensitive))
    lcid = "1065";
  else if(!culture.compare("fi", Qt::CaseInsensitive))
    lcid = "1035";
  else if(!culture.compare("fr", Qt::CaseInsensitive))
    lcid = "1036";
  else if(!culture.compare("he", Qt::CaseInsensitive))
    lcid = "1037";
  else if(!culture.compare("it", Qt::CaseInsensitive))
    lcid = "1040";
  else if(!culture.compare("nl", Qt::CaseInsensitive))
    lcid = "1043";
  else if(!culture.compare("pl", Qt::CaseInsensitive))
    lcid = "1045";
  else if(!culture.compare("pt", Qt::CaseInsensitive))
    lcid = "1046";
  else if(!culture.compare("ro", Qt::CaseInsensitive))
    lcid = "1048";
  else if(!culture.compare("ru", Qt::CaseInsensitive))
    lcid = "1049";
  else if(!culture.compare("sv", Qt::CaseInsensitive))
    lcid = "1053";
  return lcid;
}

/** Create a new message string element using the source string <b>msgid</b>
 * and the translation <b>msgstr</b> and assign identifier attribute. */
QDomElement
new_message_element(QDomDocument *wxl, const QString &strid,
                    const QString &msgid, const QString &msgstr)
{
  QDomElement message;

  message = wxl->createElement(WXL_ELEMENT_MESSAGE);
  message.setAttribute(WXL_ATTR_MESSAGE_ID, strid);

  /* Always allow localized string to be dynamic. This is required for
   * multi-language packages to link correctly.
   */
  message.setAttribute(WXL_ATTR_OVERRIDABLE, "yes");
  if (!msgstr.isEmpty())
    message.appendChild(wxl->createTextNode(msgstr));
  else
    message.appendChild(wxl->createTextNode(msgid));

  return message;
}

/** Create a new WXL document of the appropriate doctype and root
 * element with the Microsoft style culture name for locale. */
QDomDocument
new_wxl_document(const QString &culture)
{
  QDomDocument wxl;

  QDomElement root = wxl.createElementNS(WXL_NAMESPACE, WXL_ELEMENT_ROOT);
  root.setAttribute(WXL_ATTR_TRANSLATION_TYPE, culture);
  wxl.appendChild(root);

  return wxl;
}

/** Parse the context name from <b>str</b>, where the context name is of the
 * form DQUOTE ContextName DQUOTE. */
QString
parse_message_context(const QString &str)
{
  QString out = str.trimmed();
  out = out.replace("\"", "");
  return out;
}

/** Parse the context name from <b>str</b>, where <b>str</b> is of the
 * form ContextName#Number. This is the format used by translate-toolkit. */
QString
parse_message_context_lame(const QString &str)
{
  if (str.contains("#"))
    return str.section("#", 0, 0);
  return QString();
}

/** Parse the PO-formatted message string from <b>msg</b>. If <b>msg</b> is a
 * multiline string, the extra double quotes will be replaced with newlines
 * appropriately. */
QString
parse_message_string(const QString &msg)
{
  QString out = msg.trimmed();

  out.replace("\"\n\"", "\n");
  if (out.startsWith("\""))
    out = out.remove(0, 1);
  if (out.endsWith("\""))
    out.chop(1);
  out.replace("\\\"", "\"");

  /* convert NSIS style vars to Properties; avoid QRegExp if possible. */
  int lind, rind;
  while ( ((lind = out.indexOf("${")) >= 0) &&
          ((rind = out.indexOf("}", lind)) > lind) ) {
    out.replace(lind, 2, "[");
    out.replace(--rind, 1, "]");
  }
  return out;
}

/** Read and return the next non-empty line from <b>stream</b>. */
QString
read_next_line(QTextStream *stream)
{
  stream->skipWhiteSpace();
  return stream->readLine().append("\n");
}

/** Skip past the header portion of the PO file and any leading whitespace.
 * The next line read from <b>po</b> will be the first non-header line in the
 * document. */
void
skip_po_header(QTextStream *po)
{
  QString line;
  /* Skip any leading whitespace before the header */
  po->skipWhiteSpace();
  /* Read to the first empty line */
  line = po->readLine();
  while (!po->atEnd() && !line.isEmpty())
    line = po->readLine();
}

/** Convert <b>po</b> from the PO format to a WXL-formatted XML document.
 * <b>wxl</b> will be set to the resulting WXL document. Return the number of
 * converted strings on success, or -1 on error and <b>errorMessage</b> will
 * be set. */
int
po2wxl(const QString& culture, QTextStream *po, QDomDocument *wxl,
  QString *errorMessage)
{
  QString line;
  QString msgctxt, msgid, msgstr;
  QDomElement msgElement;
  int n_strings = 0;

  Q_ASSERT(po);
  Q_ASSERT(wxl);
  Q_ASSERT(errorMessage);

  *wxl = new_wxl_document(culture);

  /* Set the LCID to Language code for use as !(loc.LCID) in Product. */
  QString lcid = culture_lcid(culture);
  wxl->documentElement().appendChild(
    new_message_element(wxl, WXL_ATTR_LANGUAGE, lcid, lcid));

  skip_po_header(po);
  line = read_next_line(po);
  while (!po->atEnd()) {
    /* Ignore all "#" lines except "#:" */
    while (line.startsWith("#")) {
      if (line.startsWith("#:")) {
        /* Context was specified with the stupid overloaded "#:" syntax.*/
        msgctxt = line.section(" ", 1);
        msgctxt = parse_message_context_lame(msgctxt);
      }
      line = read_next_line(po);
    }

    /* A context specified on a "msgctxt" line takes precedence over a context
     * specified using the overload "#:" notation. */
    if (line.startsWith("msgctxt ")) {
      msgctxt = line.section(" ", 1);
      msgctxt = parse_message_context(msgctxt);
      line = read_next_line(po);
    }

    /* Parse the (possibly multiline) message source string */
    if (!line.startsWith("msgid ")) {
      *errorMessage = "expected 'msgid' line";
      return -1;
    }
    msgid = line.section(" ", 1);

    line = read_next_line(po);
    while (line.startsWith("\"")) {
      msgid.append(line);
      line = read_next_line(po);
    }
    msgid = parse_message_string(msgid);

    /* Parse the (possibly multiline) translated string */
    if (!line.startsWith("msgstr ")) {
      *errorMessage = "expected 'msgstr' line";
      return -1;
    }
    msgstr = line.section(" ", 1);

    line = read_next_line(po);
    while (line.startsWith("\"")) {
      msgstr.append(line);
      line = read_next_line(po);
    }
    msgstr = parse_message_string(msgstr);

    /* Add the message and translation to the .wxl document */
    wxl->documentElement().appendChild(
      new_message_element(wxl, msgctxt, msgid, msgstr));

    n_strings++;
  }
  return n_strings;
}

/** Display application usage and exit. */
void
print_usage_and_exit()
{
  QTextStream error(stderr);
  error << "usage: po2wxl [-q] -n <culture name> -i <infile.po> -o <outfile.wxl> "
           "[-c <encoding>]\n";
  error << "  -q (optional)    Quiet mode (errors are still displayed)\n";
  error << "  -n <culture>     Culture name for translation\n";
  error << "  -i <infile.po>   Input .po file\n";
  error << "  -o <outfile.wxl> Output .wxl file\n";
  error << "  -c <encoding>    Text encoding (default: utf-8)\n";
  error.flush();
  exit(1);
}

int
main(int argc, char *argv[])
{
  QTextStream error(stderr);
  QString culture, errorMessage;
  char *infile, *outfile;
  QTextCodec *codec = QTextCodec::codecForName("utf-8");
  bool quiet = false;

  /* Check for the correct number of input parameters. */
  if (argc < 5 || argc > 9)
    print_usage_and_exit();
  for (int i = 1; i < argc; i++) {
    QString arg(argv[i]);
    if (!arg.compare("-q", Qt::CaseInsensitive))
      quiet = true;
    else if (!arg.compare("-n", Qt::CaseInsensitive) && ++i < argc)
      culture = argv[i];
    else if (!arg.compare("-i", Qt::CaseInsensitive) && ++i < argc)
      infile = argv[i];
    else if (!arg.compare("-o", Qt::CaseInsensitive) && ++i < argc)
      outfile = argv[i];
    else if (!arg.compare("-c", Qt::CaseInsensitive) && ++i < argc) {
      codec = QTextCodec::codecForName(argv[i]);
      if (!codec) {
        error << "Invalid text encoding specified\n";
        return 1;
      }
    } else
      print_usage_and_exit();
  }

  /* Open the input PO file for reading. */
  QFile poFile(infile);
  if (!poFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    error << QString("Unable to open '%1' for reading: %2\n").arg(infile)
                                                .arg(poFile.errorString());
    return 2;
  }

  QDomDocument wxl;
  QTextStream po(&poFile);
  po.setCodec(codec);
  int n_strings = po2wxl(culture, &po, &wxl, &errorMessage);
  if (n_strings < 0) {
    error << QString("Unable to convert '%1': %2\n").arg(infile)
                                                    .arg(errorMessage);
    return 3;
  }

  /* Open the WXL file for writing. */
  QFile wxlFile(outfile);
  if (!wxlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    error << QString("Unable to open '%1' for writing: %2\n").arg(outfile)
                                                .arg(wxlFile.errorString());
    return 4;
  }

  /* Write the .wxl output. */
  QTextStream out(&wxlFile);
  out.setCodec(codec);
  out << QString("<?xml version=\"1.0\" encoding=\"%1\"?>\n")
                                                  .arg(QString(codec->name()));
  out << wxl.toString(4);

  if (!quiet) {
    QTextStream results(stdout);
    results << QString("Converted %1 strings from %2 to %3.\n").arg(n_strings)
                                                               .arg(infile)
                                                               .arg(outfile);
  }
  return 0;
}

