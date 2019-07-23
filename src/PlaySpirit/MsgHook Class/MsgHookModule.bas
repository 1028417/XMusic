Attribute VB_Name = "MsgHookModule"
Private Type POINTAPI
    X As Long
    Y As Long
End Type

Private Type MINMAXINFO
    ptReserved As POINTAPI
    ptMaxSize As POINTAPI
    ptMaxPosition As POINTAPI
    ptMinTrackSize As POINTAPI
    ptMaxTrackSize As POINTAPI
End Type

Private Declare Sub CopyMemory Lib "kernel32" Alias "RtlMoveMemory" (pDest As Any, pSource As Any, ByVal ByteLen As Long)

Private Type along
    X As Long
End Type

Private Type toint
    X As Integer
    Y As Integer
End Type

Private Declare Function CallWindowProc Lib "user32" Alias "CallWindowProcA" (ByVal lpPrevWndFunc As Long, ByVal Hwnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Private Declare Function GetClassName Lib "user32" Alias "GetClassNameA" (ByVal Hwnd As Long, ByVal lpClassName As String, ByVal nMaxCount As Long) As Long

Public MsgHooks As New Collection

Public Function EnumChildProc(ByVal Hwnd As Long, ByRef lParam As Long) As Long
    Dim i As Integer

    Dim ClassName As String * 64

    GetClassName Hwnd, ClassName, 64

    
    If InStr(ClassName, Chr(0)) > 0 Then
        If lParam = 0 Then
            For i = 1 To MsgHooks.Count
                If MsgHooks(i).Hwnd = Hwnd Then
                    MsgHooks(i).CallEnumChildEvent Hwnd, Left(ClassName, InStr(ClassName, Chr(0)) - 1)
                    Exit For
                End If
            Next

        Else
            lParam = Hwnd
        End If
    End If
    
    'If WinClass = "MacromediaFlashPlayerActiveX" Or Left(WinClass, 4) = "ATL:" Then flashHwnd = Hwnd
    
    EnumChildProc = 1
End Function

Public Function MsgProc(ByVal Hwnd As Long, ByVal Msg As Long, ByVal Wp As Long, ByVal Lp As Long) As Long
    Dim i As Integer
    Dim MsgHook As CMsgHook
    
    Dim a As along, t As toint
    
    On Error Resume Next
    
    a.X = Lp: LSet t = a

    For i = 1 To MsgHooks.Count
        If MsgHooks(i).Hwnd = Hwnd Then
            Set MsgHook = MsgHooks(i)
            Exit For
        End If
    Next
    
    Select Case Msg
        Case MsgHook.WM_MOUSEMOVE, MsgHook.WM_LBUTTONDOWN, MsgHook.WM_LBUTTONUP, MsgHook.WM_LBUTTONDBLCLK, _
        MsgHook.WM_RBUTTONDOWN, MsgHook.WM_RBUTTONUP, MsgHook.WM_RBUTTONDBLCLK, _
        MsgHook.WM_MBUTTONDOWN, MsgHook.WM_MBUTTONUP, MsgHook.WM_MBUTTONDBLCLK, _
        MsgHook.WM_NCMOUSEMOVE, MsgHook.WM_NCLBUTTONDOWN, MsgHook.WM_NCLBUTTONUP, MsgHook.WM_NCLBUTTONDBLCLK, _
        MsgHook.WM_NCRBUTTONDOWN, MsgHook.WM_NCRBUTTONUP, MsgHook.WM_NCRBUTTONDBLCLK, _
        MsgHook.WM_NCMBUTTONDOWN, MsgHook.WM_NCMBUTTONUP, MsgHook.WM_NCMBUTTONDBLCLK, _
        MsgHook.WM_HOTKEY
            MsgProc = MsgHook.CallMsgEvent(Hwnd, Msg, Wp, t.X, t.Y)
            
        Case MsgHook.WM_MOVE, MsgHook.WM_SIZE
            MsgProc = MsgHook.CallMsgEvent(Hwnd, Msg, t.X, t.Y)

        Case MsgHook.WM_KEYDOWN, MsgHook.WM_KEYUP, MsgHook.WM_SYSCOMMAND, MsgHook.WM_COMMAND, _
        MsgHook.WM_ACTIVATEAPP, MsgHook.WM_NCACTIVATE, MsgHook.WM_QUERYOPEN
            MsgProc = MsgHook.CallMsgEvent(Hwnd, Msg, Wp)

        Case MsgHook.WM_DRAWCLIPBOARD, MsgHook.WM_QUERYENDSESSION, MsgHook.WM_ENTERMENULOOP, MsgHook.WM_EXITMENULOOP
            MsgProc = MsgHook.CallMsgEvent(Hwnd, Msg)

        Case MsgHook.WM_GETMINMAXINFO
            Dim MinMax As MINMAXINFO
            CopyMemory MinMax, ByVal Lp, Len(MinMax)
            
            MsgHook.MaxSizeX = MinMax.ptMaxSize.X
            MsgHook.MaxSizeY = MinMax.ptMaxSize.Y
            MsgHook.MaxPositionX = MinMax.ptMaxPosition.X
            MsgHook.MaxPositionY = MinMax.ptMaxPosition.Y
            MsgHook.MinTrackSizeX = MinMax.ptMinTrackSize.X
            MsgHook.MinTrackSizeY = MinMax.ptMinTrackSize.Y
            MsgHook.MaxTrackSizeX = MinMax.ptMaxTrackSize.X
            MsgHook.MaxTrackSizeY = MinMax.ptMaxTrackSize.Y
            
            MsgHook.CallMsgEvent Hwnd, Msg
            
            MinMax.ptMaxSize.X = MsgHook.MaxSizeX
            MinMax.ptMaxSize.Y = MsgHook.MaxSizeY
            MinMax.ptMaxPosition.X = MsgHook.MaxPositionX
            MinMax.ptMaxPosition.Y = MsgHook.MaxPositionY
            MinMax.ptMinTrackSize.X = MsgHook.MinTrackSizeX
            MinMax.ptMinTrackSize.Y = MsgHook.MinTrackSizeY
            MinMax.ptMaxTrackSize.X = MsgHook.MaxTrackSizeX
            MinMax.ptMaxTrackSize.Y = MsgHook.MaxTrackSizeY
            
            CopyMemory ByVal Lp, MinMax, Len(MinMax)
            MsgProc = 1
            Exit Function
            
        Case Else
            MsgProc = MsgHook.CallMsgEvent(Hwnd, Msg, Wp, Lp)
    End Select
    
    If MsgProc = 0 Then
        MsgProc = CallWindowProc(MsgHook.PreProc, Hwnd, Msg, Wp, Lp)
        
        MsgHook.CallMsgPass Hwnd, Msg, Wp, Lp
    End If
End Function
