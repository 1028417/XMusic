VERSION 5.00
Begin VB.UserControl QButton 
   AutoRedraw      =   -1  'True
   BackColor       =   &H80000010&
   ClientHeight    =   465
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   555
   ControlContainer=   -1  'True
   ScaleHeight     =   465
   ScaleWidth      =   555
End
Attribute VB_Name = "QButton"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Private Declare Function SetCapture Lib "user32" (ByVal Hwnd As Long) As Long
Private Declare Function ReleaseCapture Lib "user32" () As Long

Public Event Click()

Public Event MouseDown(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)
Public Event MouseUp(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)
Public Event MouseMove(ByVal Button As Integer, ByVal Shift As Integer, ByVal X As Single, ByVal Y As Single)

Dim m_Text As String

Dim m_BackGroundPicture As StdPicture
Dim m_MouseMovePicture As StdPicture
Dim m_MouseDownPicture As StdPicture
Dim m_UnabledPicture As StdPicture
Dim m_PopDownPicture As StdPicture

Dim m_PowerButton As Integer

Dim m_Value As Boolean

Dim m_Enable As Boolean

Dim m_AutoSize As Boolean

Public HotKey$

Dim FlagButtonDown As Integer

Private Sub PrintText()
    If Text = "" Then Exit Sub
    
    UserControl.CurrentX = (UserControl.ScaleWidth - UserControl.TextWidth(Replace(Text, "&", ""))) / 2
    UserControl.CurrentY = (UserControl.ScaleHeight - UserControl.TextHeight(Replace(Text, "&", ""))) / 2
    
    ta = Split(Text, "&")
    UserControl.Print ta(0);
    
    If UBound(ta) > 0 Then
        UserControl.Font.Underline = 1
        Print Left(ta(1), 1);
        
        UserControl.Font.Underline = 0
        Print Mid(ta(1), 2);
    End If
End Sub

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

Public Property Get Value() As Boolean
    Value = m_Value
End Property
Public Property Let Value(ByVal NewValue As Boolean)
    m_Value = NewValue
    
    If NewValue Then
        If Not m_PopDownPicture Is Nothing Then
            UserControl.PaintPicture m_PopDownPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
        ElseIf Not m_MouseDownPicture Is Nothing Then
            UserControl.PaintPicture m_MouseDownPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
        End If
        
    ElseIf Not m_BackGroundPicture Is Nothing Then
        UserControl.PaintPicture m_BackGroundPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
    End If

    PrintText
End Property

Public Property Get Enable() As Boolean
    Enable = m_Enable
End Property
Public Property Let Enable(ByVal NewValue As Boolean)
    m_Enable = NewValue
    
    If m_Enable Then
        If Not m_BackGroundPicture Is Nothing Then
            UserControl.PaintPicture m_BackGroundPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
        Else
            UserControl.Cls
        End If
        
    Else
        If Not m_UnabledPicture Is Nothing Then UserControl.PaintPicture m_UnabledPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
    End If
    
    PrintText
End Property

Public Property Get ForeColor() As OLE_COLOR
    ForeColor = UserControl.ForeColor
End Property
Public Property Let ForeColor(ByVal NewValue As OLE_COLOR)
    UserControl.ForeColor = NewValue

    PrintText
End Property

Public Property Get PowerButton() As Integer
    PowerButton = m_PowerButton
End Property
Public Property Let PowerButton(ByVal NewValue As Integer)
    If NewValue <> 1 And NewValue <> 2 And NewValue <> 4 Then
        Exit Property
    End If
    
    m_PowerButton = NewValue
End Property

Public Property Get PopDownPicture() As StdPicture
    Set PopDownPicture = m_PopDownPicture
End Property
Public Property Set PopDownPicture(ByVal NewValue As StdPicture)
    Set m_PopDownPicture = NewValue
    Me.Value = Me.Value
End Property

Public Property Get UnabledPicture() As StdPicture
    Set UnabledPicture = m_UnabledPicture
End Property
Public Property Set UnabledPicture(ByVal NewValue As StdPicture)
    Set m_UnabledPicture = NewValue
    Me.Enable = Me.Enable
End Property

Public Property Get BackGroundPicture() As StdPicture
    Set BackGroundPicture = m_BackGroundPicture
    Me.Enable = m_Enable
End Property
Public Property Set BackGroundPicture(ByVal NewValue As StdPicture)
    Set m_BackGroundPicture = NewValue

    If m_BackGroundPicture Is Nothing Then
        UserControl.Cls
        Set UserControl.Picture = Nothing
    
    Else
        If Me.AutoSize Then
            UserControl.Width = ScaleX(NewValue.Width, 8, vbTwips)
            UserControl.Height = ScaleY(NewValue.Height, 8, vbTwips)
        End If
        
        If Me.Enable Then
            UserControl.PaintPicture NewValue, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
            PrintText
        End If
    End If
    
    PropertyChanged "BackGroundPicture"
End Property

Public Property Get MouseMovePicture() As StdPicture
    Set MouseMovePicture = m_MouseMovePicture
End Property
Public Property Set MouseMovePicture(ByVal NewValue As StdPicture)
    Set m_MouseMovePicture = NewValue

    PropertyChanged "MouseMovePicture"
End Property

Public Property Get MouseDownPicture() As StdPicture
    Set MouseDownPicture = m_MouseDownPicture
End Property
Public Property Set MouseDownPicture(ByVal NewValue As StdPicture)
    Set m_MouseDownPicture = NewValue
    
    PropertyChanged "MouseDownPicture"
End Property

Public Property Get Text() As String
    Text = m_Text
End Property
Public Property Let Text(ByVal NewValue As String)
    If NewValue <> "" Then
        ta = Split(NewValue, "&")
        NewValue = ta(0)
        
        If UBound(ta) > 0 Then
            NewValue = NewValue & "&"
            HotKey = Left(ta(1), 1)
            
            For ti% = 1 To UBound(ta)
                NewValue = NewValue & ta(ti)
            Next
        End If
    End If
    
    m_Text = NewValue
    If Not BackGroundPicture Is Nothing Then UserControl.PaintPicture BackGroundPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
    
    PrintText
    
    PropertyChanged "Text"
End Property

Private Sub UserControl_Initialize()
    m_AutoSize = 1
    
    m_Enable = 1

    m_PowerButton = 1
End Sub

Private Sub usercontrol_writeproperties(propbag As PropertyBag)
    With propbag
        .WriteProperty "BackGroundPicture", Me.BackGroundPicture
        .WriteProperty "MouseMovePicture", Me.MouseMovePicture
        .WriteProperty "MouseDownPicture", Me.MouseDownPicture
        .WriteProperty "UnabledPicture", Me.UnabledPicture
        .WriteProperty "PopDownPicture", Me.PopDownPicture
        
        .WriteProperty "Button", Me.PowerButton
        .WriteProperty "Text", Me.Text
        .WriteProperty "fcolor", UserControl.ForeColor
        .WriteProperty "value", Me.Value
        .WriteProperty "Enable", Me.Enable
        .WriteProperty "AutoSize", Me.AutoSize
    End With
End Sub

Private Sub usercontrol_readproperties(propbag As PropertyBag)
    With propbag
        Set BackGroundPicture = .ReadProperty("BackGroundPicture", Nothing)
        Set MouseMovePicture = .ReadProperty("MouseMovePicture", Nothing)
        Set MouseDownPicture = .ReadProperty("MouseDownPicture", Nothing)
        Set m_UnabledPicture = .ReadProperty("UnabledPicture", Nothing)
        Set m_PopDownPicture = .ReadProperty("PopDownPicture", Nothing)
        
        Me.PowerButton = .ReadProperty("Button", 1)
        
        Me.Text = .ReadProperty("Text", "")
        
        UserControl.ForeColor = .ReadProperty("fcolor", vbBlack)
        
        Me.Value = .ReadProperty("Value", 0)
        
        Me.Enable = .ReadProperty("Enable", 1)
        
        Me.AutoSize = .ReadProperty("AutoSize", 1)
    End With
End Sub

Private Sub UserControl_Resize()
    UserControl_Paint
End Sub

Private Sub UserControl_Paint()
    If Not Me.Enable And Not m_UnabledPicture Is Nothing Then
        UserControl.PaintPicture m_UnabledPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
    ElseIf Not BackGroundPicture Is Nothing Then
        UserControl.PaintPicture BackGroundPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
    End If

    PrintText
    UserControl.Refresh
End Sub

Private Sub UserControl_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    RaiseEvent MouseMove(Button, Shift, X, Y)
    
    If m_Enable Then
        If X < 0 Or Y < 0 Or X > UserControl.ScaleWidth Or Y > UserControl.ScaleHeight Then
            If Button <> m_PowerButton Then
                ReleaseCapture
            End If
            
            If Not Value Then
                If Not BackGroundPicture Is Nothing Then UserControl.PaintPicture BackGroundPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
                PrintText
            End If
            
        Else
            SetCapture UserControl.Hwnd
            
            If Not Value Then
                If Button = m_PowerButton Then
                    If Not MouseDownPicture Is Nothing Then
                        UserControl.PaintPicture MouseDownPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
                    End If
                    
                Else
                    If Not MouseMovePicture Is Nothing Then UserControl.PaintPicture MouseMovePicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
                End If
                
                PrintText
            End If
        End If
    End If
End Sub

Private Sub UserControl_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    RaiseEvent MouseDown(Button, Shift, X, Y)
    
    If m_Enable Then
        If Not Value Then
            If Not MouseDownPicture Is Nothing And Button = m_PowerButton Then UserControl.PaintPicture MouseDownPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
            PrintText
        End If
    End If
End Sub

Private Sub UserControl_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    RaiseEvent MouseUp(Button, Shift, X, Y)
    
    If m_Enable Then
        If Not Value Then
            If Not BackGroundPicture Is Nothing Then UserControl.PaintPicture BackGroundPicture, 0, 0, UserControl.ScaleWidth, UserControl.ScaleHeight
            PrintText
        End If
        
        If Button = PowerButton Then
            If X >= 0 And X <= UserControl.ScaleWidth And Y >= 0 And Y <= UserControl.ScaleHeight Then
                RaiseEvent Click
            End If
        End If
    End If
End Sub
