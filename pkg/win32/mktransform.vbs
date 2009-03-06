' Copyright (C) 2008-2009  The Tor Project, Inc.
' See LICENSE file for rights and terms.
'
' This script automates the task of creating transforms from one MSI database
' to another. This is required for language based transforms, for example.
'
' To apply vidalia_fr.msi localization to vidalia.msi with name 'fr':
'   cscript.exe mktransform.vbs fr vidalia.msi vidalia_fr.msi
'
Const msiOpenDatabaseModeReadOnly = 0
Const msiOpenDatabaseModeTransact = 1
Const msiViewModifyAssign = 3
Const msiTransformErrorNone = 0
Const msiTransformValidationNone = 0

Dim message
If Wscript.Arguments.Count < 3 Then
  message = "Usage: mktransform <transform name> <minimal msi> <target msi>"
  Wscript.Echo message
  Wscript.Quit 1
End If

Dim transformName
Dim minMsiFile
Dim currMsiFile
transformName = Wscript.Arguments.Item(0)
minMsiFile = Wscript.Arguments.Item(1)
currMsiFile = Wscript.Arguments.Item(2)

CreateTransform transformName, minMsiFile, currMsiFile
Wscript.Quit 0

Sub CreateTransform (TransformName, MinMsiFile, CurrMsiFile)
  ' Create transform from minimal to current/target MSI database
  Set installer = Wscript.CreateObject("WindowsInstaller.Installer") : CheckError
  Dim database1 : Set database1 = installer.OpenDatabase(MinMsiFile, msiOpenDatabaseModeTransact) : CheckError
  Dim database2 : Set database2 = installer.OpenDatabase(CurrMsiFile, msiOpenDatabaseModeReadOnly) : CheckError
  Dim transform : transform = TransformName & ".mst"
  database2.GenerateTransform database1, transform : CheckError
  database2.CreateTransformSummaryInfo database1, transform, msiTransformErrorNone, msiTransformValidationNone : CheckError

  ' Embed Transform into the database with the requested name
  Set view = database1.OpenView("SELECT `Name`,`Data` FROM _Storages") : CheckError
  view.Execute : CheckError
  Set record = installer.CreateRecord(2)
  record.StringData(1) = TransformName
  record.SetStream 2, transform : CheckError
  view.Modify msiViewModifyAssign, record : CheckError
  database1.Commit : CheckError
  Set database1 = Nothing
  Set database2 = Nothing
End Sub

Sub CheckError
  If Err = 0 Then Exit Sub
  Dim message, errRec
  message = Err.Source & " " & Hex(Err) & ": " & Err.Description
  If Not installer Is Nothing Then
    Set errRec = installer.LastErrorRecord
    If Not errRec Is Nothing Then
      message = message & vbNewLine & errRec.FormatText
    End If
  End If
  Wscript.Echo message
  Wscript.Quit 1
End Sub

