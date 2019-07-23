VERSION 5.00
Begin VB.Form ShadowForm4 
   AutoRedraw      =   -1  'True
   BorderStyle     =   0  'None
   Caption         =   "Logo"
   ClientHeight    =   9000
   ClientLeft      =   7575
   ClientTop       =   4395
   ClientWidth     =   12000
   LinkTopic       =   "Form1"
   ScaleHeight     =   600
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   800
   ShowInTaskbar   =   0   'False
End
Attribute VB_Name = "ShadowForm4"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Implements CShadowForm

Private m_Graphics As Long
Private m_NewDC As Long

Private m_OldBitmap As Long
Private m_NewBitmap As Long

Private Property Get CShadowForm_ClientHeight() As Long
    CShadowForm_ClientHeight = Me.ScaleHeight
End Property

Private Property Get CShadowForm_ClientWidth() As Long
    CShadowForm_ClientWidth = Me.ScaleWidth
End Property

Private Property Get CShadowForm_Handle() As Long
    CShadowForm_Handle = Me.hwnd
End Property

Private Property Get CShadowForm_NewDC() As Long
    CShadowForm_NewDC = m_NewDC
End Property
Private Property Let CShadowForm_NewDC(ByVal RHS As Long)
    m_NewDC = RHS
End Property

Private Property Let CShadowForm_OldBitmap(ByVal RHS As Long)
    m_OldBitmap = RHS
End Property
Private Property Get CShadowForm_OldBitmap() As Long
    CShadowForm_OldBitmap = m_OldBitmap
End Property

Private Property Let CShadowForm_NewBitmap(ByVal RHS As Long)
    m_NewBitmap = RHS
End Property
Private Property Get CShadowForm_NewBitmap() As Long
    CShadowForm_NewBitmap = m_NewBitmap
End Property

Private Sub Form_Load()
    SetWindowLong hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) Or WS_EX_LAYERED Or WS_EX_TRANSPARENT
    
    SetWindowPos hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE Or SWP_NOSIZE
End Sub

