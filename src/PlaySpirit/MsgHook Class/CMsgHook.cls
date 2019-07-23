VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "CMsgHook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Private Declare Function EnumChildWindows Lib "user32" (ByVal HwndParent As Long, ByVal lpEnumFunc As Long, ByRef lParam As Long) As Long

Private Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal Hwnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long

Private Declare Function RegisterHotKey Lib "user32" (ByVal Hwnd As Long, ByVal id As Long, ByVal fsModifiers As Long, ByVal vk As Long) As Long
Private Declare Function UnregisterHotKey Lib "user32" (ByVal Hwnd As Long, ByVal id As Long) As Long

Private Declare Function ChangeClipboardChain Lib "user32" (ByVal Hwnd As Long, ByVal hWndNext As Long) As Long
Private Declare Function SetClipboardViewer Lib "user32" (ByVal Hwnd As Long) As Long

Public Event MsgEvent(ByVal Hwnd&, ByVal Msg&, ByVal Arg1&, ByVal Arg2&, ByVal Arg3&)
Public Event MsgPass(ByVal Hwnd&, ByVal Msg&, ByVal Wp&, ByVal Lp&)

Public Event EnumChildevent(ByVal Hwnd&, ByVal ClassName$)

Public Enabled As Boolean

Public MaxSizeX As Long, MaxPositionX As Long, MinTrackSizeX As Long, MaxTrackSizeX As Long
Public MaxSizeY As Long, MaxPositionY As Long, MinTrackSizeY As Long, MaxTrackSizeY As Long

Dim HotKeyID&, ClipBoardChain&, ClipBoardChainHwnd&

Public PreProc&, Hwnd&

Public Sub CallEnumChildEvent(ByVal Hwnd&, ByVal ClassName$)
    RaiseEvent EnumChildevent(Hwnd, ClassName)
End Sub

Public Function CallMsgEvent(ByVal Hwnd&, ByVal Msg&, Optional ByVal Arg1&, Optional ByVal Arg2&, Optional ByVal Arg3&) As Long
    Me.Enabled = 1
    RaiseEvent MsgEvent(Hwnd, Msg, Arg1, Arg2, Arg3)
    CallMsgEvent = IIf(Me.Enabled, 0, 1)
End Function

Public Sub CallMsgPass(ByVal Hwnd&, ByVal Msg&, ByVal Wp&, ByVal Lp&)
    RaiseEvent MsgPass(Hwnd, Msg, Wp, Lp)
End Sub

Public Sub SetClipBoardHook()
    ClipBoardChain = SetClipboardViewer(Me.Hwnd)
    ClipBoardChainHwnd = Me.Hwnd
End Sub

Public Sub SetHook(ByVal Hwnd As Long)
    Dim i As Integer
    
    If Hwnd Then
        Me.Hwnd = Hwnd
        
        MsgHooks.Add Me

        Me.PreProc = SetWindowLong(Hwnd, (-4), AddressOf MsgProc)

    Else
        If ClipBoardChain <> 0 Then ChangeClipboardChain Me.Hwnd, ClipBoardChain

        SetWindowLong Me.Hwnd, (-4), Me.PreProc
        
        Me.Hwnd = 0
        Me.PreProc = 0
        
        For i = 1 To MsgHooks.Count
            If MsgHooks(i) Is Me Then
                MsgHooks.Remove i
                Exit For
            End If
        Next
    End If
End Sub

Public Sub SetHotkey(vkey&, Optional Modifier&, Optional id& = 520)
    If vkey <> 0 Then
        RegisterHotKey Me.Hwnd, id, Modifier, vkey
        HotKeyID = id
    
    Else
        UnregisterHotKey Me.Hwnd, HotKeyID
        HotKeyID = 0
    End If
End Sub

Public Function GetChildWindow(Hwnd&, Optional ClassName$ = "") As Long
    If ClassName = "" Then
        EnumChildWindows Hwnd, AddressOf EnumChildProc, 0&
    
    Else
        Dim classHwnd&
        classHwnd = 1
        EnumChildWindows Hwnd, AddressOf EnumChildProc, classHwnd
        GetChildWindow = classHwnd
    End If
End Function

Public Property Get WM_ENTERMENULOOP() As Long
WM_ENTERMENULOOP = &H211
End Property

Public Property Get WM_EXITMENULOOP() As Long
WM_EXITMENULOOP = &H212
End Property

Public Property Get SC_HOTKEY() As Long
SC_HOTKEY = &HF150&
End Property
Public Property Get SC_KEYMENU() As Long
SC_KEYMENU = &HF100&
End Property
Public Property Get SC_MOVE() As Long
SC_MOVE = &HF010&
End Property
Public Property Get SC_SIZE() As Long
SC_SIZE = &HF000&
End Property
Public Property Get SC_MINIMIZE() As Long
SC_MINIMIZE = &HF020&
End Property
Public Property Get SC_MAXIMIZE() As Long
SC_MAXIMIZE = &HF030&
End Property
Public Property Get SC_CLOSE() As Long
SC_CLOSE = &HF060&
End Property
Public Property Get SC_RESTORE() As Long
SC_RESTORE = &HF120&
End Property

Public Property Get MK_LBUTTON() As Long
MK_LBUTTON = &H1
End Property
Public Property Get MK_RBUTTON() As Long
MK_RBUTTON = &H2
End Property
Public Property Get MK_MBUTTON() As Long
MK_MBUTTON = &H10
End Property
Public Property Get MK_SHIFT() As Long
MK_SHIFT = &H4
End Property
Public Property Get MK_CONTROL() As Long
MK_CONTROL = &H8
End Property

Public Property Get MOD_ALT() As Long
MOD_ALT = &H1
End Property
Public Property Get MOD_CONTROL() As Long
MOD_CONTROL = &H2
End Property
Public Property Get MOD_SHIFT() As Long
MOD_SHIFT = &H4
End Property

Public Property Get WM_MOVE() As Long
WM_MOVE = &H3
End Property
Public Property Get WM_SIZE() As Long
WM_SIZE = &H5
End Property
Public Property Get WM_SYSCOMMAND() As Long
WM_SYSCOMMAND = &H112
End Property
Public Property Get WM_COMMAND() As Long
WM_COMMAND = &H111
End Property
Public Property Get WM_ACTIVATEAPP() As Long
WM_ACTIVATEAPP = &H1C
End Property
Public Property Get WM_NCACTIVATE() As Long
WM_NCACTIVATE = &H86
End Property
Public Property Get WM_GETMINMAXINFO() As Long '限制大小
WM_GETMINMAXINFO = &H24
End Property
Public Property Get WM_QUERYOPEN() As Long '恢复窗口
WM_QUERYOPEN = &H13
End Property
Public Property Get WM_HOTKEY() As Long
WM_HOTKEY = &H312
End Property
Public Property Get WM_DRAWCLIPBOARD() As Long '剪切板
WM_DRAWCLIPBOARD = &H308
End Property
Public Property Get WM_QUERYENDSESSION() As Long '退出系统
WM_QUERYENDSESSION = &H11
End Property

Public Property Get WM_KEYDOWN() As Long
WM_KEYDOWN = &H100C
End Property
Public Property Get WM_KEYUP() As Long
WM_KEYUP = &H101
End Property

Public Property Get WM_MOUSEMOVE() As Long
WM_MOUSEMOVE = &H200
End Property
Public Property Get WM_LBUTTONDOWN() As Long
WM_LBUTTONDOWN = &H201
End Property
Public Property Get WM_LBUTTONUP() As Long
WM_LBUTTONUP = &H202
End Property
Public Property Get WM_LBUTTONDBLCLK() As Long
WM_LBUTTONDBLCLK = &H203
End Property
Public Property Get WM_RBUTTONDOWN() As Long
WM_RBUTTONDOWN = &H204
End Property
Public Property Get WM_RBUTTONUP() As Long
WM_RBUTTONUP = &H205
End Property
Public Property Get WM_RBUTTONDBLCLK() As Long
WM_RBUTTONDBLCLK = &H206
End Property
Public Property Get WM_MBUTTONDOWN() As Long
WM_MBUTTONDOWN = &H207
End Property
Public Property Get WM_MBUTTONUP() As Long
WM_MBUTTONUP = &H208
End Property
Public Property Get WM_MBUTTONDBLCLK() As Long
WM_MBUTTONDBLCLK = &H209
End Property

Public Property Get WM_NCMOUSEMOVE() As Long
WM_NCMOUSEMOVE = &HA0
End Property
Public Property Get WM_NCLBUTTONDOWN() As Long
WM_NCLBUTTONDOWN = &HA1
End Property
Public Property Get WM_NCLBUTTONUP() As Long
WM_NCLBUTTONUP = &HA2
End Property
Public Property Get WM_NCLBUTTONDBLCLK() As Long
WM_NCLBUTTONDBLCLK = &HA3
End Property
Public Property Get WM_NCRBUTTONDOWN() As Long
WM_NCRBUTTONDOWN = &HA4
End Property
Public Property Get WM_NCRBUTTONUP() As Long
WM_NCRBUTTONUP = &HA5
End Property
Public Property Get WM_NCRBUTTONDBLCLK() As Long
WM_NCRBUTTONDBLCLK = &HA6
End Property
Public Property Get WM_NCMBUTTONDOWN() As Long
WM_NCMBUTTONDOWN = &HA7
End Property
Public Property Get WM_NCMBUTTONUP() As Long
WM_NCMBUTTONUP = &HA8
End Property
Public Property Get WM_NCMBUTTONDBLCLK() As Long
WM_NCMBUTTONDBLCLK = &HA9
End Property
