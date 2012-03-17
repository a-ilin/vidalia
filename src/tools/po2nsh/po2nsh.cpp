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
#include <stdlib.h>


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

  out.replace("\"\n\"", "");
  if (out.startsWith("\""))
    out = out.remove(0, 1);
  if (out.endsWith("\""))
    out.chop(1);
  out.replace("\\\"", "\"");
  out.replace("\\n", "\\r\\n");
  out.replace("$\\r\\n", "$\\r$\\n");
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

/** Convert <b>po</b> from the PO format to a NSIS-formatted .nsh document.
 * <b>nsh</b> will be set to the resulting .nsh document. Return the number of
 * converted strings on success, or -1 on error and <b>errorMessage</b> will
 * be set. */
int
po2nsh(QTextStream *po, QString *nsh, const QString &language,
      QString *errorMessage)
{
  QString line;
  QString msgctxt, msgid, msgstr;
  QHash<QString,QString> header;
  QTextCodec *codec;
  int n_strings = 0;

  Q_ASSERT(po);
  Q_ASSERT(nsh);
  Q_ASSERT(errorMessage);

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

    /* Add the message translation to the .nsh document */
    nsh->append(QString("LangString "));
    nsh->append(msgctxt);
    nsh->append(QString(" ${LANG_%1} ").arg(language));
    if (msgstr.isEmpty())
      nsh->append("\"" + msgid + "\"");
    else
      nsh->append("\"" + msgstr + "\"");
    nsh->append("\n");

    n_strings++;
  }
  return n_strings;
}

/** Display application usage and exit. */
void
print_usage_and_exit()
{
  QTextStream error(stderr);
  error << "usage: po2nsh [-q] -i <infile.po> -o <outfile.nsh> "
           "-l <language> [-f <from-encoding>] [-t <to-encoding>]\n";
  error << "  -q (optional)      Quiet mode (errors are still displayed)\n";
  error << "  -i <infile.po>     Input .po file\n";
  error << "  -o <outfile.nsh>   Output .nsh file\n";
  error << "  -l <language>      NSIS language table name\n";
  error << "  -f <from-encoding> .po file encoding (default: utf-8)\n";
  error << "  -t <to-encoding>   .nsh file encoding (default: iso-8859-1)\n";
  error.flush();
  exit(1);
}

int
main(int argc, char *argv[])
{
  QTextStream error(stderr);
  QString language, errorMessage;
  char *infile, *outfile;
  bool quiet = false;
  QTextCodec *from_codec = QTextCodec::codecForName("utf-8");
  QTextCodec *to_codec   = QTextCodec::codecForName("iso-8859-1");

  /* Check for the correct number of input parameters. */
  if (argc < 7 || argc > 12)
    print_usage_and_exit();
  for (int i = 1; i < argc; i++) {
    QString arg(argv[i]);
    if (!arg.compare("-q", Qt::CaseInsensitive))
      quiet = true;
    else if (!arg.compare("-i", Qt::CaseInsensitive) && ++i < argc)
      infile = argv[i];
    else if (!arg.compare("-o", Qt::CaseInsensitive) && ++i < argc)
      outfile = argv[i];
    else if (!arg.compare("-l", Qt::CaseInsensitive) && ++i < argc)
      language = QString(argv[i]).toUpper();
    else if (!arg.compare("-f", Qt::CaseInsensitive) && ++i < argc) {
      from_codec = QTextCodec::codecForName(argv[i]);
      if (!from_codec) {
        error << "Invalid input encoding: " << argv[i] << "\n";
        return 1;
      }
    } else if (!arg.compare("-t", Qt::CaseInsensitive) && ++i < argc) {
      to_codec = QTextCodec::codecForName(argv[i]);
      if (!to_codec) {
        error << "Invalid output encoding: " << argv[i] << "\n";
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

  QString nsh;
  QTextStream po(&poFile);
  po.setCodec(from_codec);
  int n_strings = po2nsh(&po, &nsh, language, &errorMessage);
  if (n_strings < 0) {
    error << QString("Unable to convert '%1': %2\n").arg(infile)
                                                    .arg(errorMessage);
    return 3;
  }

  /* Open the NSH file for writing. */
  QFile nshFile(outfile);
  if (!nshFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    error << QString("Unable to open '%1' for writing: %2\n").arg(outfile)
                                                .arg(nshFile.errorString());
    return 4;
  }

  /* Finally write the .nsh output. */
  QTextStream out(&nshFile);
  out.setCodec(to_codec);
  out << nsh;

  if (!quiet) {
    QTextStream results(stdout);
    results << QString("Converted %1 strings from %2 to %3.\n").arg(n_strings)
                                                               .arg(infile)
                                                               .arg(outfile);
  }
  return 0;
}

