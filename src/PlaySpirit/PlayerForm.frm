VERSION 5.00
Object = "{5031481D-CB67-4109-9750-C90DD6C48F4C}#3.0#0"; "QPlayer.ocx"
Begin VB.Form PlayerForm 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   0  'None
   Caption         =   "PlaySpirit"
   ClientHeight    =   660
   ClientLeft      =   5325
   ClientTop       =   4815
   ClientWidth     =   3600
   ControlBox      =   0   'False
   FillColor       =   &H00FFFFFF&
   ForeColor       =   &H00FFFFFF&
   Icon            =   "PlayerForm.frx":0000
   LinkTopic       =   "Form1"
   MinButton       =   0   'False
   ScaleHeight     =   660
   ScaleWidth      =   3600
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin QPlayerOcxPrj.PlayerCtr Player 
      Height          =   735
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   3600
      _ExtentX        =   6350
      _ExtentY        =   1296
   End
End
Attribute VB_Name = "PlayerForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Public m_Shadow As New CShadow

Private Sub Form_Load()
    Me.Left = (Screen.Width - Me.Width) / 2
    Me.Top = (Screen.Height - Me.Height) / 2
End Sub

Private Sub Form_Unload(Cancel As Integer)
    m_Shadow.UnsetShadow
    Set m_Shadow = Nothing
End Sub
