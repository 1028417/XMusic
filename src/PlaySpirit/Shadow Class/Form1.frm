VERSION 5.00
Begin VB.Form Form1 
   BorderStyle     =   0  'None
   Caption         =   "Form1"
   ClientHeight    =   7245
   ClientLeft      =   -60
   ClientTop       =   -30
   ClientWidth     =   7110
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   7245
   ScaleWidth      =   7110
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  '´°¿ÚÈ±Ê¡
   Begin VB.CommandButton Command1 
      Caption         =   "Command1"
      Height          =   495
      Left            =   3000
      TabIndex        =   0
      Top             =   3360
      Width           =   1215
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim t As New CShadow

Private Sub s()
Dim arr1() As Byte
Dim arr2() As Byte
Dim arr3() As Byte

Open App.Path & "\Shadow_10.png" For Binary As #1
ReDim arr1(LOF(1) - 1)
Get #1, , arr1
Close

Open App.Path & "\Shadow_20.png" For Binary As #1
ReDim arr2(LOF(1) - 1)
Get #1, , arr2
Close

Open App.Path & "\Shadow_30.png" For Binary As #1
ReDim arr3(LOF(1) - 1)
Get #1, , arr3
Close

t.SetShadow Me, arr1, arr2, arr3
t.Visible = 1
End Sub

Private Sub Command1_Click()
t.Thickness = 3
End Sub

Private Sub Form_Click()
t.UnsetShadow

Static i
i = i + 1
If i Mod 2 = 1 Then
t.SetShadow Me
Else
s
End If
End Sub

Private Sub Form_Load()
Me.Show
End Sub

Private Sub Form_Unload(Cancel As Integer)
t.UnsetShadow
Set t = Nothing
End Sub

