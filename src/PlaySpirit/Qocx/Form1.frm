VERSION 5.00
Object = "{3090BD0E-BFD9-4DB3-A049-CD4274B533D3}#2.0#0"; "Qocx.ocx"
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3150
   ClientLeft      =   60
   ClientTop       =   390
   ClientWidth     =   6675
   LinkTopic       =   "Form1"
   ScaleHeight     =   3150
   ScaleWidth      =   6675
   StartUpPosition =   3  '窗口缺省
   Begin QocxPrj.QScroll PlayProgram 
      Height          =   135
      Left            =   1800
      TabIndex        =   0
      Top             =   1680
      Width           =   3015
      _ExtentX        =   5318
      _ExtentY        =   238
      Value           =   0
      Min             =   0
      Max             =   100
      Enable          =   -1  'True
      Scrollable      =   -1  'True
      AutoSize        =   -1  'True
      xMargin         =   345
      yMargin         =   15
      BackGroundPicture=   "Form1.frx":0000
      ForeGroundPicture=   "Form1.frx":1590
      ScrollBackGroundPicture=   "Form1.frx":1A0C
      ScrollMouseMovePicture=   "Form1.frx":1C34
      ScrollMouseDownPicture=   "Form1.frx":1E5C
   End
   Begin QocxPrj.QLabel QLabel1 
      Height          =   735
      Left            =   600
      TabIndex        =   1
      Top             =   720
      Width           =   2655
      _ExtentX        =   4683
      _ExtentY        =   1296
      Text            =   "14151"
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Form_Click()
    Picture1.Picture = LoadPicture("D:\photo\picture\风景\57972279.jpg")
    QLabel1.Refresh
End Sub

Private Sub Form_Load()
    Picture = LoadPicture("D:\photo\picture\风景\1520239.jpg")
    QLabel1.Refresh
End Sub
