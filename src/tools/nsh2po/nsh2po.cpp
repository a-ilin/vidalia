/*
**  This file is part of Vidalia, and is subject to the license terms in the
**  LICENSE file, found in the top level directory of this distribution. If you
**  did not receive the LICENSE file with this file, you may obtain it from the
**  Vidalia source package distributed by the Vidalia Project at
**  http://www.torproject.org/projects/vidalia.html. No part of Vidalia,
**  including this file, may be copied, modified, propagated, or distributed
**  except according to the terms described in the LICENSE file.
*/

#include <QHash>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDateTime>
#include <QStringList>
#include <stdlib.h>

#include "nsh2po_config.h"


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

/** Parse the PO-formatted message string from <b>msg</b>. */
QString
parse_message_string(const QString &msg)
{
  QString out = msg.trimmed();

  if (out.startsWith("\""))
    out = out.remove(0, 1);
  if (out.endsWith("\""))
    out.chop(1);
  out.replace("\\\"", "\"");
  out.replace("\\r\\n", "\\n");
  return out;
}

/** Parse the NSIS-formatted LangString message from <b>msg</b>. */
QString
parse_nsh_langstring(const QString &msg)
{
  QString out = msg.trimmed();

  if (out.startsWith("\""))
    out = out.remove(0, 1);
  if (out.endsWith("\""))
    out.chop(1);
  out.replace("$\\n", "\\n");
  out.replace("$\\r", "");
  out.replace("\\r",  "");
  return out;
}

/** Return the current time (in UTC) in the format YYYY-MM-DD HH:MM+0000. */
QString
create_po_timestamp()
{
  QDateTime now = QDateTime::currentDateTime().toUTC();
  return now.toString("yyyy-MM-dd hh:mm+0000");
}

/** Return a header to be placed at the top of the .po file. */
QString
create_po_header(const QString &charset)
{
  QString header;
  QString tstamp = create_po_timestamp();

  header.append("msgid \"\"\n");
  header.append("msgstr \"\"\n");
  header.append("\"Project-Id-Version: "NSH2PO_PROJECT_ID"\\n\"\n");
  header.append("\"Report-Msgid-Bugs-To: "NSH2PO_CONTACT_ADDR"\\n\"\n");
  header.append(QString("\"POT-Creation-Date: %1\\n\"\n").arg(tstamp));
  header.append("\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n");
  header.append("\"Last-Translator: \\n\"\n");
  header.append("\"Language-Team: "NSH2PO_LANGUAGE_TEAM"\\n\"\n");
  header.append("\"MIME-Version: 1.0\\n\"\n");
  header.append(QString("\"Content-Type: text/plain; "
                        "charset=%1\\n\"\n").arg(charset));
  header.append("\"Content-Transfer-Encoding: 8bit\\n\"\n");
  header.append("\"Plural-Forms: nplurals=INTEGER; plural=EXPRESSION;\\n\"\n");
  header.append("\"X-Generator: Vidalia nsh2po "NSH2PO_VERSION"\\n\"\n");
  header.append("\n");

  return header;
}

/** Read and return the next non-empty line from <b>stream</b>. */
QString
read_next_line(QTextStream *stream)
{
  Q_ASSERT(stream);
  stream->skipWhiteSpace();
  return stream->readLine();
}

/** Skip past the header portion of the POT file and any leading whitespace.
 * The next line read from <b>po</b> will be the first non-header line in the
 * document. */
void
skip_pot_header(QTextStream *pot)
{
  QString line;
  /* Skip any leading whitespace before the header */
  pot->skipWhiteSpace();
  /* Read to the first empty line */
  line = pot->readLine();
  while (!pot->atEnd() && !line.isEmpty())
    line = pot->readLine();
}

/** Parse a PO template file for (context,source string) pairs, which are
 * be stored in <b>out</b> using <i>msgctxt</i> as the key and <i>msgid</i>
 * as the value. Return true on success, or false on failure and set
 * <b>errmsg</b>. */
bool
parse_po_template(QTextStream *pot, QHash<QString,QString> *out,
                  QString *errmsg)
{
  QString line, msgctxt, msgid;

  skip_pot_header(pot);
  line = read_next_line(pot);
  while (!pot->atEnd()) {
    if (!line.startsWith("#:") && !line.startsWith("msgctxt")) {
      /* Skip to the start of the next message entry */
      line = read_next_line(pot);
      continue;
    }

    if (line.startsWith("#:")) {
      /* Context was specified with the stupid overloaded "#:" syntax.*/
      msgctxt = line.section(" ", 1);
      msgctxt = parse_message_context_lame(msgctxt);
      line = read_next_line(pot);
      continue;
    }

    if (line.startsWith("msgctxt ")) {
      /* A context specified on a "msgctxt" line takes precedence over a
       * context specified using the overload "#:" notation. */
      msgctxt = line.section(" ", 1);
      msgctxt = parse_message_context(msgctxt);
      line = read_next_line(pot);
    }

    if (!line.startsWith("msgid ")) {
      *errmsg = "expected 'msgid' line";
      return false;
    }
    msgid = line.section(" ", 1);

    line = read_next_line(pot);
    while (line.startsWith("\"")) {
      /* This msgid line had multiple parts to it */
      msgid.append(line);
      line = read_next_line(pot);
    }
    msgid = parse_message_string(msgid);

    out->insert(msgctxt, msgid);
  }

  return true;
}

/** Read an NSIS-formatted file containing LangString entries from <b>nsh</b>.
 * If a LangString entry has a corresponding entry in <b>pot</b>, then the
 * message entry is PO-formatted and appended to <b>po</b>. Return true on
 * success, or false on failure and <b>errmsg</b> will be set. */
int
nsh2po(QTextStream *nsh, const QString &charset,
       const QHash<QString,QString> &pot, QString *po, QString *errmsg)
{
  QString line, msgctxt, msgid, msgstr;
  QStringList parts;
  QHash<QString,QString> langStrings;
  int idx, n_strings;

  *po = create_po_header(charset);

  /* Parse the translated strings from the NSH file */
  while (!nsh->atEnd()) {
    line = read_next_line(nsh);
    if (!line.startsWith("LangString "))
      continue;

    parts = line.split(" ");
    if (parts.size() > 3)
      msgctxt = parts.at(1);
    else
      continue; /* Not properly formatted */

    idx = line.indexOf("\"");
    if (idx > 0)
      msgstr = parse_nsh_langstring(line.mid(idx));
    langStrings.insert(msgctxt, msgstr);
  }

  /* Format the PO file based on the template. */
  n_strings = 0;
  foreach (QString msgctxt, pot.keys()) {
    msgid = pot.value(msgctxt);
    if (langStrings.contains(msgctxt)) {
      msgstr = langStrings.value(msgctxt);
      n_strings++;
    } else {
      msgstr = msgid;
    }

    po->append(QString("msgctxt \"%1\"\n").arg(msgctxt));
    po->append(QString("msgid \"%1\"\n").arg(msgid));
    po->append(QString("msgstr \"%1\"\n").arg(msgstr));
    po->append("\n");
  }
  return n_strings;
}

/** Write <b>po</b> to <b>poFileName</b> using <b>codec</b>. Return true on
 * success. On failure, return false and set <b>errmsg</b> to the reason for
 * failure. */
bool
write_po_output(const char *poFileName, const QString &po, QTextCodec *codec,
                QString *errmsg)
{
  QFile poFile(poFileName);
  if (!poFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    *errmsg = QString("Unable to open '%1' for writing.").arg(poFileName);
    return false;
  }

  QTextStream out(&poFile);
  out.setCodec(codec);
  out << po;
  return true;
}

/** Display application usage and exit. */
void
print_usage_and_exit()
{
  QTextStream error(stderr);
  error << "usage: nsh2po [-q] -t <template.pot> -i <infile.nsh> "
           "-o <outfile.po> [-c <encoding>]\n";
  error << "  -q (optional)       Quiet mode (errors are still displayed)\n";
  error << "  -t <template.pot>   PO template file\n";
  error << "  -i <infile.ts>      Input .ts file\n";
  error << "  -o <outfile.po>     Output .po file\n";
  error << "  -c <encoding>       Text encoding (default: utf-8)\n";
  error.flush();
  exit(1);
}

int
main(int argc, char *argv[])
{
  QTextStream error(stderr);
  QString po, errorMessage;
  char *outFileName;
  QFile potFile, nshFile;
  QTextStream pot, nsh;
  QTextCodec *codec = QTextCodec::codecForName("utf-8");
  bool quiet = false;

  /* Check for the correct number of input parameters. */
  if (argc < 7 || argc > 10)
    print_usage_and_exit();
  for (int i = 1; i < argc; i++) {
    QString arg(argv[i]);
    if (!arg.compare("-q", Qt::CaseInsensitive)) {
      quiet = true;
    } else if (!arg.compare("-t", Qt::CaseInsensitive) && ++i < argc) {
      /* Open the input PO template file */
      potFile.setFileName(argv[i]);
      if (!potFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error << QString("Couldn't open '%1' for reading: ").arg(argv[i])
              <<  potFile.errorString();
        return 1;
      }
      pot.setDevice(&potFile);
    } else if (!arg.compare("-i", Qt::CaseInsensitive) && ++i < argc) {
      /* Open the input NSH file */
      nshFile.setFileName(argv[i]);
      if (!nshFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error << QString("Couldn't open '%1' for reading: ").arg(argv[i])
              <<  nshFile.errorString();
        return 1;
      }
      nsh.setDevice(&nshFile);
    } else if (!arg.compare("-o", Qt::CaseInsensitive) && ++i < argc) {
      outFileName = argv[i];
    } else if (!arg.compare("-c", Qt::CaseInsensitive) && ++i < argc) {
      /* Set the text encoding used for input and output */
      codec = QTextCodec::codecForName(argv[i]);
      if (!codec) {
        error << "Invalid text encoding specified.\n";
        return 1;
      }
    } else
      print_usage_and_exit();
  }
  pot.setCodec(codec);
  nsh.setCodec(codec);

  /* Parse the template for the source strings */
  QHash<QString,QString> poTemplate;
  if (!parse_po_template(&pot, &poTemplate, &errorMessage)) {
    error << QString("Failed to parse PO template: %1\n").arg(errorMessage);
    return 1;
  }

  /* Parse the nsh for the translated strings */
  int n_strings = nsh2po(&nsh, QString(codec->name()), poTemplate,
                         &po, &errorMessage);
  if (n_strings < 0) {
    error << QString("Conversion failed: %1\n").arg(errorMessage);
    return 2;
  }

  /* Write the formatted PO output */
  if (!write_po_output(outFileName, po, codec, &errorMessage)) {
    error << QString("Failed to write PO output: %1\n").arg(errorMessage);
    return 3;
  }

  if (!quiet) {
    QTextStream out(stdout);
    out << QString("Wrote %1 strings to '%2'.\n").arg(n_strings)
                                                 .arg(outFileName);
  }
  return 0;
}

