VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "PlaySpirit"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

Private Declare Function ReleaseCapture Lib "user32" () As Long
Private Declare Function SendMessage Lib "user32" Alias "SendMessageA" (ByVal hwnd As Long, ByVal wMsg As Long, ByVal wParam As Long, lParam As Any) As Long

Const GWL_EXSTYLE As Long = -20
Const WS_EX_LAYERED As Long = &H80000

Private Declare Function GetWindowLong Lib "user32.dll" Alias "GetWindowLongA" (ByVal hwnd As Long, ByVal nIndex As Long) As Long
Private Declare Function SetWindowLong Lib "user32.dll" Alias "SetWindowLongA" (ByVal hwnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Private Declare Function SetLayeredWindowAttributes Lib "user32.dll" (ByVal hwnd As Long, ByVal crKey As Long, ByVal bAlpha As Byte, ByVal dwFlags As Long) As Long

Private Declare Function LockWindowUpdate Lib "user32" (ByVal hwndLock As Long) As Long

Private Declare Function IsWindowEnabled Lib "user32" (ByVal hwnd As Long) As Long

Private Declare Function SetWindowPos Lib "user32" (ByVal hwnd As Long, ByVal hWndInsertAfter As Long, ByVal x As Long, ByVal y As Long, ByVal cx As Long, ByVal cy As Long, ByVal wFlags As Long) As Long

Private WithEvents m_Player As PlayerCtr
Attribute m_Player.VB_VarHelpID = -1

Private m_Shadow As CShadow

Private m_PlayerDefaultSkinBuffer As Variant

Public Event OnRButtonUp()
Public Event OnDblClick()

Public Event OnButton(ByVal eButton As ST_PlaySpiritButton, ByVal para As Integer)


Public Enum ST_PlaySpiritButton
    PSB_Play = ST_PlayerCtrButton.OCB_Play
    PSB_Pause = ST_PlayerCtrButton.OCB_Pause
    PSB_Stop = ST_PlayerCtrButton.OCB_Stop
    PSB_Last = ST_PlayerCtrButton.OCB_Last
    PSB_Next = ST_PlayerCtrButton.OCB_Next
    
    PSB_Mute = OCB_Mute
    PSB_Volume = OCB_Volume
    
    PSB_Progress = OCB_Progress
End Enum

Private m_bDestroy As Boolean

Private Sub Class_Initialize()
    SetWindowLong PlayerForm.hwnd, GWL_EXSTYLE, GetWindowLong(PlayerForm.hwnd, GWL_EXSTYLE) Or WS_EX_LAYERED
    SetLayeredWindowAttributes PlayerForm.hwnd, 0, 0, 0
    
    Set m_Player = PlayerForm.Player
    m_PlayerDefaultSkinBuffer = m_Player.Skin.Save()
    
    Set m_Shadow = PlayerForm.m_Shadow
End Sub

Private Sub Class_Terminate()
    If m_bDestroy Then
        End
    End If
End Sub

Public Sub Destroy()
'    If m_Player.Status = mmsPlayed Then
'        m_Player.Status = mmsPaused
'
'        Dim preTimer As Single
'        preTimer = Timer
'
'        Do While Timer - preTimer < 1.5
'            DoEvents
'        Loop
'    End If


    Set m_Player = Nothing
    Set m_Shadow = Nothing
    Unload PlayerForm
    Set PlayerForm = Nothing
    
    m_bDestroy = True
End Sub

Public Sub LoadSkin(Optional ByVal strSkbFilePath As String = "")
    m_Shadow.UnsetShadow

    'PlayerForm.Visible = False
    'LockWindowUpdate PlayerForm.hwnd

    If strSkbFilePath = "" Then
        m_Player.Skin.Load m_PlayerDefaultSkinBuffer
    Else
        m_Player.Skin.LoadFile strSkbFilePath
    End If


    PlayerForm.Move PlayerForm.Left - (m_Player.Width - PlayerForm.Width) / 2 _
        , PlayerForm.Top - (m_Player.Height - PlayerForm.Height) / 2, m_Player.Width, m_Player.Height

    If IsArray(m_Player.Skin.MaskShadow(1)) Then
        SetLayeredWindowAttributes PlayerForm.hwnd, 255, 0, &H1
    Else
        SetLayeredWindowAttributes PlayerForm.hwnd, 0, 0, 0
    End If

    'DoEvents
    'PlayerForm.Visible = True
    'LockWindowUpdate 0

    m_Shadow.SetShadow PlayerForm, m_Player.Skin.MaskShadow(1), m_Player.Skin.MaskShadow(2), m_Player.Skin.MaskShadow(3)
End Sub

Public Sub SetShadowLevel(ByVal nLevel As Integer)
    m_Shadow.Thickness = nLevel
End Sub

Public Function Show(ByVal x As Integer, ByVal y As Integer) As Long
    App.TaskVisible = False
    
    m_Shadow.SetShadow PlayerForm, m_Player.Skin.MaskShadow(1), m_Player.Skin.MaskShadow(2), m_Player.Skin.MaskShadow(3)

    'SetWindowPos PlayerForm.hwnd, 0, x, y, 0, 0, 5

    PlayerForm.Move x, y
    PlayerForm.Show

    Show = PlayerForm.hwnd
End Function

Public Function GetShadowHandle() As Long
    GetShadowHandle = m_Shadow.GetWndHandle()
End Function

Public Sub clear()
    m_Player.clear
    DoEvents
End Sub

Public Sub SetPlayState(ByVal strMediaPath As String, ByVal lDuration As Long, ByVal lCurrPos As Long)
    m_Player.SetPlayState strMediaPath, lDuration, lCurrPos
    DoEvents
End Sub

'Public Function GetPlayStatus() As Boolean
'    GetPlayStatus = m_Player.Status = mmsPlayed
'End Function

Public Sub SetPause(ByVal bPause As Boolean)
    If bPause Then
        m_Player.Status = mmsPaused
    Else
        m_Player.Status = mmsPlayed
    End If
End Sub

Public Property Get MediaPath() As String
    MediaPath = m_Player.File
End Property

Public Property Let MediaPath(ByVal strMediaPath As String)
    m_Player.File = strMediaPath
End Property

Public Property Get Volum() As Integer
    Volum = m_Player.Volum
End Property

Public Property Let Volum(ByVal nVolum As Integer)
    m_Player.Volum = nVolum
End Property


Public Sub EnableButton(ByVal nButton As ST_PlaySpiritButton, ByVal bEnable As Boolean)
    m_Player.EnableButton nButton, bEnable
End Sub

Private Sub m_Player_DblClick()
    RaiseEvent OnDblClick
End Sub

Private Sub m_Player_MouseDown(ByVal Button As Integer)
    If Button = 1 Then
        DoEvents

        ReleaseCapture
        SendMessage PlayerForm.hwnd, &H112, &HF010& + 2, 0
    End If
End Sub

Private Sub m_Player_MouseUp(ByVal Button As Integer)
    If Button = 2 Then
        RaiseEvent OnRButtonUp
    End If
End Sub

Private Sub m_Player_OnButton(ByVal eButton As QPlayerOcxPrj.ST_PlayerCtrButton, ByVal para As Integer)
    RaiseEvent OnButton(eButton, para)
End Sub
