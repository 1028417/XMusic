VERSION 5.00
Begin VB.Form ShadowForm 
   AutoRedraw      =   -1  'True
   BorderStyle     =   0  'None
   ClientHeight    =   11460
   ClientLeft      =   585
   ClientTop       =   0
   ClientWidth     =   17280
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   ScaleHeight     =   764
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   1152
   ShowInTaskbar   =   0   'False
End
Attribute VB_Name = "ShadowForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Implements ILayeredForm

Private m_Graphics As Long

Private m_MemDC As Long
Private m_PaintDC As Long

Private m_MemBitmap As Long
Private m_PaintBitmap As Long

Private Property Get ILayeredForm_Width() As Long
    ILayeredForm_Width = Me.Width / Screen.TwipsPerPixelX
End Property
Private Property Get ILayeredForm_Height() As Long
    ILayeredForm_Height = Me.Height / Screen.TwipsPerPixelY
End Property

Private Property Get ILayeredForm_Handle() As Long
    ILayeredForm_Handle = Me.hwnd
End Property

Private Property Get ILayeredForm_Hdc() As Long
    ILayeredForm_Hdc = Me.Hdc
End Property

Private Property Get ILayeredForm_MemDC() As Long
    ILayeredForm_MemDC = m_MemDC
End Property
Private Property Let ILayeredForm_MemDC(ByVal RHS As Long)
    m_MemDC = RHS
End Property

Private Property Get ILayeredForm_PaintDC() As Long
    ILayeredForm_PaintDC = m_PaintDC
End Property
Private Property Let ILayeredForm_PaintDC(ByVal RHS As Long)
    m_PaintDC = RHS
End Property

Private Property Let ILayeredForm_MemBitmap(ByVal RHS As Long)
    m_MemBitmap = RHS
End Property
Private Property Get ILayeredForm_MemBitmap() As Long
    ILayeredForm_MemBitmap = m_MemBitmap
End Property

Private Property Let ILayeredForm_PaintBitmap(ByVal RHS As Long)
    m_PaintBitmap = RHS
End Property
Private Property Get ILayeredForm_PaintBitmap() As Long
    ILayeredForm_PaintBitmap = m_PaintBitmap
End Property
