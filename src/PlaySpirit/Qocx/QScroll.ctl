VERSION 5.00
Begin VB.UserControl QScroll 
   AutoRedraw      =   -1  'True
   BackColor       =   &H8000000C&
   ClientHeight    =   255
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   3795
   ForeColor       =   &H8000000D&
   ScaleHeight     =   255
   ScaleWidth      =   3795
   Begin QocxPrj.QButton ScrollButton 
      Height          =   255
      Left            =   1680
      TabIndex        =   0
      Top             =   0
      Width           =   255
      _ExtentX        =   450
      _ExtentY        =   450
      Button          =   1
      Text            =   ""
      fcolor          =   0
      value           =   0   'False
      Enable          =   -1  'True
      AutoSize        =   -1  'True
   End
End
Attribute VB_Name = "QScroll"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Private Declare Function SetCapture Lib "user32" (ByVal Hwnd As Long) As Long
Private Declare Function GetCapture Lib "user32" () As Long

Public Event ValueChange(ByVal Value As Long)

Public Event Scroll()

Public Event MouseUp(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)

Dim m_Max As Long, m_Min As Long, m_Value As Long, m_Enable As Boolean

Dim m_ScrollBackGroundPicture As StdPicture, m_ScrollMouseMovePicture As StdPicture, m_ScrollMouseDownPicture As StdPicture, m_ScrollUnabledPicture As StdPicture

Dim m_xMargin As Long
Dim m_yMargin As Long

Dim m_BackGroundPicture As StdPicture
Dim m_ForeGroundPicture As StdPicture

Const MIN_HEIGHT = 50
Const MIN_WIDTH = 500

Dim m_Scrollable As Boolean

Dim m_AutoSize As Boolean

Dim m_ForeGroundPictureWidth As Long

Public Property Get Hwnd() As Long
    Hwnd = UserControl.Hwnd
End Property

Public Property Get AutoSize() As Boolean
    AutoSize = m_AutoSize
End Property
Public Property Let AutoSize(ByVal NewValue As Boolean)
    m_AutoSize = NewValue
    PropertyChanged "AutoSize"
    
    If NewValue Then
        Set Me.BackGroundPicture = Me.BackGroundPicture
    End If
End Property

Public Property Get Scrollable() As Boolean
    Scrollable = m_Scrollable
End Property
Public Property Let Scrollable(ByVal NewValue As Boolean)
    m_Scrollable = NewValue
    PropertyChanged "Scrollable"
    
    Me.Value = Me.Value
End Property

Public Property Get BackGroundPicture() As StdPicture
    Set BackGroundPicture = m_BackGroundPicture
End Property
Public Property Set BackGroundPicture(ByVal NewValue As StdPicture)
    Set m_BackGroundPicture = NewValue
    PropertyChanged "BackGroundPicture"
    
    If m_BackGroundPicture Is Nothing Then
        UserControl.Cls
        Set UserControl.Picture = Nothing
    
    Else
        If Me.AutoSize Then
            UserControl.Width = ScaleX(m_BackGroundPicture.Width, 8, vbTwips)
            UserControl.Height = ScaleY(m_BackGroundPicture.Height, 8, vbTwips)
        End If
        
        UserControl.PaintPicture m_BackGroundPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
        Set UserControl.Picture = UserControl.Image
                
        Me.Value = Me.Value
    End If
End Property

Public Property Get ForeGroundPicture() As StdPicture
    Set ForeGroundPicture = m_ForeGroundPicture
End Property
Public Property Set ForeGroundPicture(ByVal NewValue As StdPicture)
    Set m_ForeGroundPicture = NewValue
    PropertyChanged "ForeGroundPicture"

    If Not m_ForeGroundPicture Is Nothing Then
        m_ForeGroundPictureWidth = ScaleX(m_ForeGroundPicture.Width, 8, vbTwips)
    End If
    Me.Value = Me.Value
End Property

Public Property Get xMargin() As Long
    xMargin = m_xMargin
End Property
Public Property Let xMargin(ByVal NewValue As Long)
    If NewValue < 0 Or NewValue > UserControl.ScaleWidth Then
        Exit Property
    End If
    
    m_xMargin = NewValue
    PropertyChanged "xMargin"

    Me.Value = Me.Value
End Property

Private Property Get TruexMargin() As Long
    TruexMargin = xMargin + IIf(Me.Scrollable, ScrollButton.Width / 2, 0)
End Property

Public Property Get yMargin() As Long
    yMargin = m_yMargin
End Property
Public Property Let yMargin(ByVal NewValue As Long)
    If NewValue < 0 Or NewValue > UserControl.ScaleHeight Then
        Exit Property
    End If
    
    m_yMargin = NewValue
    PropertyChanged "yMargin"

    Me.Value = Me.Value
End Property

Public Property Get Enable() As Boolean
    Enable = m_Enable
End Property
Public Property Let Enable(ByVal NewValue As Boolean)
    m_Enable = NewValue
    
    ScrollButton.Enable = m_Enable
    PropertyChanged "Enable"
End Property

Public Sub init(ByVal min As Long, ByVal max As Long, ByVal Value As Long)
    m_Min = min
    m_Max = max
    SetValue Value
End Sub

Public Property Get max() As Long
    max = m_Max
End Property
Public Property Let max(ByVal NewValue As Long)
    Dim t As Long
    
    If NewValue > m_Min Then
        t = m_Max
        
        m_Max = NewValue
        
        Value = m_Min + (Value - m_Min) / (t - m_Min) * (m_Max - m_Min)
        
        PropertyChanged "Max"
    End If
End Property

Public Property Get min() As Long
    min = m_Min
End Property
Public Property Let min(ByVal NewValue As Long)
    Dim t As Long
    
    If NewValue < m_Max Then
        t = m_Min
        
        m_Min = NewValue

        Value = m_Min + (Value - t) / (m_Max - t) * (m_Max - m_Min)
        
        PropertyChanged "Min"
    End If
End Property

Public Property Get Value() As Long
    Value = m_Value
End Property
Public Property Let Value(ByVal NewValue As Long)
    If GetCapture() = Me.Hwnd Then
        Exit Property
    End If
    
    SetValue NewValue
End Property

Private Sub SetValue(ByVal NewValue As Long)
    Dim CX As Long
    
    If NewValue < Me.min Then
        NewValue = Me.min
    End If
    
    If NewValue > Me.max Then
        NewValue = Me.max
    End If
    
    m_Value = NewValue
    
    UserControl.Cls
    
    CX = (NewValue - min) / (max - min) * (UserControl.ScaleWidth - TruexMargin * 2) + IIf(Me.Scrollable, ScrollButton.Width / 2, 0)

    If m_Value > Me.min And CX >= 15 Then
        If Me.ForeGroundPicture Is Nothing Then
            UserControl.Line (xMargin, yMargin)-(CX + TruexMargin, UserControl.ScaleHeight - yMargin), , BF

        Else
            If CX > m_ForeGroundPictureWidth Then
                UserControl.PaintPicture Me.ForeGroundPicture, xMargin, yMargin, CX, UserControl.ScaleHeight - yMargin * 2 ', 0, 0, CX, UserControl.ScaleHeight - m_yMargin * 2
            Else
                UserControl.PaintPicture Me.ForeGroundPicture, xMargin, yMargin, CX, UserControl.ScaleHeight - yMargin * 2, 0, 0, CX, UserControl.ScaleHeight - m_yMargin * 2
            End If
        End If
    End If
    
    If Me.Scrollable Then
        ScrollButton.Move xMargin + CX - ScrollButton.Width / 2, (UserControl.ScaleHeight - ScrollButton.Height) / 2
    End If
    ScrollButton.Visible = Me.Scrollable
            
    'PropertyChanged "Value"
    
    DoEvents
End Sub

Private Sub ScrollButton_MouseDown(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)
    If Not m_Enable Then
        Exit Sub
    End If
    
    If Button <> 1 Then
        Exit Sub
    End If
    
    UserControl.SetFocus
    
    SetCapture Hwnd
End Sub

Private Sub ScrollButton_MouseUp(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)
    SetCapture 0
    
    RaiseEvent MouseUp(Button, Shift, ScrollButton.Left + X, ScrollButton.Top + Y)
End Sub

Private Sub UserControl_Initialize()
    m_Max = 100
    
    m_AutoSize = 1
    
    m_Enable = 1
End Sub

Private Sub UserControl_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If Not m_Enable Then
        Exit Sub
    End If
    
    If Button <> 1 Then
        Exit Sub
    End If
    
    SetCapture Hwnd
    
    UserControl_MouseMove Button, Shift, X, Y
End Sub

Private Sub UserControl_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If Not m_Enable Then
        Exit Sub
    End If
    
    If Button <> 1 Then
        Exit Sub
    End If
    
    X = X - TruexMargin
    
    SetValue X * (max - min) / (UserControl.ScaleWidth - TruexMargin * 2) + min
    
    RaiseEvent ValueChange(m_Value)
End Sub

Private Sub UserControl_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    RaiseEvent MouseUp(Button, Shift, X, Y)
    
    If Not m_Enable Then
        Exit Sub
    End If
    
    If Button <> 1 Then
        Exit Sub
    End If
    
    If Me.Scrollable Then
        Set ScrollButton.BackGroundPicture = ScrollButton.BackGroundPicture
    End If
    
    SetCapture 0
    
    RaiseEvent Scroll
End Sub

Private Sub UserControl_Resize()
    If UserControl.Width < MIN_WIDTH + xMargin * 2 Then
        UserControl.Width = MIN_WIDTH + xMargin * 2
    End If
    
    If UserControl.Height < MIN_HEIGHT + m_yMargin * 2 Then
        UserControl.Height = MIN_HEIGHT + m_yMargin * 2
    End If
    
    UserControl.ScaleWidth = UserControl.Width
    UserControl.ScaleHeight = UserControl.Height
    
    If Me.Scrollable And Not Me.AutoSize Then
        ScrollButton.Top = m_yMargin
        ScrollButton.Height = UserControl.ScaleHeight - m_yMargin * 2
    End If
    
    Set Me.BackGroundPicture = Me.BackGroundPicture
    Me.Value = Me.Value
End Sub
Private Sub usercontrol_writeproperties(propbag As PropertyBag)
    With propbag
        .WriteProperty "Value", Me.Value
        .WriteProperty "Min", Me.min
        .WriteProperty "Max", Me.max
        
        .WriteProperty "Enable", Me.Enable
        .WriteProperty "Scrollable", Me.Scrollable
        .WriteProperty "AutoSize", Me.AutoSize
        
        .WriteProperty "xMargin", Me.xMargin
        .WriteProperty "yMargin", Me.yMargin
        
        .WriteProperty "BackGroundPicture", Me.BackGroundPicture
        .WriteProperty "ForeGroundPicture", Me.ForeGroundPicture
        
        .WriteProperty "ScrollBackGroundPicture", Me.ScrollBackGroundPicture
        .WriteProperty "ScrollMouseMovePicture", Me.ScrollMouseMovePicture
        .WriteProperty "ScrollMouseDownPicture", Me.ScrollMouseDownPicture
        .WriteProperty "ScrollUnabledPicture", Me.ScrollUnabledPicture
    End With
End Sub
Private Sub usercontrol_readproperties(propbag As PropertyBag)
    With propbag
        m_Max = .ReadProperty("Max", 100)
        m_Min = .ReadProperty("Min", 0)
        Value = .ReadProperty("Value", 0)
        
        Enable = .ReadProperty("Enable", True)
        Scrollable = .ReadProperty("Scrollable", 0)
        AutoSize = .ReadProperty("AutoSize", 1)
        
        Me.xMargin = .ReadProperty("xMargin", 30)
        yMargin = .ReadProperty("yMargin", 30)
        
        Set Me.BackGroundPicture = .ReadProperty("BackGroundPicture", Nothing)
        Set Me.ForeGroundPicture = .ReadProperty("ForeGroundPicture", Nothing)
        
        Set Me.ScrollBackGroundPicture = .ReadProperty("ScrollBackGroundPicture", Nothing)
        Set Me.ScrollMouseMovePicture = .ReadProperty("ScrollMouseMovePicture", Nothing)
        Set Me.ScrollMouseDownPicture = .ReadProperty("ScrollMouseDownPicture", Nothing)
        Set Me.ScrollUnabledPicture = .ReadProperty("ScrollUnabledPicture", Nothing)
    End With
End Sub


Public Property Get ScrollUnabledPicture() As StdPicture
    Set ScrollUnabledPicture = m_ScrollUnabledPicture
End Property
Public Property Set ScrollUnabledPicture(ByVal NewValue As StdPicture)
    Set m_ScrollUnabledPicture = NewValue
    
    PropertyChanged "ScrollUnabledPicture"

    Set ScrollButton.UnabledPicture = NewValue
End Property

Public Property Get ScrollBackGroundPicture() As StdPicture
    Set ScrollBackGroundPicture = m_ScrollBackGroundPicture
End Property
Public Property Set ScrollBackGroundPicture(ByVal NewValue As StdPicture)
    Set m_ScrollBackGroundPicture = NewValue
    
    PropertyChanged "ScrollBackGroundPicture"
    
    Set ScrollButton.BackGroundPicture = NewValue
    
    Me.Value = Me.Value
End Property

Public Property Get ScrollMouseMovePicture() As StdPicture
    Set ScrollMouseMovePicture = m_ScrollMouseMovePicture
End Property
Public Property Set ScrollMouseMovePicture(ByVal NewValue As StdPicture)
    Set m_ScrollMouseMovePicture = NewValue

    PropertyChanged "ScrollMouseMovePicture"

    Set ScrollButton.MouseMovePicture = NewValue
End Property

Public Property Get ScrollMouseDownPicture() As StdPicture
    Set ScrollMouseDownPicture = m_ScrollMouseDownPicture
End Property
Public Property Set ScrollMouseDownPicture(ByVal NewValue As StdPicture)
    Set m_ScrollMouseDownPicture = NewValue
    
    PropertyChanged "ScrollMouseDownPicture"

    Set ScrollButton.MouseDownPicture = NewValue
End Property

