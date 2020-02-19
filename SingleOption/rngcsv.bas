Attribute VB_Name = "rngcsv"
Option Explicit
Sub rng2csv(rng As Range, fileName As String, Optional sep As String = ";", Optional decimalSign As String)
'export range data to a CSV file, allowing to chose the separator and decimal symbol
'can export using rng number formatting!
'by Patrick Honorez --- www.idevlop.com
    Dim f As Integer, i As Long, c As Long, r
    Dim ar, rowAr, sOut As String
    Dim replaceDecimal As Boolean, oldDec As String

    Dim a As Application:   Set a = Application

    ar = rng
    f = FreeFile()
    Open fileName For Output As #f

    oldDec = Format(0, ".")     'current client's decimal symbol
    replaceDecimal = (decimalSign <> "") And (decimalSign <> oldDec)

    For Each r In rng.Rows
        rowAr = a.Transpose(a.Transpose(r.Value))
        If replaceDecimal Then
            For c = 1 To UBound(rowAr)
                'use isnumber() to avoid cells with numbers formatted as strings
                If a.IsNumber(rowAr(c)) Then
                    'uncomment the next 3 lines to export numbers using source number formatting
'                    If r.cells(1, c).NumberFormat <> "General" Then
'                        rowAr(c) = Format$(rowAr(c), r.cells(1, c).NumberFormat)
'                    End If
                    rowAr(c) = Replace(rowAr(c), oldDec, decimalSign, 1, 1)
                End If
            Next c
        End If
        sOut = Join(rowAr, sep)
        Print #f, sOut
    Next r
    Close #f

End Sub


Sub export_csv()
    Dim strEnd As String
    strEnd = InputBox("xxx_filename?")
    Dim ws_vol As Worksheet
    Set ws_vol = ThisWorkbook.Worksheets("Imvol")
    Dim rng_vol As Range: Set rng_vol = ws_vol.Range("a3:v14")
    Call rng2csv(rng_vol, "vol" & strEnd & ".csv", ",", ".")
    
    Dim ws_rate As Worksheet
    Set ws_rate = ThisWorkbook.Worksheets("rate")
    Dim rng_rate As Range: Set rng_rate = ws_rate.Range("a1:p3")
    Call rng2csv(rng_rate, "rate" & strEnd & ".csv", ",", ".")

    Dim ws_div As Worksheet
    Set ws_div = ThisWorkbook.Worksheets("div")
    Dim rng_div As Range: Set rng_div = ws_div.Range("a1:aq5")
    Call rng2csv(rng_div, "div" & strEnd & ".csv", ",", ".")

End Sub
