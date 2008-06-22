/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.vidalia-project.net/. No part of Vidalia, including this file,
**  may be copied, modified, propagated, or distributed except according to the
**  terms described in the LICENSE file.
*/

#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QDateTime>

#include "ts2po_config.h"

#define TS_DOCTYPE                    "TS"
#define TS_ELEMENT_CONTEXT            "context"
#define TS_ELEMENT_NAME               "name"
#define TS_ELEMENT_MESSAGE            "message"
#define TS_ELEMENT_SOURCE             "source"
#define TS_ELEMENT_TRANSLATION        "translation"


/** Return the current time (in UTC) in the format YYYY-MM-DD HH:MM+0000. */
QString
create_po_timestamp()
{
  QDateTime now = QDateTime::currentDateTime().toUTC();
  return now.toString("yyyy-MM-dd hh:mm+0000");
}

/** Return a header to be placed at the top of the .po file. */
QString
create_po_header()
{
  QString header;
  QString tstamp = create_po_timestamp();

  header.append("#,fuzzy\n");
  header.append("msgid \"\"\n");
  header.append("msgstr \"\"\n");
  header.append("\"Project-Id-Version: "TS2PO_PROJECT_ID"\\n\"\n");
  header.append("\"Report-Msgid-Bugs-To: "TS2PO_CONTACT_ADDR"\\n\"\n");
  header.append(QString("\"POT-Creation-Date: %1\\n\"\n").arg(tstamp));
  header.append("\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n");
  header.append("\"Last-Translator: \\n\"\n");
  header.append("\"Language-Team: "TS2PO_LANGUAGE_TEAM"\\n\"\n");
  header.append("\"MIME-Version: 1.0\\n\"\n");
  header.append("\"Content-Type: text/plain; charset=UTF-8\\n\"\n");
  header.append("\"Content-Transfer-Encoding: 8bit\\n\"\n");
  header.append("\"Plural-Forms: nplurals=INTEGER; plural=EXPRESSION;\\n\"\n");
  header.append("\"X-Generator: Vidalia ts2po "TS2PO_VERSION"\\n\"\n");
  header.append("\n");

  return header;
}

/** Convert the messages in <b>context</b> to PO format. The output will be
 * appended to <b>po</b>. Returns the number of source messages converted on
 * success, or -1 on error and <b>errorMessage</b> will be set. */
int
convert_context(const QDomElement &context, QString *po, QString *errorMessage)
{
  QString msgid, msgstr;
  QDomElement location, source, translation;
  int n = 0;

  Q_ASSERT(po);
  Q_ASSERT(errorMessage);

  QDomElement name = context.firstChildElement(TS_ELEMENT_NAME);
  if (name.isNull()) {
    *errorMessage = QString("context element with no name (line %1)")
                                                 .arg(context.lineNumber());
    return -1;
  }

  QDomElement msg = context.firstChildElement(TS_ELEMENT_MESSAGE);
  while (!msg.isNull()) {
    /* Extract the <source> tags */
    source = msg.firstChildElement(TS_ELEMENT_SOURCE);
    if (source.isNull()) {
      *errorMessage = QString("message element with no source string "
                              "(line %1)").arg(msg.lineNumber());
      return -1;
    }
    msgid = source.text();
    msgid.replace("\r", "");
    msgid.replace("\"", "\\\"");
    msgid.replace("\n", "\"\n\"");

    /* Extract the <translation> tags */
    translation = msg.firstChildElement(TS_ELEMENT_TRANSLATION);
    msgstr = translation.text();
    msgstr.replace("\r", "");
    msgstr.replace("\"", "\\\"");
    msgstr.replace("\n", "\"\n\"");
  
    /* Format the .po entry for this string */
    (*po).append(QString("#: %1#%2\n").arg(name.text()).arg(++n));
    (*po).append(QString("msgid \"%1\"\n").arg(msgid));
    (*po).append(QString("msgstr \"%1\"\n").arg(msgstr));
    (*po).append("\n");
  
    /* Find the next source message in the current context */
    msg = msg.nextSiblingElement(TS_ELEMENT_MESSAGE);
  }
  return n;
}

/** Convert the TS-formatted document in <b>ts</b> to a PO-formatted document.
 * The output will be written to <b>po</b>. Returns the number of strings
 * converted on success, or -1 on error and <b>errorMessage</b> will be set. */
int
ts2po(const QDomDocument *ts, QString *po, QString *errorMessage)
{
  int n_strings = 0;
  QString context;

  Q_ASSERT(ts);
  Q_ASSERT(po);
  Q_ASSERT(errorMessage);

  /* Get the document root and check that it's valid */
  QDomElement root = ts->documentElement(); 
  if (root.tagName() != TS_DOCTYPE)
    return -1;

  /* Start with the PO header */
  *po = create_po_header();

  /* Iterate through all of the translation contexts and build up the PO file
   * output. */
  QDomElement child = root.firstChildElement(TS_ELEMENT_CONTEXT);
  while (!child.isNull()) {
    QString context;
  
    /* Convert the current .ts context to .po */
    int n = convert_context(child, &context, errorMessage);
    if (n < 0)
      return -1;
    
    /* Add it to the output file */
    (*po).append(context);
    n_strings += n;

    /* Move to the next context */
    child = child.nextSiblingElement(TS_ELEMENT_CONTEXT);
  }
  return n_strings;
}

/** Display application usage and exit. */
void
print_usage_and_exit()
{
  QTextStream error(stderr);
  error << "usage: ts2po [-q] -i <infile.ts> -o <outfile.po>\n";
  error << "  -q (optional)   Quiet mode (errors are still displayed)\n";
  error << "  -i <infile.ts>  Input .ts file\n";
  error << "  -o <outfile.po> Output .po file\n";
  error.flush();
  exit(1);
}

int
main(int argc, char *argv[])
{
  QTextStream error(stderr);
  QString errorMessage;
  char *infile, *outfile;
  bool quiet = false;
  
  /* Check for the correct number of input parameters. */
  if (argc < 5 || argc > 6)
    print_usage_and_exit();
  for (int i = 1; i < argc; i++) {
    QString arg(argv[i]);
    if (!arg.compare("-q", Qt::CaseInsensitive))
      quiet = true;
    else if (!arg.compare("-i", Qt::CaseInsensitive) && ++i < argc)
      infile = argv[i];
    else if (!arg.compare("-o", Qt::CaseInsensitive) && ++i < argc)
      outfile = argv[i];
    else
      print_usage_and_exit(); 
  }

  /* Read and parse the input .ts file. */
  QDomDocument ts;
  QFile tsFile(infile);
  if (!ts.setContent(&tsFile, true, &errorMessage)) {
    error << QString("Unable to parse '%1': %2\n").arg(infile)
                                                  .arg(errorMessage);
    return 1;
  }
  
  /* Try to open the output .po file for writing. */
  QFile poFile(outfile);
  if (!poFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    error << QString("Unable to open '%1' for writing: %2\n")
                                                   .arg(outfile)
                                                   .arg(tsFile.errorString());
    return 2;
  }
 
  /* Convert the input .ts file to a .po formatted file. */
  QString po;
  int n_strings = ts2po(&ts, &po, &errorMessage);
  if (n_strings < 0) {
    error << QString("Unable to convert '%1' to '%2': %3\n").arg(infile)
                                                            .arg(outfile)
                                                            .arg(errorMessage);
    return 3;
  }

  /* Write the .po output. */
  QTextStream out(&poFile);
  out.setCodec("UTF-8");
  out << po;
  poFile.close();
 
  if (!quiet) {
    QTextStream results(stdout);
    results << QString("Converted %1 strings from %2 to %3.\n").arg(n_strings)
                                                               .arg(infile)
                                                               .arg(outfile);
  }
  return 0;
}

