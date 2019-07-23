Attribute VB_Name = "Layer"
Option Explicit

Private Const ULW_OPAQUE = &H4
Private Const ULW_COLORKEY = &H1
Private Const ULW_ALPHA = &H2

Private Const BI_RGB As Long = 0&
Private Const DIB_RGB_COLORS As Long = 0
Private Const AC_SRC_ALPHA As Long = &H1
Private Const AC_SRC_OVER = &H0

Private Type BLENDFUNCTION
    BlendOp As Byte
    BlendFlags As Byte
    SourceConstantAlpha As Byte
    AlphaFormat As Byte
End Type

Private Type Size
    cx As Long
    cy As Long
End Type

Private Type POINTAPI
    X As Long
    Y As Long
End Type

Private Type RGBQUAD
    rgbBlue As Byte
    rgbGreen As Byte
    rgbRed As Byte
    rgbReserved As Byte
End Type

Private Type BITMAPINFOHEADER
    biSize As Long
    biWidth As Long
    biHeight As Long
    biPlanes As Integer
    biBitCount As Integer
    biCompression As Long
    biSizeImage As Long
    biXPelsPerMeter As Long
    biYPelsPerMeter As Long
    biClrUsed As Long
    biClrImportant As Long
End Type

Private Type BITMAPINFO
    bmiHeader As BITMAPINFOHEADER
    bmiColors As RGBQUAD
End Type

Private Type GdiplusStartupInput
    GdiplusVersion As Long
    DebugEventCallback As Long
    SuppressBackgroundThread As Long
    SuppressExternalCodecs As Long
End Type

Private Enum GpStatus
    publicOk = 0
    publicGenericError = 1
    publicInvalidParameter = 2
    publicOutOfMemory = 3
    publicObjectBusy = 4
    publicInsufficientBuffer = 5
    publicNotImplemented = 6
    publicWin32Error = 7
    publicWrongState = 8
    publicAborted = 9
    publicFileNotFound = 10
    publicValueOverflow = 11
    publicAccessDenied = 12
    publicUnknownImageFormat = 13
    publicFontFamilyNotFound = 14
    publicFontStyleNotFound = 15
    publicNotTrueTypeFont = 16
    publicUnsupportedGdiplusVersion = 17
    publicGdiplusNotInitialized = 18
    publicPropertyNotFound = 19
    publicPropertyNotSupported = 20
End Enum

Private Declare Function BitBlt Lib "gdi32.dll" (ByVal hDestDC As Long, ByVal X As Long, ByVal Y As Long, ByVal nWidth As Long, ByVal nHeight As Long, ByVal hSrcDC As Long, ByVal xSrc As Long, ByVal ySrc As Long, ByVal dwRop As Long) As Long
Private Declare Function StretchBlt Lib "gdi32" (ByVal Hdc As Long, ByVal X As Long, ByVal Y As Long, ByVal nWidth As Long, ByVal nHeight As Long, ByVal hSrcDC As Long, ByVal xSrc As Long, ByVal ySrc As Long, ByVal nSrcWidth As Long, ByVal nSrcHeight As Long, ByVal dwRop As Long) As Long

Private Declare Function GdiplusStartup Lib "GdiPlus" (token As Long, inputbuf As GdiplusStartupInput, Optional ByVal outputbuf As Long = 0) As GpStatus
Private Declare Sub GdiplusShutdown Lib "GdiPlus" (ByVal token As Long)

Private Declare Function DeleteObject Lib "gdi32" (ByVal hObject As Long) As Long
Private Declare Function UpdateLayeredWindow Lib "user32.dll" (ByVal Hwnd As Long, ByVal hdcDst As Long, pptDst As Any, psize As Any, ByVal hdcSrc As Long, pptSrc As Any, ByVal crKey As Long, ByRef pblend As BLENDFUNCTION, ByVal dwFlags As Long) As Long
Private Declare Function CreateDIBSection Lib "gdi32.dll" (ByVal Hdc As Long, pBitmapInfo As BITMAPINFO, ByVal un As Long, ByRef lplpVoid As Any, ByVal Handle As Long, ByVal dw As Long) As Long
Private Declare Function CreateCompatibleDC Lib "gdi32.dll" (ByVal Hdc As Long) As Long
Private Declare Function SelectObject Lib "gdi32.dll" (ByVal Hdc As Long, ByVal hObject As Long) As Long
Private Declare Function DeleteDC Lib "gdi32.dll" (ByVal Hdc As Long) As Long

Private Declare Function GdipCreateFromHDC Lib "GdiPlus" (ByVal Hdc As Long, Graphics As Long) As GpStatus
Private Declare Function GdipDeleteGraphics Lib "GdiPlus" (ByVal Graphics As Long) As GpStatus
Private Declare Function GdipReleaseDC Lib "GdiPlus" (ByVal Graphics As Long, ByVal Hdc As Long) As GpStatus
Private Declare Function GdipDrawImageRect Lib "GdiPlus" (ByVal Graphics As Long, ByVal image As Long, ByVal X As Single, ByVal Y As Single, ByVal Width As Single, ByVal Height As Single) As GpStatus
Private Declare Function GdipGetImageWidth Lib "GdiPlus" (ByVal image As Long, Width As Long) As GpStatus
Private Declare Function GdipGetImageHeight Lib "GdiPlus" (ByVal image As Long, Height As Long) As GpStatus
Private Declare Function GdipDisposeImage Lib "GdiPlus" (ByVal image As Long) As GpStatus

Private Declare Function GdipLoadImageFromStream Lib "GdiPlus" (ByVal Stream As Any, ByRef image As Long) As Long
Private Declare Sub CreateStreamOnHGlobal Lib "ole32.dll" (ByRef hGlobal As Any, ByVal fDeleteOnRelease As Long, ByRef ppstm As Any)

Dim blendFunc32bpp As BLENDFUNCTION

Public Function StartToken() As Long
    Dim GpInput As GdiplusStartupInput
    Dim hGdiplusToken As Long
    
    GpInput.GdiplusVersion = 1
    GdiplusStartup hGdiplusToken, GpInput
     
    StartToken = hGdiplusToken
    

    With blendFunc32bpp
      .AlphaFormat = AC_SRC_ALPHA
      .BlendFlags = 0
      .BlendOp = AC_SRC_OVER
      .SourceConstantAlpha = 255
    End With
End Function

Public Sub StopToken(ByVal hGdiplusToken As Long)
    GdiplusShutdown hGdiplusToken
End Sub

Public Function SetLayer(frm As ILayeredForm, Optional ByRef PngArr As Variant) As Boolean
    Dim Png() As Byte

    Dim hStream As Long
    Dim img As Long

    Dim lngWidth As Long, lngHeight As Long
    
    Png = PngArr

    CreateStreamOnHGlobal Png(0), False, hStream
    GdipLoadImageFromStream hStream, img

    GdipGetImageWidth img, lngWidth
    GdipGetImageHeight img, lngHeight
    
    
    Dim tempBI As BITMAPINFO
    With tempBI.bmiHeader
      .biSize = Len(tempBI.bmiHeader)
      .biBitCount = 32
      .biWidth = lngWidth
      .biHeight = lngHeight
      .biPlanes = 1
      .biSizeImage = .biWidth * .biHeight * (.biBitCount / 8)
    End With
    
    frm.MemDC = CreateCompatibleDC(frm.Hdc)
    frm.MemBitmap = CreateDIBSection(frm.MemDC, tempBI, DIB_RGB_COLORS, ByVal 0, 0, 0)
    SelectObject frm.MemDC, frm.MemBitmap
    
    frm.PaintDC = CreateCompatibleDC(frm.Hdc)
    frm.PaintBitmap = CreateDIBSection(frm.PaintDC, tempBI, DIB_RGB_COLORS, ByVal 0, 0, 0)
    SelectObject frm.PaintDC, frm.PaintBitmap


    Dim Graphics As Long
    
    GdipCreateFromHDC frm.MemDC, Graphics
    GdipDrawImageRect Graphics, img, 0, 0, lngWidth, lngHeight

    GdipDeleteGraphics Graphics
    GdipDisposeImage img
End Function

Public Function UpdateLayer(frm As ILayeredForm, ByVal bStretch As Boolean) As Boolean
    Dim srcPoint As POINTAPI
    
    Dim winSize As Size
    winSize.cx = frm.Width
    winSize.cy = frm.Height

    If bStretch Then
        Dim WCape As Long
        WCape = 40

        Const nSize As Integer = 400

        If frm.Width \ 2 < WCape Then
            WCape = frm.Width \ 2
        End If
        
        If frm.Height \ 2 < WCape Then
            WCape = frm.Height \ 2
        End If
        
        StretchBlt frm.PaintDC, WCape, 0, frm.Width - WCape * 2, WCape, frm.MemDC, WCape, 0, 10, WCape, vbSrcCopy
        StretchBlt frm.PaintDC, 0, 0, frm.Width, WCape, frm.MemDC, WCape, 0, 10, WCape, vbSrcCopy
        StretchBlt frm.PaintDC, WCape, frm.Height - WCape, frm.Width - WCape * 2, WCape, frm.MemDC, WCape, nSize - WCape, 10, WCape, vbSrcCopy
        StretchBlt frm.PaintDC, 0, WCape, WCape, frm.Height - WCape * 2, frm.MemDC, 0, WCape, WCape, 10, vbSrcCopy
        StretchBlt frm.PaintDC, frm.Width - WCape, WCape, WCape, frm.Height - WCape * 2, frm.MemDC, nSize - WCape, WCape, WCape, 10, vbSrcCopy

        BitBlt frm.PaintDC, 0, 0, WCape, WCape, frm.MemDC, 0, 0, vbSrcCopy
        BitBlt frm.PaintDC, frm.Width - WCape, 0, WCape, WCape, frm.MemDC, nSize - WCape, 0, vbSrcCopy
        BitBlt frm.PaintDC, 0, frm.Height - WCape, WCape, WCape, frm.MemDC, 0, nSize - WCape, vbSrcCopy
        BitBlt frm.PaintDC, frm.Width - WCape, frm.Height - WCape, WCape, WCape, frm.MemDC, nSize - WCape, nSize - WCape, vbSrcCopy

        StretchBlt frm.PaintDC, WCape, WCape, frm.Width - WCape * 2, frm.Height - WCape * 2, frm.MemDC, WCape, WCape, 10, 10, vbSrcCopy

    Else
        StretchBlt frm.PaintDC, 0, 0, frm.Width, frm.Height, frm.MemDC, 0, 0, frm.Width, frm.Height, vbSrcCopy
    End If

    UpdateLayeredWindow frm.Handle, 0, ByVal 0&, winSize, frm.PaintDC, srcPoint, 0, blendFunc32bpp, ULW_ALPHA
End Function

Public Sub DelTrans(frm As ILayeredForm)
    DeleteObject frm.MemBitmap
    frm.MemBitmap = 0

    DeleteDC frm.MemDC
    frm.MemDC = 0

    DeleteObject frm.PaintBitmap
    frm.PaintBitmap = 0

    DeleteDC frm.PaintDC
    frm.PaintDC = 0
End Sub
