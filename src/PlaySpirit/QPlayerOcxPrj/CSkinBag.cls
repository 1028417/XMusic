VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "CSkinBag"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
Public Enum SkinItemIndexConstants
    siiMaskShadow1 = 3
    siiMaskShadow2
    siiMaskShadow3
    
    siiPlayButtonLeft
    siiPlayButtonTop

    siiPauseButtonLeft
    siiPauseButtonTop
        
    siiStopButtonLeft
    siiStopButtonTop

    siiNextButtonLeft
    siiNextButtonTop
    
    siiLastButtonLeft
    siiLastButtonTop
    
    siiSoundButtonLeft
    siiSoundButtonTop
    
    siiPlayProgramLeft
    siiPlayProgramTop
    siiPlayProgramScrollable
    siiPlayProgramxMargin
    siiPlayProgramyMargin
    
    siiVolumSliderLeft
    siiVolumSliderTop
    siiVolumSliderScrollable
    siiVolumSliderxMargin
    siiVolumSlideryMargin
    
    siiMonitorVisible
    siiMonitorLeft
    siiMonitorTop
    siiMonitorWidth
    siiMonitorHeight
    
    
    siiBackGroundPicture
    
    siiPlayButtonUnabledPicture
    siiPlayButtonBackGroundPicture
    siiPlayButtonMouseMovePicture
    siiPlayButtonMouseDownPicture

    siiPauseButtonUnabledPicture
    siiPauseButtonBackGroundPicture
    siiPauseButtonMouseMovePicture
    siiPauseButtonMouseDownPicture

    siiStopButtonUnabledPicture
    siiStopButtonBackGroundPicture
    siiStopButtonMouseMovePicture
    siiStopButtonMouseDownPicture

    siiNextButtonUnabledPicture
    siiNextButtonBackGroundPicture
    siiNextButtonMouseMovePicture
    siiNextButtonMouseDownPicture

    siiLastButtonUnabledPicture
    siiLastButtonBackGroundPicture
    siiLastButtonMouseMovePicture
    siiLastButtonMouseDownPicture

    siiSoundButtonUnabledPicture
    siiSoundButtonBackGroundPicture
    siiSoundButtonMouseMovePicture
    siiSoundButtonMouseDownPicture
    siiSoundButtonPopDownPicture

    siiPlayProgramBackGroundPicture
    siiPlayProgramForeGroundPicture
    siiPlayProgramScrollUnabledPicture
    siiPlayProgramScrollBackGroundPicture
    siiPlayProgramScrollMouseMovePicture
    siiPlayProgramScrollMouseDownPicture

    siiVolumSliderBackGroundPicture
    siiVolumSliderForeGroundPicture
    siiVolumSliderScrollUnabledPicture
    siiVolumSliderScrollBackGroundPicture
    siiVolumSliderScrollMouseMovePicture
    siiVolumSliderScrollMouseDownPicture
End Enum


Public Event OnLoadSkin()
Public Event OnSaveSkin()

Private m_PropertyBag As New PropertyBag


Friend Property Get Item(ByVal Index As SkinItemIndexConstants) As Variant
    If IsObject(m_PropertyBag.ReadProperty(CStr(Index))) Then
        Set Item = m_PropertyBag.ReadProperty(CStr(Index))
    Else
        Item = m_PropertyBag.ReadProperty(CStr(Index))
    End If
End Property

Friend Property Let Item(ByVal Index As SkinItemIndexConstants, ByVal Value As Variant)
    m_PropertyBag.WriteProperty CStr(Index), Value
End Property


Public Property Get MaskPicture() As StdPicture
    On Error Resume Next
    Set MaskPicture = Item(siiBackGroundPicture)
End Property

Public Property Get MaskShadow(ByVal Index As Integer) As Variant
    On Error Resume Next
    MaskShadow = Item(Choose(Index, siiMaskShadow1, siiMaskShadow2, siiMaskShadow3))
End Property

Public Sub Load(ByVal buff As Variant)
    m_PropertyBag.Contents = buff
    RaiseEvent OnLoadSkin
End Sub

Public Function Save() As Variant
    Dim prevPropertyBag As PropertyBag
    Set prevPropertyBag = m_PropertyBag
    
    Set m_PropertyBag = New PropertyBag
    RaiseEvent OnSaveSkin
    Save = m_PropertyBag.Contents

    Set m_PropertyBag = prevPropertyBag
End Function


Public Sub LoadFile(ByVal File As String)
    Dim buff As Variant

    Open File For Binary As #1
    Get #1, , buff
    Close

    Load buff
End Sub

Public Sub SaveFile(ByVal File As String)
    Dim buff As Variant
    buff = Save()
    
    Open File For Binary As #1
    Put #1, , buff
    Close
End Sub
