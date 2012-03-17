/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QTextStream>
#include <QTextCodec>
#include <QDateTime>
#include <stdlib.h>

#include "ts2po_config.h"

#define TS_DOCTYPE                    "TS"
#define TS_ELEMENT_CONTEXT            "context"
#define TS_ELEMENT_NAME               "name"
#define TS_ELEMENT_MESSAGE            "message"
#define TS_ELEMENT_SOURCE             "source"
#define TS_ELEMENT_TRANSLATION        "translation"
#define TS_ELEMENT_LOCATION           "location"
#define TS_ATTR_FILENAME              "filename"
#define TS_ATTR_LINE                  "line"


/** Return the current time (in UTC) in the format YYYY-MM-DD HH:MM+0000. */
QString
create_po_timestamp()
{
  QDateTime now = QDateTime::currentDateTime().toUTC();
  return now.toString("yyyy-MM-dd hh:mm+0000");
}

/** Return a header to be placed at the top of the .po file. The header will
 * include <b>encoding</b> in the Content-Type header line. */
QString
create_po_header(const QString &encoding)
{
  QString header;
  QString tstamp = create_po_timestamp();

  header.append("msgid \"\"\n");
  header.append("msgstr \"\"\n");
  header.append("\"Project-Id-Version: "TS2PO_PROJECT_ID"\\n\"\n");
  header.append("\"Report-Msgid-Bugs-To: "TS2PO_CONTACT_ADDR"\\n\"\n");
  header.append(QString("\"POT-Creation-Date: %1\\n\"\n").arg(tstamp));
  header.append("\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n");
  header.append("\"Last-Translator: \\n\"\n");
  header.append("\"Language-Team: "TS2PO_LANGUAGE_TEAM"\\n\"\n");
  header.append("\"MIME-Version: 1.0\\n\"\n");
  header.append("\"Content-Type: text/plain; ");
  header.append(QString("charset=%1\\n\"\n").arg(encoding));
  header.append("\"Content-Transfer-Encoding: 8bit\\n\"\n");
  header.append("\"Plural-Forms: nplurals=INTEGER; plural=EXPRESSION;\\n\"\n");
  header.append("\"X-Generator: Vidalia ts2po "TS2PO_VERSION"\\n\"\n");
  header.append("\n");

  return header;
}

/** Parse the filename from the relative or absolute path given in
 * <b>filePath</b>. */
QString
parse_filename(const QString &filePath)
{
  QFileInfo file(filePath);
  return file.fileName();
}

/** Convert the messages in <b>context</b> to PO format. The output will be
 * appended to <b>po</b>. Returns the number of source messages converted on
 * success, or -1 on error and <b>errorMessage</b> will be set. */
int
convert_context(const QDomElement &context, QString *po, QString *errorMessage)
{
  QString msgctxt, msgid, msgstr;
  QString filename, line;
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
  msgctxt = name.text();

  QDomElement msg = context.firstChildElement(TS_ELEMENT_MESSAGE);
  while (!msg.isNull()) {
    /* Extract the <source> tags */
    source = msg.firstChildElement(TS_ELEMENT_SOURCE);
    if (source.isNull()) {
      *errorMessage = QString("message element with no source string "
                              "(line %1)").arg(msg.lineNumber());
      return -1;
    }
    msgid = source.text().trimmed();
    msgid.replace("\r", "");
    msgid.replace("\"", "\\\"");
    msgid.replace("\n", "\\n\"\n\"");

    /* Extract the <translation> tags */
    translation = msg.firstChildElement(TS_ELEMENT_TRANSLATION);
    msgstr = translation.text().trimmed();
    msgstr.replace("\r", "");
    msgstr.replace("\"", "\\\"");
    msgstr.replace("\n", "\\n\"\n\"");

    /* Try to extract the <location> tags (optional) */
    location = msg.firstChildElement(TS_ELEMENT_LOCATION);
    filename = parse_filename(location.attribute(TS_ATTR_FILENAME));
    line = location.attribute(TS_ATTR_LINE);

    /* Format the .po entry for this string */
    if (!filename.isEmpty() && !line.isEmpty())
      (*po).append(QString("#: %1:%2\n").arg(filename).arg(line));
    (*po).append(QString("msgctxt \"%1\"\n").arg(msgctxt));
    (*po).append(QString("msgid \"%1\"\n").arg(msgid));
    (*po).append(QString("msgstr \"%1\"\n").arg(msgstr));
    (*po).append("\n");

    /* Find the next source message in the current context */
    msg = msg.nextSiblingElement(TS_ELEMENT_MESSAGE);
    n++;
  }
  return n;
}

/** Convert the TS-formatted document in <b>ts</b> to a PO-formatted document.
 * The output will be written to <b>po</b>, including a file header that
 * specifies <b>encoding</b> as the character set. Returns the number of strings
 * converted on success, or -1 on error and <b>errorMessage</b> will be set. */
int
ts2po(const QDomDocument *ts, QString *po, const QString &encoding,
      QString *errorMessage)
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
  *po = create_po_header(encoding);

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
  error << "usage: ts2po [-q] -i <infile.ts> -o <outfile.po> "
           "[-c <encoding>]\n";
  error << "  -q (optional)   Quiet mode (errors are still displayed)\n";
  error << "  -i <infile.ts>  Input .ts file\n";
  error << "  -o <outfile.po> Output .po file\n";
  error << "  -c <encoding>   Text encoding (default: utf-8)\n";
  error.flush();
  exit(1);
}

int
main(int argc, char *argv[])
{
  QTextStream error(stderr);
  QString errorMessage;
  char *infile, *outfile;
  QTextCodec *codec = QTextCodec::codecForName("utf-8");
  bool quiet = false;

  /* Check for the correct number of input parameters. */
  if (argc < 5 || argc > 8)
    print_usage_and_exit();
  for (int i = 1; i < argc; i++) {
    QString arg(argv[i]);
    if (!arg.compare("-q", Qt::CaseInsensitive))
      quiet = true;
    else if (!arg.compare("-i", Qt::CaseInsensitive) && ++i < argc)
      infile = argv[i];
    else if (!arg.compare("-o", Qt::CaseInsensitive) && ++i < argc)
      outfile = argv[i];
    else if (!arg.compare("-c", Qt::CaseInsensitive) && ++i < argc) {
      codec = QTextCodec::codecForName(argv[i]);
      if (!codec) {
        error << "Invalid text encoding specified.\n";
        return 1;
      }
    } else
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
  int n_strings = ts2po(&ts, &po, QString(codec->name()), &errorMessage);
  if (n_strings < 0) {
    error << QString("Unable to convert '%1' to '%2': %3\n").arg(infile)
                                                            .arg(outfile)
                                                            .arg(errorMessage);
    return 3;
  }

  /* Write the .po output. */
  QTextStream out(&poFile);
  out.setCodec(codec);
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

