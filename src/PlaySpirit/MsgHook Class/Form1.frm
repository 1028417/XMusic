VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3150
   ClientLeft      =   165
   ClientTop       =   495
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3150
   ScaleWidth      =   4680
   StartUpPosition =   3  '´°¿ÚÈ±Ê¡
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim WithEvents t As CMsgHook
Attribute t.VB_VarHelpID = -1
Private Sub Form_Load()
    Set t = New CMsgHook
    
    t.SetHook Hwnd
    t.SetHotkey vbKeyQ, t.MOD_CONTROL, 1860
    t.SetClipBoardHook
    
    Show
    Form2.Show
End Sub

Private Sub Form_Unload(Cancel As Integer)
    t.SetHook 0
End Sub

Private Sub Form_Resize()
    If Me.WindowState = 1 Then
        Me.Hide
        Me.WindowState = 0
    End If
End Sub

Private Sub t_MsgEvent(ByVal Hwnd As Long, ByVal Msg As Long, ByVal Arg1 As Long, ByVal Arg2 As Long, ByVal Arg3 As Long)
    Select Case Msg
        Case t.WM_SIZE, t.WM_MOVE
            Me.Caption = Arg1 & " " & Arg2
            
        Case t.WM_SYSCOMMAND
            If Arg1 = t.SC_CLOSE Then t.Enabled = 1
            If Arg1 = &HF090& Then MsgBox ""
            
        Case t.WM_HOTKEY
            Me.Show
        
        Case t.WM_DRAWCLIPBOARD
            MsgBox "clipbaord chaged"
        
        Case t.WM_GETMINMAXINFO
            t.MinTrackSizeX = 200
            t.MinTrackSizeY = 150
            t.MaxPositionX = -10
            t.MaxPositionY = -10
            t.MaxSizeX = Screen.Width / 15 - 1
            t.MaxSizeY = Screen.Height / 15 - 1
    End Select
End Sub
