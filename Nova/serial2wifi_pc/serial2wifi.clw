; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CPCWIFIDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "serial2wifi.h"

ClassCount=3
Class1=CPCWIFIApp
Class2=CPCWIFIDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_PCWIFI_DIALOG

[CLS:CPCWIFIApp]
Type=0
HeaderFile=serial2wifi.h
ImplementationFile=serial2wifi.cpp
Filter=N

[CLS:CPCWIFIDlg]
Type=0
HeaderFile=serial2wifiDlg.h
ImplementationFile=serial2wifiDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_COMBO1

[CLS:CAboutDlg]
Type=0
HeaderFile=serial2wifiDlg.h
ImplementationFile=serial2wifiDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_PCWIFI_DIALOG]
Type=1
Class=CPCWIFIDlg
ControlCount=15
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_COMBO1,combobox,1344340227
Control4=IDC_COMBO2,combobox,1344340227
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_IPADDRESS1,SysIPAddress32,1342242816
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT2,edit,1350639744
Control11=IDC_EDIT3,edit,1352732740
Control12=IDC_EDIT4,edit,1352732676
Control13=IDC_STATIC1,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_BUTTON1,button,1342242816

