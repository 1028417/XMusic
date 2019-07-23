VERSION 5.00
Object = "{3090BD0E-BFD9-4DB3-A049-CD4274B533D3}#2.2#0"; "Qocx.ocx"
Begin VB.UserControl PlayerCtr 
   AutoRedraw      =   -1  'True
   ClientHeight    =   660
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   3600
   ForeColor       =   &H00E0E0E0&
   ScaleHeight     =   660
   ScaleWidth      =   3600
   Begin VB.Timer Timer1 
      Interval        =   1000
      Left            =   2400
      Top             =   -120
   End
   Begin VB.PictureBox MainContainer 
      AutoRedraw      =   -1  'True
      AutoSize        =   -1  'True
      BackColor       =   &H000000FF&
      BorderStyle     =   0  'None
      Height          =   660
      Left            =   0
      Picture         =   "PlayerCtr.ctx":0000
      ScaleHeight     =   660
      ScaleWidth      =   3600
      TabIndex        =   0
      Top             =   0
      Width           =   3600
      Begin QocxPrj.QButton SoundButton 
         Height          =   285
         Left            =   2460
         TabIndex        =   1
         Top             =   300
         Width           =   285
         _ExtentX        =   503
         _ExtentY        =   503
         BackGroundPicture=   "PlayerCtr.ctx":7C04
         MouseMovePicture=   "PlayerCtr.ctx":80CC
         MouseDownPicture=   "PlayerCtr.ctx":8594
         UnabledPicture  =   "PlayerCtr.ctx":8A5C
         Button          =   1
         Text            =   ""
         fcolor          =   -2147483630
         value           =   0   'False
         Enable          =   -1  'True
         AutoSize        =   -1  'True
      End
      Begin QocxPrj.QLabel Monitor 
         Height          =   255
         Left            =   1920
         TabIndex        =   2
         Top             =   360
         Visible         =   0   'False
         Width           =   100
         _ExtentX        =   185
         _ExtentY        =   450
         Text            =   ""
      End
      Begin QocxPrj.QScroll VolumSlider 
         Height          =   255
         Left            =   2815
         TabIndex        =   3
         Top             =   300
         Width           =   675
         _ExtentX        =   1191
         _ExtentY        =   450
         Value           =   100
         Min             =   0
         Max             =   100
         Enable          =   -1  'True
         Scrollable      =   -1  'True
         AutoSize        =   -1  'True
         xMargin         =   0
         yMargin         =   0
         BackGroundPicture=   "PlayerCtr.ctx":8F24
         ForeGroundPicture=   "PlayerCtr.ctx":9880
         ScrollBackGroundPicture=   "PlayerCtr.ctx":A1DC
         ScrollMouseMovePicture=   "PlayerCtr.ctx":A40C
         ScrollMouseDownPicture=   "PlayerCtr.ctx":A63C
         ScrollUnabledPicture=   "PlayerCtr.ctx":A86C
      End
      Begin QocxPrj.QButton LastButton 
         Height          =   285
         Left            =   1000
         TabIndex        =   4
         Top             =   300
         Width           =   285
         _ExtentX        =   503
         _ExtentY        =   503
         BackGroundPicture=   "PlayerCtr.ctx":AA9C
         MouseMovePicture=   "PlayerCtr.ctx":AF64
         MouseDownPicture=   "PlayerCtr.ctx":B42C
         UnabledPicture  =   "PlayerCtr.ctx":B8F4
         Button          =   1
         Text            =   ""
         fcolor          =   -2147483630
         value           =   0   'False
         Enable          =   0   'False
         AutoSize        =   -1  'True
      End
      Begin QocxPrj.QScroll PlayProgram 
         Height          =   120
         Left            =   195
         TabIndex        =   5
         Top             =   60
         Width           =   3180
         _ExtentX        =   5609
         _ExtentY        =   212
         Value           =   0
         Min             =   0
         Max             =   100
         Enable          =   0   'False
         Scrollable      =   -1  'True
         AutoSize        =   -1  'True
         xMargin         =   30
         yMargin         =   0
         BackGroundPicture=   "PlayerCtr.ctx":BDBC
         ForeGroundPicture=   "PlayerCtr.ctx":D1F0
         ScrollBackGroundPicture=   "PlayerCtr.ctx":E624
         ScrollMouseMovePicture=   "PlayerCtr.ctx":E818
         ScrollMouseDownPicture=   "PlayerCtr.ctx":EA0C
         ScrollUnabledPicture=   "PlayerCtr.ctx":EC00
      End
      Begin QocxPrj.QButton NextButton 
         Height          =   285
         Left            =   1450
         TabIndex        =   6
         Top             =   300
         Width           =   285
         _ExtentX        =   503
         _ExtentY        =   503
         BackGroundPicture=   "PlayerCtr.ctx":EDF4
         MouseMovePicture=   "PlayerCtr.ctx":F2BC
         MouseDownPicture=   "PlayerCtr.ctx":F784
         UnabledPicture  =   "PlayerCtr.ctx":FC4C
         Button          =   1
         Text            =   ""
         fcolor          =   -2147483630
         value           =   0   'False
         Enable          =   0   'False
         AutoSize        =   -1  'True
      End
      Begin QocxPrj.QButton PlayButton 
         Height          =   375
         Left            =   60
         TabIndex        =   8
         Top             =   240
         Width           =   375
         _ExtentX        =   661
         _ExtentY        =   661
         BackGroundPicture=   "PlayerCtr.ctx":10114
         MouseMovePicture=   "PlayerCtr.ctx":108D4
         MouseDownPicture=   "PlayerCtr.ctx":11094
         UnabledPicture  =   "PlayerCtr.ctx":11854
         Button          =   1
         Text            =   ""
         fcolor          =   -2147483630
         value           =   0   'False
         Enable          =   0   'False
         AutoSize        =   -1  'True
      End
      Begin QocxPrj.QButton StopButton 
         Height          =   300
         Left            =   570
         TabIndex        =   9
         Top             =   300
         Width           =   300
         _ExtentX        =   529
         _ExtentY        =   529
         BackGroundPicture=   "PlayerCtr.ctx":12014
         MouseMovePicture=   "PlayerCtr.ctx":12518
         MouseDownPicture=   "PlayerCtr.ctx":12A1C
         UnabledPicture  =   "PlayerCtr.ctx":12F20
         Button          =   1
         Text            =   ""
         fcolor          =   -2147483630
         value           =   0   'False
         Enable          =   0   'False
         AutoSize        =   -1  'True
      End
      Begin QocxPrj.QButton PauseButton 
         Height          =   375
         Left            =   60
         TabIndex        =   7
         Top             =   240
         Width           =   375
         _ExtentX        =   661
         _ExtentY        =   661
         BackGroundPicture=   "PlayerCtr.ctx":13424
         MouseMovePicture=   "PlayerCtr.ctx":13BE4
         MouseDownPicture=   "PlayerCtr.ctx":143A4
         UnabledPicture  =   "PlayerCtr.ctx":14B64
         Button          =   1
         Text            =   ""
         fcolor          =   -2147483630
         value           =   0   'False
         Enable          =   0   'False
         AutoSize        =   -1  'True
      End
   End
End
Attribute VB_Name = "PlayerCtr"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Option Explicit

Private Declare Function SetParent Lib "user32" (ByVal hWndChild As Long, ByVal hWndNewParent As Long) As Long


Public Event Resize()

Public Event MouseDown(ByVal button As Integer)
Public Event MouseUp(ByVal button As Integer)

Public Event DblClick()

Public Enum ST_PlayerCtrButton
    OCB_Play = 0
    OCB_Pause
    OCB_Stop
    OCB_Last
    OCB_Next
    
    OCB_Mute
    OCB_Volume
    
    OCB_Progress
End Enum

Public Event OnButton(ByVal eButton As ST_PlayerCtrButton, ByVal para As Integer)

Public WithEvents Skin As CSkinBag
Attribute Skin.VB_VarHelpID = -1

Private m_Enable As Boolean

Public File As String

Public Enum MciMediaStatus
    mmsClosed
    mmsPlayed
    mmsPaused
End Enum

'Public Property Get Status() As MciMediaStatus
'    If Not UserControl.Ambient.UserMode Then
'        Exit Property
'    End If
'
'    Status = m_MciCtr.Status
'End Property

Public Property Let Status(ByVal Value As MciMediaStatus)
    PlayButton.Visible = Value <> mmsPlayed
    StopButton.Enable = Value <> mmsClosed
    
    PlayProgram.Enable = Value = mmsPlayed
    
    'm_MciCtr.Status = Value
    
    If Monitor.Visible Then
        If Value <> mmsClosed Then
            Monitor.Text = Me.File
        Else
            Monitor.Text = ""
        End If
    End If
End Property

Public Sub EnableButton(ByVal nButton As ST_PlayerCtrButton, ByVal bEnable As Boolean)
    Dim button As QButton

    Select Case nButton
        Case OCB_Play
            Set button = PlayButton
        Case OCB_Pause
            Set button = PauseButton
        Case OCB_Stop
            Set button = StopButton
            
        Case OCB_Last
            Set button = LastButton
        Case OCB_Next
            Set button = NextButton
        
        Case OCB_Mute
            Set button = SoundButton
        Case OCB_Volume
            VolumSlider.Enable = bEnable
            Exit Sub
            
        Case OCB_Progress
            PlayProgram.Enable = bEnable
            If Not bEnable Then
                PlayProgram.Value = 0
            End If
            Exit Sub
            
        Case Else
            Exit Sub
    End Select
    
    button.Enable = bEnable
End Sub

Private Sub MainContainer_DblClick()
    RaiseEvent DblClick
End Sub

Private Sub Monitor_DblClick()
    RaiseEvent DblClick
End Sub

Private Sub PlayProgram_ValueChange(ByVal Value As Long)
    RaiseEvent OnButton(OCB_Progress, Value)
End Sub

Private Sub Skin_OnLoadSkin()
    PlayButton.Left = Skin.Item(siiPlayButtonLeft)
    PlayButton.Top = Skin.Item(siiPlayButtonTop)

    PauseButton.Left = Skin.Item(siiPauseButtonLeft)
    PauseButton.Top = Skin.Item(siiPauseButtonTop)
    
    StopButton.Left = Skin.Item(siiStopButtonLeft)
    StopButton.Top = Skin.Item(siiStopButtonTop)

    NextButton.Left = Skin.Item(siiNextButtonLeft)
    NextButton.Top = Skin.Item(siiNextButtonTop)
    
    LastButton.Left = Skin.Item(siiLastButtonLeft)
    LastButton.Top = Skin.Item(siiLastButtonTop)
    
    SoundButton.Left = Skin.Item(siiSoundButtonLeft)
    SoundButton.Top = Skin.Item(siiSoundButtonTop)
    
    PlayProgram.Left = Skin.Item(siiPlayProgramLeft)
    PlayProgram.Top = Skin.Item(siiPlayProgramTop)
    PlayProgram.Scrollable = Skin.Item(siiPlayProgramScrollable)
    PlayProgram.xMargin = Skin.Item(siiPlayProgramxMargin)
    PlayProgram.yMargin = Skin.Item(siiPlayProgramyMargin)
    
    VolumSlider.Left = Skin.Item(siiVolumSliderLeft)
    VolumSlider.Top = Skin.Item(siiVolumSliderTop)
    VolumSlider.Scrollable = Skin.Item(siiVolumSliderScrollable)
    VolumSlider.xMargin = Skin.Item(siiVolumSliderxMargin)
    VolumSlider.yMargin = Skin.Item(siiVolumSlideryMargin)
    

    Set MainContainer.Picture = Skin.Item(siiBackGroundPicture)
    
    Set PlayButton.UnabledPicture = Skin.Item(siiPlayButtonUnabledPicture)
    Set PlayButton.BackGroundPicture = Skin.Item(siiPlayButtonBackGroundPicture)
    Set PlayButton.MouseMovePicture = Skin.Item(siiPlayButtonMouseMovePicture)
    Set PlayButton.MouseDownPicture = Skin.Item(siiPlayButtonMouseDownPicture)
    
    Set PauseButton.UnabledPicture = Skin.Item(siiPauseButtonUnabledPicture)
    Set PauseButton.BackGroundPicture = Skin.Item(siiPauseButtonBackGroundPicture)
    Set PauseButton.MouseMovePicture = Skin.Item(siiPauseButtonMouseMovePicture)
    Set PauseButton.MouseDownPicture = Skin.Item(siiPauseButtonMouseDownPicture)

    Set StopButton.UnabledPicture = Skin.Item(siiStopButtonUnabledPicture)
    Set StopButton.BackGroundPicture = Skin.Item(siiStopButtonBackGroundPicture)
    Set StopButton.MouseMovePicture = Skin.Item(siiStopButtonMouseMovePicture)
    Set StopButton.MouseDownPicture = Skin.Item(siiStopButtonMouseDownPicture)

    Set NextButton.UnabledPicture = Skin.Item(siiNextButtonUnabledPicture)
    Set NextButton.BackGroundPicture = Skin.Item(siiNextButtonBackGroundPicture)
    Set NextButton.MouseMovePicture = Skin.Item(siiNextButtonMouseMovePicture)
    Set NextButton.MouseDownPicture = Skin.Item(siiNextButtonMouseDownPicture)

    Set LastButton.UnabledPicture = Skin.Item(siiLastButtonUnabledPicture)
    Set LastButton.BackGroundPicture = Skin.Item(siiLastButtonBackGroundPicture)
    Set LastButton.MouseMovePicture = Skin.Item(siiLastButtonMouseMovePicture)
    Set LastButton.MouseDownPicture = Skin.Item(siiLastButtonMouseDownPicture)

    Set SoundButton.UnabledPicture = Skin.Item(siiSoundButtonUnabledPicture)
    Set SoundButton.BackGroundPicture = Skin.Item(siiSoundButtonBackGroundPicture)
    Set SoundButton.MouseMovePicture = Skin.Item(siiSoundButtonMouseMovePicture)
    Set SoundButton.MouseDownPicture = Skin.Item(siiSoundButtonMouseDownPicture)
    Set SoundButton.PopDownPicture = Skin.Item(siiSoundButtonPopDownPicture)

    Set PlayProgram.BackGroundPicture = Skin.Item(siiPlayProgramBackGroundPicture)
    Set PlayProgram.ForeGroundPicture = Skin.Item(siiPlayProgramForeGroundPicture)
    Set PlayProgram.ScrollUnabledPicture = Skin.Item(siiPlayProgramScrollUnabledPicture)
    Set PlayProgram.ScrollBackGroundPicture = Skin.Item(siiPlayProgramScrollBackGroundPicture)
    Set PlayProgram.ScrollMouseMovePicture = Skin.Item(siiPlayProgramScrollMouseMovePicture)
    Set PlayProgram.ScrollMouseDownPicture = Skin.Item(siiPlayProgramScrollMouseDownPicture)

    Set VolumSlider.BackGroundPicture = Skin.Item(siiVolumSliderBackGroundPicture)
    Set VolumSlider.ForeGroundPicture = Skin.Item(siiVolumSliderForeGroundPicture)
    Set VolumSlider.ScrollUnabledPicture = Skin.Item(siiVolumSliderScrollUnabledPicture)
    Set VolumSlider.ScrollBackGroundPicture = Skin.Item(siiVolumSliderScrollBackGroundPicture)
    Set VolumSlider.ScrollMouseMovePicture = Skin.Item(siiVolumSliderScrollMouseMovePicture)
    Set VolumSlider.ScrollMouseDownPicture = Skin.Item(siiVolumSliderScrollMouseDownPicture)
    
    Dim bVisible As Boolean
    bVisible = Skin.Item(siiMonitorVisible)
    Monitor.Visible = bVisible
    If bVisible Then
        Monitor.Move Skin.Item(siiMonitorLeft), Skin.Item(siiMonitorTop), Skin.Item(siiMonitorWidth), Skin.Item(siiMonitorHeight)
        Monitor.Refresh
            
        Monitor.Text = Me.File
    End If
    
    
    UserControl.Width = MainContainer.Width
    UserControl.Height = MainContainer.Height
    RaiseEvent Resize
End Sub

Private Sub Skin_OnSaveSkin()
    On Error Resume Next
    Skin.Item(siiMaskShadow1) = LoadResData(101, "PNG")
    Skin.Item(siiMaskShadow2) = LoadResData(102, "PNG")
    Skin.Item(siiMaskShadow3) = LoadResData(103, "PNG")
    On Error GoTo 0
    
    
    Skin.Item(siiPlayButtonLeft) = PlayButton.Left
    Skin.Item(siiPlayButtonTop) = PlayButton.Top

    Skin.Item(siiPauseButtonLeft) = PauseButton.Left
    Skin.Item(siiPauseButtonTop) = PauseButton.Top
        
    Skin.Item(siiStopButtonLeft) = StopButton.Left
    Skin.Item(siiStopButtonTop) = StopButton.Top
    
    Skin.Item(siiNextButtonLeft) = NextButton.Left
    Skin.Item(siiNextButtonTop) = NextButton.Top
        
    Skin.Item(siiLastButtonLeft) = LastButton.Left
    Skin.Item(siiLastButtonTop) = LastButton.Top
    
    Skin.Item(siiSoundButtonLeft) = SoundButton.Left
    Skin.Item(siiSoundButtonTop) = SoundButton.Top
    
    Skin.Item(siiPlayProgramLeft) = PlayProgram.Left
    Skin.Item(siiPlayProgramTop) = PlayProgram.Top
    Skin.Item(siiPlayProgramScrollable) = PlayProgram.Scrollable
    Skin.Item(siiPlayProgramxMargin) = PlayProgram.xMargin
    Skin.Item(siiPlayProgramyMargin) = PlayProgram.yMargin
    
    Skin.Item(siiVolumSliderLeft) = VolumSlider.Left
    Skin.Item(siiVolumSliderTop) = VolumSlider.Top
    Skin.Item(siiVolumSliderScrollable) = VolumSlider.Scrollable
    Skin.Item(siiVolumSliderxMargin) = VolumSlider.xMargin
    Skin.Item(siiVolumSlideryMargin) = VolumSlider.yMargin
    
    
    Skin.Item(siiMonitorVisible) = 0
    
    If Monitor.Width > 200 Then
        Skin.Item(siiMonitorVisible) = 1

        Skin.Item(siiMonitorLeft) = Monitor.Left
        Skin.Item(siiMonitorTop) = Monitor.Top
        Skin.Item(siiMonitorWidth) = Monitor.Width
        Skin.Item(siiMonitorHeight) = Monitor.Height
    End If

    Skin.Item(siiBackGroundPicture) = MainContainer.Picture
    
    Skin.Item(siiPlayButtonUnabledPicture) = PlayButton.UnabledPicture
    Skin.Item(siiPlayButtonBackGroundPicture) = PlayButton.BackGroundPicture
    Skin.Item(siiPlayButtonMouseMovePicture) = PlayButton.MouseMovePicture
    Skin.Item(siiPlayButtonMouseDownPicture) = PlayButton.MouseDownPicture
    
    Skin.Item(siiPauseButtonUnabledPicture) = PauseButton.UnabledPicture
    Skin.Item(siiPauseButtonBackGroundPicture) = PauseButton.BackGroundPicture
    Skin.Item(siiPauseButtonMouseMovePicture) = PauseButton.MouseMovePicture
    Skin.Item(siiPauseButtonMouseDownPicture) = PauseButton.MouseDownPicture
    
    Skin.Item(siiStopButtonUnabledPicture) = StopButton.UnabledPicture
    Skin.Item(siiStopButtonBackGroundPicture) = StopButton.BackGroundPicture
    Skin.Item(siiStopButtonMouseMovePicture) = StopButton.MouseMovePicture
    Skin.Item(siiStopButtonMouseDownPicture) = StopButton.MouseDownPicture
    
    Skin.Item(siiNextButtonUnabledPicture) = NextButton.UnabledPicture
    Skin.Item(siiNextButtonBackGroundPicture) = NextButton.BackGroundPicture
    Skin.Item(siiNextButtonMouseMovePicture) = NextButton.MouseMovePicture
    Skin.Item(siiNextButtonMouseDownPicture) = NextButton.MouseDownPicture
    
    Skin.Item(siiLastButtonUnabledPicture) = LastButton.UnabledPicture
    Skin.Item(siiLastButtonBackGroundPicture) = LastButton.BackGroundPicture
    Skin.Item(siiLastButtonMouseMovePicture) = LastButton.MouseMovePicture
    Skin.Item(siiLastButtonMouseDownPicture) = LastButton.MouseDownPicture
    
    Skin.Item(siiSoundButtonUnabledPicture) = SoundButton.UnabledPicture
    Skin.Item(siiSoundButtonBackGroundPicture) = SoundButton.BackGroundPicture
    Skin.Item(siiSoundButtonMouseMovePicture) = SoundButton.MouseMovePicture
    Skin.Item(siiSoundButtonMouseDownPicture) = SoundButton.MouseDownPicture
    Skin.Item(siiSoundButtonPopDownPicture) = SoundButton.PopDownPicture
    
    Skin.Item(siiPlayProgramBackGroundPicture) = PlayProgram.BackGroundPicture
    Skin.Item(siiPlayProgramForeGroundPicture) = PlayProgram.ForeGroundPicture
    Skin.Item(siiPlayProgramScrollUnabledPicture) = PlayProgram.ScrollUnabledPicture
    Skin.Item(siiPlayProgramScrollBackGroundPicture) = PlayProgram.ScrollBackGroundPicture
    Skin.Item(siiPlayProgramScrollMouseMovePicture) = PlayProgram.ScrollMouseMovePicture
    Skin.Item(siiPlayProgramScrollMouseDownPicture) = PlayProgram.ScrollMouseDownPicture
    
    Skin.Item(siiVolumSliderBackGroundPicture) = VolumSlider.BackGroundPicture
    Skin.Item(siiVolumSliderForeGroundPicture) = VolumSlider.ForeGroundPicture
    Skin.Item(siiVolumSliderScrollUnabledPicture) = VolumSlider.ScrollUnabledPicture
    Skin.Item(siiVolumSliderScrollBackGroundPicture) = VolumSlider.ScrollBackGroundPicture
    Skin.Item(siiVolumSliderScrollMouseMovePicture) = VolumSlider.ScrollMouseMovePicture
    Skin.Item(siiVolumSliderScrollMouseDownPicture) = VolumSlider.ScrollMouseDownPicture
End Sub


Private Sub Timer1_Timer()
    If PlayProgram.Enable Then
        If PlayProgram.Value < PlayProgram.Max Then
            PlayProgram.Value = PlayProgram.Value + 1
        End If
    End If
End Sub

Private Sub UserControl_Initialize()
    m_Enable = 1
    
    Set Skin = New CSkinBag
        
    UserControl.Width = MainContainer.Width
    UserControl.Height = MainContainer.Height
    RaiseEvent Resize
End Sub

Private Sub Monitor_MouseDown(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseDown(button)
End Sub
Private Sub MainContainer_MouseDown(button As Integer, Shift As Integer, x As Single, y As Single)
    RaiseEvent MouseDown(button)
End Sub

Private Sub MainContainer_MouseUp(button As Integer, Shift As Integer, x As Single, y As Single)
    RaiseEvent MouseUp(button)
End Sub
Private Sub PlayButton_MouseUp(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseUp(button)
End Sub
Private Sub PauseButton_MouseUp(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseUp(button)
End Sub
Private Sub StopButton_MouseUp(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseUp(button)
End Sub
Private Sub LastButton_MouseUp(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseUp(button)
End Sub
Private Sub NextButton_MouseUp(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseUp(button)
End Sub
Private Sub SoundButton_MouseUp(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseUp(button)
End Sub
Private Sub PlayProgram_MouseUp(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseUp(button)
End Sub
Private Sub VolumSlider_MouseUp(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseUp(button)
End Sub
Private Sub Monitor_MouseUp(ByVal button As Integer, ByVal Shift As Integer, ByVal x As Single, ByVal y As Single)
    RaiseEvent MouseUp(button)
End Sub

Private Sub PlayButton_Click()
    Status = mmsPlayed
        
    RaiseEvent OnButton(OCB_Play, 0)
End Sub

Private Sub PauseButton_Click()
    Status = mmsPaused
    
    RaiseEvent OnButton(OCB_Pause, 0)
End Sub

Private Sub StopButton_Click()
    Status = mmsClosed

    RaiseEvent OnButton(OCB_Stop, 0)
    
    PlayProgram.Enable = False
    PlayProgram.Value = 0
End Sub

Private Sub LastButton_Click()
    Static bDoing As Boolean
    If bDoing Then
        Exit Sub
    End If
    bDoing = 1
    
    RaiseEvent OnButton(OCB_Last, 0)
    
    bDoing = 0
End Sub

Private Sub NextButton_Click()
    Static bDoing As Boolean
    If bDoing Then
        Exit Sub
    End If
    bDoing = 1
    
    RaiseEvent OnButton(OCB_Next, 0)

    bDoing = 0
End Sub

Private Sub SoundButton_Click()
    SoundButton.Value = Not SoundButton.Value
    'm_MciCtr.Soundable = Not SoundButton.Value
    
    RaiseEvent OnButton(OCB_Mute, IIf(SoundButton.Value, 1, 0))
End Sub

Private Sub VolumSlider_ValueChange(ByVal Value As Long)
    SoundButton.Value = False
    
    RaiseEvent OnButton(OCB_Volume, VolumSlider.Value)
End Sub

Public Property Let Enable(ByVal Value As Boolean)
    m_Enable = Value

    SoundButton.Enable = Value
    VolumSlider.Enable = Value
    
    'If Status <> mmsClosed Then
        PlayButton.Enable = Value
        PauseButton.Enable = Value
        StopButton.Enable = Value
        LastButton.Enable = Value
        NextButton.Enable = Value
    
        PlayProgram.Enable = Value
    'End If
End Property
Public Property Get Enable() As Boolean
    Enable = m_Enable
End Property

Public Property Get Image() As StdPicture
    Set Image = UserControl.Image
End Property


Public Property Get Soundable() As Boolean
    Soundable = Not SoundButton.Value
End Property
Public Property Let Soundable(ByVal Value As Boolean)
    SoundButton.Value = Not Value
    'm_MciCtr.Soundable = Value
End Property

Public Property Get Volum() As Integer
    Volum = VolumSlider.Value
End Property
Public Property Let Volum(ByVal Value As Integer)
    VolumSlider.Value = Value
    'm_MciCtr.Volum = Value
End Property

Public Sub clear()
    Me.Status = mmsClosed
    
    PlayButton.Enable = 0
    PauseButton.Enable = 0
    StopButton.Enable = 0
    
    LastButton.Enable = 0
    NextButton.Enable = 0

    PlayProgram.Enable = 0
    
    PlayProgram.init 0, 1, 0
End Sub

Public Sub SetPlayState(ByVal strFile As String, ByVal lDuration As Long, ByVal lCurrPos As Long)
    If Not Me.Enable Then
        Exit Sub
    End If

    Me.File = strFile
    If strFile <> "" Then
        PlayButton.Enable = 1
        PauseButton.Enable = 1
        StopButton.Enable = 1
        
        LastButton.Enable = 1
        NextButton.Enable = 1
        
        PlayProgram.Enable = 1
        
        Status = mmsPlayed
    Else
        Me.Status = mmsClosed
        
        'PlayButton.Enable = 0
        PauseButton.Enable = 0
        StopButton.Enable = 0
        
        'LastButton.Enable = 0
        'NextButton.Enable = 0

        PlayProgram.Enable = 0
    End If
            
    PlayProgram.init 0, IIf(lDuration > 0, lDuration, 1), lCurrPos
End Sub

