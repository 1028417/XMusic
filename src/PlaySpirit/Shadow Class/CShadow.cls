VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "CShadow"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

Private Const WS_EX_LAYERED = &H80000
Private Const WS_EX_TRANSPARENT = &H20

Private Const GWL_STYLE As Long = -16
Private Const GWL_EXSTYLE As Long = -20

Private Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal hwnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Private Declare Function GetWindowLong Lib "user32.dll" Alias "GetWindowLongA" (ByVal hwnd As Long, ByVal nIndex As Long) As Long

Const SWP_NOSIZE = &H1
Const SWP_NOMOVE = &H2
Const SWP_NOZORDER = &H4
Const SWP_NOACTIVATE = &H10
Const SWP_SHOWWINDOW = &H40
Private Declare Function SetWindowPos Lib "user32" (ByVal hwnd As Long, ByVal hWndInsertAfter As Long, ByVal X As Long, ByVal Y As Long, ByVal cx As Long, ByVal cy As Long, ByVal wFlags As Long) As Long

Const GWL_HWNDPARENT = (-8)

Const WM_ACTIVATE = &H6
Private Declare Function SetForegroundWindow Lib "user32" (ByVal hwnd As Long) As Long

Private Declare Function IsWindowEnabled Lib "user32" (ByVal hwnd As Long) As Long


Private m_hGdiplusToken As Long


Private m_ShadowForm As ShadowForm


Private m_ParentFrm As Form

Dim WithEvents MsgHook As CMsgHook
Attribute MsgHook.VB_VarHelpID = -1


Private m_Thickness As Integer


Private m_Png1 As Variant
Private m_Png2 As Variant
Private m_Png3 As Variant


Private Sub Class_Initialize()
    m_Thickness = 1

    m_hGdiplusToken = StartToken
End Sub

Private Sub Class_Terminate()
    Me.UnsetShadow

    StopToken m_hGdiplusToken
End Sub

Public Sub SetShadow(frm As Object, Optional ByVal Png1 As Variant, Optional ByVal Png2 As Variant, Optional ByVal Png3 As Variant)
    If Me.Exists Then
        Exit Sub
    End If

    Set m_ParentFrm = frm
    Set MsgHook = New CMsgHook
    MsgHook.SetHook m_ParentFrm.hwnd

    Set m_ShadowForm = New ShadowForm
    SetWindowLong m_ShadowForm.hwnd, GWL_EXSTYLE, GetWindowLong(m_ShadowForm.hwnd, GWL_EXSTYLE) Or WS_EX_LAYERED Or WS_EX_TRANSPARENT
    SetWindowLong m_ShadowForm.hwnd, -8, m_ParentFrm.hwnd

    m_Png1 = Png1
    m_Png2 = Png2
    m_Png3 = Png3

    Me.Show
End Sub

Public Sub UnsetShadow()
    If Not Me.Exists Then
        Exit Sub
    End If

    Set m_ParentFrm = Nothing
    MsgHook.SetHook 0
    Set MsgHook = Nothing

    DelTrans m_ShadowForm
    Unload m_ShadowForm
    Set m_ShadowForm = Nothing
End Sub

Public Function GetWndHandle() As Long
    GetWndHandle = m_ShadowForm.hwnd
End Function

Private Sub OnSetShadow()
    If Not Me.Exists Then
        Exit Sub
    End If

    DelTrans m_ShadowForm

    If IsArray(m_Png1) Then
        If m_Thickness = 1 Then
            SetLayer m_ShadowForm, m_Png1
        ElseIf m_Thickness = 2 Then
            SetLayer m_ShadowForm, m_Png2
        ElseIf m_Thickness = 3 Then
            SetLayer m_ShadowForm, m_Png3
        End If
    Else
        SetLayer m_ShadowForm, LoadResData(100 + m_Thickness, "PNG")
    End If

    SetPosize
    UpdateLayer m_ShadowForm, Not IsArray(m_Png1)
End Sub

Public Sub Show()
    If Not Me.Exists Then
        Exit Sub
    End If
    
    If Me.Visible Then
        Exit Sub
    End If

    OnSetShadow

    'm_ShadowForm.Show 0, m_ParentFrm

    Load m_ShadowForm
    SetWindowLong m_ShadowForm.hwnd, GWL_HWNDPARENT, m_ParentFrm.hwnd
    SetWindowPos m_ShadowForm.hwnd, 0, 0, 0, 0, 0, SWP_SHOWWINDOW Or SWP_NOACTIVATE Or SWP_NOZORDER Or SWP_NOSIZE Or SWP_NOMOVE
End Sub

Public Sub Hide()
    If Not Me.Visible Then
        Exit Sub
    End If
    
    m_ShadowForm.Hide
End Sub

Private Sub SetPosize()
    Const nWidth As Integer = 20

    m_ShadowForm.Move m_ParentFrm.Left - nWidth * Screen.TwipsPerPixelX, m_ParentFrm.Top - nWidth * Screen.TwipsPerPixelY _
        , m_ParentFrm.Width + nWidth * 2 * Screen.TwipsPerPixelX, m_ParentFrm.Height + nWidth * 2 * Screen.TwipsPerPixelY

    DoEvents
End Sub

Public Property Get Exists() As Boolean
    Exists = Not m_ShadowForm Is Nothing
End Property

Public Property Get Visible() As Boolean
    If Not Me.Exists Then
        Exit Property
    End If

    Visible = m_ShadowForm.Visible
End Property

Public Property Get Thickness() As Integer
    Thickness = m_Thickness
End Property
Public Property Let Thickness(ByVal Value As Integer)
    If Value < 1 Then
        Value = 1
    ElseIf Value > 3 Then
        Value = 3
    End If

    If Value = m_Thickness Then
        Exit Property
    End If

    If Not Exists Then
        Exit Property
    End If

    m_Thickness = Value

    OnSetShadow
End Property

Private Sub MsgHook_MsgEvent(ByVal hwnd As Long, ByVal Msg As Long, ByVal Arg1 As Long, ByVal Arg2 As Long, ByVal Arg3 As Long)
    If Msg = MsgHook.WM_MOVE Or Msg = MsgHook.WM_SIZE And Me.Visible Then
        SetPosize

        If Msg = MsgHook.WM_SIZE Then
            UpdateLayer m_ShadowForm, Not IsArray(m_Png1)
        End If
    End If

'    Static bFlag As Boolean
'    If Msg = WM_ACTIVATE And Arg1 And Not bFlag Then
'        bFlag = True
'
'        If IsWindowEnabled(m_ParentFrm.hwnd) Then
'            SetForegroundWindow m_ShadowForm.hwnd
'            SetForegroundWindow m_ParentFrm.hwnd
'        End If
'
'        bFlag = False
'    End If
End Sub

