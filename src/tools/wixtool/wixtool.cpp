/*
**  $Id$
**
**  Copyright (C) 2009  The Tor Project, Inc.
**  See LICENSE file for terms; may be used according
**  Vidalia or Tor license constraints. (dual license)
*/

#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QTextCodec>
#include <QStringList>
#include <stdlib.h>

#define WIX_ATTR_ID  "Id"
#define WIX_ATTR_DIRACTION "uninstall"
#define WIX_ATTR_REGACTION "createAndRemoveOnUninstall"
#define WIX_ATTR_VALUE "Value"
#define WIX_ATTR_KEY "KeyPath"
#define WIX_ATTR_GUID "Guid"
#define WIX_ATTR_NAME "Name"
#define WIX_ATTR_REG_TYPE "Type"
#define WIX_ATTR_REG_NAME "Name"
#define WIX_ATTR_REG_ROOT "Root"
#define WIX_ATTR_REG_KEYPATH  "Key"
#define WIX_ATTR_REG_ACTION  "Action"
#define WIX_REG_KEY_TYPE "integer"
#define WIX_TAG_FILE "File"
#define WIX_TAG_DIR "Directory"
#define WIX_TAG_FEATURE "Feature"
#define WIX_TAG_COMPONENT "Component"
#define WIX_TAG_COMPONENT_REF "ComponentRef"
#define WIX_TAG_CREATEDIR "CreateFolder"
#define WIX_TAG_REMOVEDIR "RemoveFolder"
#define WIX_TAG_REGKEY "RegistryKey"
#define WIX_TAG_REGVAL "RegistryValue"

typedef void (*TraverseCallback)(void *cbdata, QDomElement e);

/* Splice command takes an element or sub tree from one
 * document and inserts it into another. This is useful for
 * expanding placeholder elements with their desired content
 * for example.
 * If an element name is not unique the conventional WiX Id
 * attribute can be used to identify the specific element.
 */
typedef struct s_SpliceData {
  QString      dtag;
  QString      did;
  QDomElement  splice;
} SpliceData;

/* Replace operates on tags by name or Id like Splice but
 * only makes modifications to individual elements. Replace
 * can also remove elements. (replace with null)
 */
typedef struct s_ReplaceData {
  QString  dtag;
  QString  did;
  QString  dprop;
  QString  newtag;
  QString  newprop;
  QString  newpropval;
} ReplaceData;

/* Add operates on tags by name or Id as usual.
 */
typedef struct s_AddData {
  QString  dtag;
  QString  did;
  QString  newtag;
  QString  newprop;
  QString  newpropval;
} AddData;

/* In order to support local per user installation some basic
 * constrains must apply to every component included in a
 * package. This includes using a key path for each component
 * via registry keys and placing all application data under the
 * local user profile folder.
 * This utility will navigate the components and convert any
 * keys to registry key paths and create folders in the deployment
 * hierarchy as required.
 */
typedef struct s_UserLocalData {
  QString      keypath;
  QString      featureid;
  QStringList  newcomps;
} UserLocalData;


/* Note that we must walk the tree ourselves as locate by ID
 * nor suitable select by classification is available in the
 * Qt API.
 */
bool
do_walkdoc(QDomNode  n,
  TraverseCallback  cb,
  void *            cbdata,
  QString *errorMessage)
{
  QTextStream error(stderr);
  if ( !n.isNull() ) {
    if ( n.isElement() ) {
      QDomElement e = n.toElement();
      (*cb)(cbdata, e);
    }
    if ( n.hasChildNodes() ) {
      QDomNodeList subnodes = n.childNodes();
      int i = 0;
      while (i < subnodes.count()) {
        do_walkdoc(subnodes.item(i++), cb, cbdata, errorMessage);
      }
    }
  }
  return true;
}

bool
walkdoc(QDomDocument *doc,
  TraverseCallback  cb,
  void *            cbdata,
  QString *errorMessage)
{
  QTextStream error(stderr);
  QDomNode n = doc->documentElement();
  do_walkdoc(n, cb, cbdata, errorMessage);
  return true;
}

void
splicefunc(void *cbdata,
 QDomElement e)
{
  SpliceData *d = reinterpret_cast<SpliceData *>(cbdata);
  QString eid = e.attribute(WIX_ATTR_ID);

  if (e.tagName().compare(d->dtag) == 0) {
    /* if a specific Id is set, verify it too. */
    if (d->did.isEmpty() ||
        (eid.size() && !eid.compare(d->did)) ) {

      /* expected behavior is to graft children of the splice under target.
       * if we're only given a single element graft it instead.
       */
      if (d->splice.hasChildNodes()) {
        QDomNodeList subnodes = d->splice.childNodes();
        int i = 0;
        while (i < subnodes.count()) {
          e.appendChild(e.ownerDocument().importNode(subnodes.item(i++), true));
        }
      }
      else {
        e.appendChild(e.ownerDocument().importNode(d->splice, true));
      }
    }
  }
}

/** Make modifications to requested documents.
 * returns false on error and <b>errorMessage</b> will be set.
 */
bool
docsplice(QDomDocument *doc,
  QString arguments,
  QString *errorMessage)
{
  Q_ASSERT(doc);
  Q_ASSERT(errorMessage);
  SpliceData  cbdata;

  QStringList spliceinfo = arguments.split("=");
  if (spliceinfo.count() != 2) {
    *errorMessage = "Invalid argument for splice command: " + arguments;
    return false;
  }
  if (spliceinfo[0].contains(':')) {
    /* Id syntax */
    QStringList destinfo = spliceinfo[0].split(":");
    cbdata.dtag = destinfo[0];
    cbdata.did  = destinfo[1];
  }
  else {
    cbdata.dtag = spliceinfo[0];
  }

  QStringList  srcinfo = spliceinfo[1].split(":");
  if (srcinfo.count() < 2) {
    *errorMessage = "Invalid source argument for splice command: " + arguments;
    return false;
  }
  QFile spliceFile(srcinfo[0]);
  if (!spliceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    *errorMessage = QString("Unable to open '%1' for reading: %2\n")
                     .arg(srcinfo[0]).arg(spliceFile.errorString());
    return false;
  }
  QTextStream sfiletxt(&spliceFile);
  QDomDocument sdoc;
  QString parseError;
  int  badline, badcol;
  if (!sdoc.setContent (sfiletxt.readAll(), false, &parseError, &badline, &badcol)) {
    *errorMessage = QString("Error parsing splice document '%1' at line %2 and column %3: %4")
                    .arg(srcinfo[0]).arg(badline).arg(badcol).arg(parseError);
    return false;
  }

  QDomNodeList elist = sdoc.elementsByTagName(srcinfo[1]);
  if (elist.count() == 0) {
    *errorMessage = QString("Unable to locate splice element '%1' in document.\n").arg(srcinfo[1]);
    return false;
  }
  if (srcinfo.count() == 3) {
    /* Id syntax for source elem */
    for (int i=0; i < elist.count(); i++) {
      QString eid = elist.item(i).toElement().attribute(WIX_ATTR_ID);
      if (eid.size() && !eid.compare(srcinfo[2])) {
        cbdata.splice = elist.item(i).toElement();
      }
    }
  }
  else {
    /* without an Id the tag name should be unique. */
    cbdata.splice = elist.item(0).toElement();
  }
  return walkdoc(doc, &splicefunc, &cbdata, errorMessage);
}

void
replacefunc(void *cbdata,
 QDomElement e)
{
  ReplaceData *d = reinterpret_cast<ReplaceData *>(cbdata);
  QString eid = e.attribute(WIX_ATTR_ID);

  if (e.tagName().compare(d->dtag) == 0) {
    /* if a specific Id is set, verify it too. */
    if (d->did.isEmpty() ||
        (eid.size() && !eid.compare(d->did)) ) {

      /* no destination means remove node from tree */
      if (d->newtag.isNull()) {
        QDomNode parent = e.parentNode();
        parent.removeChild(e);
      }
      else {
        if (d->newtag.compare(e.tagName())) {
          e.setTagName (d->newtag);
        }
        if (d->newprop.isNull()) {
          /* clear all attributes (except Id if present) */
          QDomNamedNodeMap attrs = e.attributes();
          for (int i = 0; i < attrs.count(); i++) {
            if (attrs.item(i).nodeName().compare(WIX_ATTR_ID)) {
              e.removeAttribute(attrs.item(i).nodeName());
            }
          }
        }
        else {
          /* only modify / clear a specific property */
          QDomNode prop = e.attributeNode(d->newprop);
          if (!prop.isNull()) {
            e.setAttribute(d->newprop, d->newpropval);
          }
        }
      }
    }
  }
}

/** Make modifications to requested documents.
 * returns false on error and <b>errorMessage</b> will be set.
 */
bool
docreplace(QDomDocument *doc,
  QString arguments,
  QString *errorMessage)
{
  Q_ASSERT(doc);
  Q_ASSERT(errorMessage);
  ReplaceData  cbdata;

  QStringList replaceinfo = arguments.split("=");
  if (replaceinfo.count() < 1) {
    *errorMessage = "Invalid argument for replace command: " + arguments;
    return false;
  }
  if (replaceinfo[0].contains(':')) {
    /* Id syntax */
    QStringList destinfo = replaceinfo[0].split(":");
    cbdata.dtag = destinfo[0];
    cbdata.did  = destinfo[1];
    if (destinfo.count() >= 3) {
      cbdata.dprop = destinfo[2];
    }
  }
  else {
    cbdata.dtag = replaceinfo[0];
  }
  if (replaceinfo.count() > 1) {
    QStringList  srcinfo = replaceinfo[1].split(":");
    if (srcinfo.count() < 1) {
      *errorMessage = "Invalid target argument for replace command: " + arguments;
      return false;
    }
    if (srcinfo.count() >= 1) {
      if (srcinfo[0].length()) cbdata.newtag = srcinfo[0];
    }
    if (srcinfo.count() >= 2) {
      if (srcinfo[1].length()) cbdata.newprop = srcinfo[1];
    }
    if (srcinfo.count() >= 3) {
      if (srcinfo[2].length()) cbdata.newpropval = srcinfo[2];
    }
  }
  return walkdoc(doc, &replacefunc, &cbdata, errorMessage);
}

void
addfunc(void *cbdata,
 QDomElement e)
{
  AddData *d = reinterpret_cast<AddData *>(cbdata);
  QString eid = e.attribute(WIX_ATTR_ID);

  if (e.tagName().compare(d->dtag) == 0) {
    /* if a specific Id is set, verify it too. */
    if (d->did.isEmpty() ||
        (eid.size() && !eid.compare(d->did)) ) {
      if (d->newtag.compare(d->dtag)) {
        QDomElement ne = e.ownerDocument().createElement(d->newtag);
        if (!d->newprop.isNull()) {
          ne.setAttribute(d->newprop, d->newpropval);
        }
        e.appendChild(ne);
      }
      else {
        e.setAttribute(d->newprop, d->newpropval);
      }
    }
  }
}

/** Make modifications to requested documents.
 * returns false on error and <b>errorMessage</b> will be set.
 */
bool
docadd(QDomDocument *doc,
  QString arguments,
  QString *errorMessage)
{
  Q_ASSERT(doc);
  Q_ASSERT(errorMessage);
  AddData  cbdata;

  QStringList addinfo = arguments.split("=");
  if (addinfo.count() < 1) {
    *errorMessage = "Invalid argument for add command: " + arguments;
    return false;
  }
  if (addinfo[0].contains(':')) {
    /* Id syntax */
    QStringList destinfo = addinfo[0].split(":");
    cbdata.dtag = destinfo[0];
    cbdata.did  = destinfo[1];
  }
  else {
    cbdata.dtag = addinfo[0];
  }
  if (addinfo.count() > 1) {
    QStringList  srcinfo = addinfo[1].split(":");
    if (srcinfo.count() < 1) {
      *errorMessage = "Invalid target argument for add command: " + arguments;
      return false;
    }
    if (srcinfo.count() >= 1) {
      if (srcinfo[0].length()) cbdata.newtag = srcinfo[0];
    }
    if (srcinfo.count() >= 2) {
      if (srcinfo[1].length()) cbdata.newprop = srcinfo[1];
    }
    if (srcinfo.count() >= 3) {
      if (srcinfo[2].length()) cbdata.newpropval = srcinfo[2];
    }
  }
  return walkdoc(doc, &addfunc, &cbdata, errorMessage);
}

void
createRegLocalComponent(QDomElement e,
 QString dirName,
 QString keyPath)
{
  QDomElement nrk = e.ownerDocument().createElement(WIX_TAG_REGKEY);
  QDomElement nrv = e.ownerDocument().createElement(WIX_TAG_REGVAL);
  nrk.setAttribute(WIX_ATTR_REG_ROOT, "HKCU");
  nrk.setAttribute(WIX_ATTR_REG_ACTION, "createAndRemoveOnUninstall");
  nrk.setAttribute(WIX_ATTR_REG_KEYPATH, keyPath);
  nrv.setAttribute(WIX_ATTR_REG_TYPE, WIX_REG_KEY_TYPE);
  nrv.setAttribute(WIX_ATTR_REG_NAME, dirName);
  nrv.setAttribute(WIX_ATTR_VALUE, "1");
  nrv.setAttribute(WIX_ATTR_KEY, "yes");
  nrk.appendChild(nrv);
  e.appendChild(nrk);
}

void
createDirMgmtComponent(QDomElement e,
 QString dirName)
{
  QDomElement nce;
  /* An empty dir might produce a createdir, so only add if not present. */
  if (e.elementsByTagName(WIX_TAG_CREATEDIR).count() == 0) {
    nce = e.ownerDocument().createElement(WIX_TAG_CREATEDIR);
    e.appendChild(nce);
  }
  nce = e.ownerDocument().createElement(WIX_TAG_REMOVEDIR);
  nce.setAttribute("On", WIX_ATTR_DIRACTION);
  nce.setAttribute(WIX_ATTR_ID, QString("Remove").append(dirName));
  e.appendChild(nce);
}

void
userlocalfunc(void *cbdata,
 QDomElement e)
{
  UserLocalData *ulinfo = reinterpret_cast<UserLocalData *>(cbdata);
  QString eid = e.attribute(WIX_ATTR_ID);

  if (e.tagName().compare(WIX_TAG_FILE) == 0) {
    e.removeAttribute(WIX_ATTR_KEY);
  }
  else if (e.tagName().compare(WIX_TAG_COMPONENT) == 0) {
    /* If the WiX tools get confused we need to remove KeyPath attrs
     * on any component elements after creation or merging.
     * Empty directories with a CreateFolder and nothing else will do this.
     */
    e.removeAttribute(WIX_ATTR_KEY);
  }
  else if (e.tagName().compare(WIX_TAG_FEATURE) == 0) {
    /* be sure to remove any default feature names; changed added above. */
    QDomNodeList cnl = e.elementsByTagName(WIX_TAG_COMPONENT_REF);
    for (int i = 0; i < cnl.count(); i++) {
      QDomElement cre = cnl.item(i).toElement();
      if (cre.attribute(WIX_ATTR_ID).compare(WIX_TAG_COMPONENT) == 0) {
        e.removeChild(cre);
      }
    }
    if (ulinfo->featureid.compare(e.attribute(WIX_ATTR_ID)) == 0) {
      /* this is the target feature element for the new components, if any. */
      QDomElement ne;
      for (int i = 0; i < ulinfo->newcomps.count(); i++) {
        QString currid = ulinfo->newcomps[i];
        ne = e.ownerDocument().createElement(WIX_TAG_COMPONENT_REF);
        ne.setAttribute(WIX_ATTR_ID, currid);
        e.appendChild(ne);
      }
    }
  }
  else if (e.tagName().compare(WIX_TAG_DIR) == 0) {
    QString dirName = e.attribute(WIX_ATTR_NAME);
    QString dirId = e.attribute(WIX_ATTR_ID);
    /* find all child components for this dir and see if it contains:
     * create/remove folder elements, a registry element
     */
    if ( e.hasChildNodes() ) {
      QDomElement fc;
      bool  hasComponent = false;
      bool  hasRegKey;
      QDomNodeList subnodes = e.childNodes();
      for (int i = 0; i < subnodes.count(); i++) {
        hasRegKey = false;
        if (subnodes.item(i).isElement()) {
          QDomElement ce = subnodes.item(i).toElement();
          if (ce.tagName().compare(WIX_TAG_COMPONENT) == 0) {
            if (!hasComponent) {
              hasComponent = true;
              fc = ce;
              if (ce.attribute(WIX_ATTR_ID).compare(WIX_TAG_COMPONENT) == 0) {
                /* Fix default named components before adding registry elements. */
                ce.setAttribute(WIX_ATTR_ID, QString("DCOMP").append(dirName));
                ulinfo->newcomps.append(ce.attribute(WIX_ATTR_ID));
              }
              if (ce.elementsByTagName(WIX_TAG_REMOVEDIR).count() == 0) {
                createDirMgmtComponent(ce, ce.attribute(WIX_ATTR_ID));
              }
            }
            QDomNodeList compnodes = ce.childNodes();
            for (int j = 0; j < compnodes.count(); j++) {
              if (compnodes.item(j).isElement()) {
                QDomElement compe = compnodes.item(j).toElement();
                if (compe.tagName().compare(WIX_TAG_REGKEY) == 0) {
                  hasRegKey = true;
                }
              }
            }
            if (!hasRegKey) {
              createRegLocalComponent(ce, QString("RK").append(ce.attribute(WIX_ATTR_ID)), ulinfo->keypath);
            }
          }
        }
      }
      if (!hasComponent) {
        /* Certain system directories must be ignored; we don't manage them. */
        if (dirId.compare("LocalAppDataFolder") &&
            dirId.compare("AppDataFolder") &&
            dirId.compare("CommonAppDataFolder") &&
            dirId.compare("CommonFilesFolder") &&
            dirId.compare("DesktopFolder") &&
            dirId.compare("PersonalFolder") &&
            dirId.compare("ProgramFilesFolder") &&
            dirId.compare("ProgramMenuFolder") &&
            dirId.compare("StartMenuFolder") &&
            dirId.compare("StartupFolder") &&
            dirId.compare("SystemFolder") &&
            dirId.compare("TempFolder") &&
            dirId.compare("WindowsFolder") ) {
          /* if there is no component under this dir parent then we
           * must create a component for the sole purpose of dir
           * creation with the requisite registry key path.
           */
          QDomElement ne = e.ownerDocument().createElement(WIX_TAG_COMPONENT);
          QString compId = QString("ULDirComp_").append(dirName);
          ne.setAttribute(WIX_ATTR_GUID, "*");
          ne.setAttribute(WIX_ATTR_ID, compId);
          e.appendChild(ne);
          createDirMgmtComponent(ne, dirName);
          createRegLocalComponent(ne, QString("DRK").append(dirName), ulinfo->keypath);
          ulinfo->newcomps.append(compId);
        }
      }
    }
  }
}

/** Make modifications to requested documents.
 * returns false on error and <b>errorMessage</b> will be set.
 */
bool
docuserlocal(QDomDocument *doc,
  QString argument,
  QString *errorMessage)
{
  Q_ASSERT(doc);
  Q_ASSERT(errorMessage);
  UserLocalData  cbdata;

  QStringList ulinfo = argument.split(":");
  if (ulinfo.count() < 2) {
    *errorMessage = "Invalid argument for userlocal command: " + argument;
    return false;
  }
  cbdata.keypath = ulinfo[0];
  cbdata.featureid = ulinfo[1];
  return walkdoc(doc, &userlocalfunc, &cbdata, errorMessage);
}

/** Display application usage and exit. */
void
print_usage_and_exit()
{
  QTextStream error(stderr);
  error << "usage: wixtool <command> [-q] -i <infile> -o <outfile> <Arg0> [... <ArgN>]" << endl;
  error << "  command one of: " << endl;
  error << "    splice        Splice children from one document into another." << endl;
  error << "    replace       Replace elements or attributes in a document." << endl;
  error << "    add           Add elements or attributes into a document." << endl;
  error << "    userlocal     Convert File elements into per-user local elements." << endl;
  error << "  -i <infile>     Input or template file" << endl;
  error << "  -o <outfile>    Output file" << endl;
  error << endl;
  error << "  splice args:    desttagname[:Id]=file:basetag[:Id]" << endl;
  error << "    Splice children of basetag in file under desttagname" << endl;
  error << endl;
  error << "  replace args:   tagname[:Id]:property=newtagname[:Id]:property:value" << endl;
  error << "    If newtagname is empty the element is deleted" << endl;
  error << "    If newproperty is empty the property is deleted" << endl;
  error << endl;
  error << "  add args:       desttagname[:Id]=newtagname[:Id]:property:value" << endl;
  error << "    Add properties or child elements to target" << endl;
  error << "    If newtagname is empty only properties added to dest" << endl;
  error << endl;
  error << "  userlocal arg:  <registry key path>:<dest feature id>" << endl;
  error << "    Convert KeyPath File elements into the per user local idiom" << endl;
  error << "    with corresponding Create/RemoveDir and RegistryKey elements." << endl;
  error << endl;
  error << " NOTE: text content within an element is not accessible." << endl;
  error << "       Use the Value= attribute syntax if necessary." << endl;
  error << "       The optional :Id syntax restricts matching to elements with" << endl;
  error << "       the Id attribute set to the value indicated." << endl;
  error.flush();
  exit(1);
}

int
main(int argc, char *argv[])
{
  QTextStream error(stderr);
  QString command, errorMessage;
  char *infile = 0, *outfile = 0;
  QTextCodec *codec = QTextCodec::codecForName("utf-8");
  bool quiet = false;
  QStringList commandargs;

  /* Check for the correct number of input parameters. */
  if (argc < 6)
    print_usage_and_exit();

  /* Verify command is supported. */
  command = argv[1];
  if ( command.compare("splice", Qt::CaseInsensitive) &&
       command.compare("replace", Qt::CaseInsensitive) &&
       command.compare("add", Qt::CaseInsensitive) &&
       command.compare("userlocal", Qt::CaseInsensitive) ) {
    print_usage_and_exit();
  }

  /* Gather remaining arguments. */
  for (int i = 2; i < argc; i++) {
    QString arg(argv[i]);
    if (!arg.compare("-q", Qt::CaseInsensitive))
      quiet = true;
    else if (!arg.compare("-i", Qt::CaseInsensitive) && ++i < argc)
      infile = argv[i];
    else if (!arg.compare("-o", Qt::CaseInsensitive) && ++i < argc)
      outfile = argv[i];
    else if (infile && outfile) {
      commandargs.append(arg);
    }
  }
  if ( !infile || !outfile || !commandargs.count() ) {
    print_usage_and_exit();
  }

  /* Open the source document for reading. */
  QFile srcFile(infile);
  QTextStream sfiletxt(&srcFile);
  sfiletxt.setCodec(codec);
  if (!srcFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    error << QString("Unable to open '%1' for reading: %2\n").arg(infile)
                                                .arg(srcFile.errorString());
    return 2;
  }

  /* Make sure the outfile does not exist before we use it. */
  if (QFile::exists(outfile)) {
    if (!QFile::remove(outfile)) {
      error << QString("Unable to truncate outfile '%1'\n").arg(outfile);
      return 2;
    }
  }

  QDomDocument doc;
  QString parseError;
  int  badline, badcol;
  if (!doc.setContent (sfiletxt.readAll(), false, &parseError, &badline, &badcol)) {
    error << QString("Error parsing source document '%1' at line %2 and column %3: %4")
                    .arg(infile).arg(badline).arg(badcol).arg(parseError);
    return 3;
  }

  if (!command.compare("userlocal", Qt::CaseInsensitive)) {
    if (!docuserlocal(&doc, commandargs[0], &errorMessage)) {
      error << QString("Unable to convert document components to user local: %1\n")
                          .arg(errorMessage);
      return 4;
    }
  }
  else {
    for (int i = 0; i < commandargs.count(); i++) {
      if (!command.compare("splice", Qt::CaseInsensitive)) {
        if (!docsplice(&doc, commandargs[i], &errorMessage)) {
          error << QString("Unable to process splice command '%1': %2\n")
                          .arg(commandargs[i]).arg(errorMessage);
          return 4;
        }
      }
      else if (!command.compare("replace", Qt::CaseInsensitive)) {
        if (!docreplace(&doc, commandargs[i], &errorMessage)) {
          error << QString("Unable to process replace command '%1': %2\n")
                          .arg(commandargs[i]).arg(errorMessage);
          return 4;
        }
      }
      else if (!command.compare("add", Qt::CaseInsensitive)) {
        if (!docadd(&doc, commandargs[i], &errorMessage)) {
          error << QString("Unable to process add command '%1': %2\n")
                          .arg(commandargs[i]).arg(errorMessage);
          return 4;
        }
      }
    }
  }

  /* Open the output file for writing. */
  QFile docFile(outfile);
  if (!docFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    error << QString("Unable to open '%1' for writing: %2\n").arg(outfile)
                                                .arg(docFile.errorString());
    return 5;
  }

  /* Write the .wxl output. */
  QTextStream out(&docFile);
  out << doc.toString(4);

  return 0;
}

