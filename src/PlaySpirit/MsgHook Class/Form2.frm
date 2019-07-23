VERSION 5.00
Begin VB.Form Form2 
   Caption         =   "Form2"
   ClientHeight    =   3150
   ClientLeft      =   60
   ClientTop       =   390
   ClientWidth     =   4680
   LinkTopic       =   "Form2"
   ScaleHeight     =   3150
   ScaleWidth      =   4680
   StartUpPosition =   3  '´°¿ÚÈ±Ê¡
   Begin VB.TextBox Text1 
      Height          =   1335
      Left            =   0
      TabIndex        =   0
      Text            =   "Text1"
      Top             =   0
      Width           =   2655
   End
End
Attribute VB_Name = "Form2"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim WithEvents t As CMsgHook
Attribute t.VB_VarHelpID = -1
Private Sub Form_Load()
    Set t = New CMsgHook
    
    't.GetChildWindow Hwnd
    text1Hwnd& = t.GetChildWindow(Hwnd, "ThunderTextBox")

    If text1Hwnd <> 0 Then t.SetHook text1Hwnd
End Sub

Private Sub Form_Unload(Cancel As Integer)
    t.SetHook 0
End Sub

Private Sub t_EnumChildevent(ByVal Hwnd As Long, ByVal ClassName As String)
    If LCase(ClassName) = LCase("thundertextbox") Then text1Hwnd = Hwnd
End Sub

Private Sub t_MsgEvent(ByVal Hwnd As Long, ByVal Msg As Long, ByVal Arg1 As Long, ByVal Arg2 As Long, ByVal Arg3 As Long)
    If Msg = t.WM_RBUTTONDOWN Then
        t.Enabled = 0
    End If
End Sub
