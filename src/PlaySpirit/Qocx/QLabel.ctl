VERSION 5.00
Begin VB.UserControl QLabel 
   AutoRedraw      =   -1  'True
   BackColor       =   &H8000000C&
   ClientHeight    =   300
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   3000
   ForeColor       =   &H8000000E&
   ScaleHeight     =   300
   ScaleWidth      =   3000
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   100
      Left            =   1200
      Top             =   0
   End
End
Attribute VB_Name = "QLabel"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Public Event MouseDown(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)
Public Event MouseUp(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)
Public Event MouseMove(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)
Public Event DblClick()

Dim m_Text As String

Dim m_CurrentX As Long
Dim m_CurrentY As Long

Dim m_TextWidth As Long

Private Sub Timer1_Timer()
    Static flag As Integer
    If flag = 0 Then
        flag = -1
    End If

    If Text <> "" Then
        UserControl.Cls
        
        m_CurrentX = m_CurrentX + flag * 30

        UserControl.CurrentX = m_CurrentX
        UserControl.CurrentY = m_CurrentY
        UserControl.Print Text;
        
        UserControl.CurrentX = UserControl.CurrentX + UserControl.ScaleWidth / 2
        Dim CX As Long
        CX = UserControl.CurrentX
        
        UserControl.Print Text;
        
        If m_CurrentX + m_TextWidth < 0 Then
            m_CurrentX = CX
        End If
        
        DoEvents
    End If
End Sub

Private Sub UserControl_DblClick()
    RaiseEvent DblClick
End Sub

Private Sub UserControl_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    RaiseEvent MouseDown(Button, Shift, X, Y)
End Sub

Private Sub UserControl_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    RaiseEvent MouseMove(Button, Shift, X, Y)
End Sub

Private Sub UserControl_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    RaiseEvent MouseUp(Button, Shift, X, Y)
End Sub


Private Sub usercontrol_readproperties(propbag As PropertyBag)
    Me.Text = propbag.ReadProperty("Text", "")
End Sub

Private Sub usercontrol_writeproperties(propbag As PropertyBag)
    propbag.WriteProperty "Text", Me.Text
End Sub


Private Sub UserControl_Resize()
    Refresh
End Sub

Private Sub UserControl_Show()
    Refresh
End Sub


Public Property Get Text() As String
    Text = m_Text
End Property
Public Property Let Text(ByVal Value As String)
    If Value = m_Text Then
        Exit Property
    End If
    
    m_Text = Value
    
    m_TextWidth = UserControl.TextWidth(m_Text)
    
    m_CurrentX = UserControl.ScaleWidth
    m_CurrentY = (UserControl.ScaleHeight - UserControl.TextHeight(m_Text)) / 2


    Timer1.Enabled = m_Text <> ""
    If m_Text = "" Then
        UserControl.Cls
    End If
End Property

Public Sub Refresh()
    Dim parent As Object
    Dim Hwnd As Long

    For Each obj In UserControl.ParentControls
        On Error Resume Next
        Hwnd = obj.Hwnd
        On Error GoTo 0
        
        If UserControl.ContainerHwnd = Hwnd Then
            Set parent = obj
            Exit For
        End If
    Next

    On Error Resume Next
    
    If Not parent Is Nothing Then
        UserControl.PaintPicture parent.Image, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight, UserControl.Extender.Left, UserControl.Extender.Top, UserControl.ScaleWidth, UserControl.ScaleHeight
        UserControl.Picture = UserControl.Image
    End If
End Sub
