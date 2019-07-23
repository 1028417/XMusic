VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "ILayeredForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Public Property Get Handle() As Long

End Property
Public Property Get Hdc() As Long

End Property

Public Property Get Width() As Long

End Property
Public Property Get Height() As Long

End Property

Public Property Get MemDC() As Long

End Property
Public Property Let MemDC(ByVal Value As Long)

End Property

Public Property Get PaintDC() As Long

End Property
Public Property Let PaintDC(ByVal Value As Long)

End Property

Public Property Get MemBitmap() As Long

End Property
Public Property Let MemBitmap(ByVal Value As Long)

End Property

Public Property Get PaintBitmap() As Long

End Property
Public Property Let PaintBitmap(ByVal Value As Long)

End Property

