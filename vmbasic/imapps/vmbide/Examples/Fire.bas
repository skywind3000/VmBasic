' ���������Ϸ
' ���ݽṹ˵��������P(200,6)Ϊ�洢����ռ�
'
Dim Shared P(200,6)            ' ����ռ�
Dim Shared FlyX,FlyY           ' �ɻ�����
Dim Shared PicFly,PicEnemy,PicFire   ' ͼƬ��Ϣ
Dim Shared Layer                     ' ��������
Dim Shared FireCount                 ' �ӵ�����

Sub InitBmp()
    Print "����һ��400x300�Ļ�����˫���沢�Ҷ���ͼƬ....";
    Layer    = NewCanvas(400,300)
    PicFly   = LoadCanvas("GAME1.TGA")
    PicEnemy = LoadCanvas("GAME2.TGA")
    PicFire  = LoadCanvas("GAME3.TGA")
    If (Layer<0)Or(PicFly<0)Or(PicEnemy<0)Or(PicFire<0) Then
        Print "����ͼƬ����"
        VASM("EXIT")
    End If
    Print "�ɹ�"
    Print "��ʼ����ɣ������Ϸ�÷�������Ʒɻ��ո�������ӵ�"
    Print
    Print "�س�����ʼ��Ϸ....";
    While Not KeyPress(KEY_ENTER) : WEnd
End Sub

'=================== Game Engine =======================
Declare Sub ClearData()
Declare Function Alloc()
Declare Sub DrawFly(X,Y,F)
Declare Sub Plane()

Dim Shared Star(100,4)

Sub ClearData()     ' ���P��������
    I=0
    While I < 200
          P(I,0)=-1
          I=I+1
    WEnd
    I=0
    While I < 80
          If I<50 Then
             Star(I,0)=2
             Star(I,1)=Random(400)
             Star(I,2)=Random(300)
          Else
             Star(I,0)=3
             Star(I,1)=Random(400)
             Star(I,2)=Random(300)
          End If
          I=I+1
    WEnd
    FlyX=200
    FlyY=180
End Sub

Function Alloc()     ' ��P(0..199)��Ѱ��һ��û���ù��ĵ�ַ
    I=0
    While (I<31)And(P(I,0)>=0) : I=I+1 : WEnd
    Alloc=I
End Function

Sub DrawFly(X,Y,F)
    If F<2 Then      ' �������̺͵л�
       PIC=PicFly
       If F=1 Then PIC=PicEnemy
       BlitCanvas(Layer,PIC,X-16,Y-16,32,32,0,0,&HF81F) ' F81Fh����ɫ͸��ɫ
    Else             ' ���ӵ�
       BlitCanvas(Layer,PicFire,X-7-4,Y,8,8,0,0,&HF81F)
       BlitCanvas(Layer,PicFire,X+7-4,Y,8,8,0,0,&HF81F)
    End If
End Sub

Sub Plane()
    Dim Shared SpaceStatus ' �ո��״̬
    If KeyPress(KEY_UP)   Then FlyY=FlyY-6 : If FlyY<-10 Then FlyY=-10
    If KeyPress(KEY_DOWN) Then FlyY=FlyY+4 : If FlyY>300 Then FlyY=300
    If KeyPress(KEY_LEFT) Then FlyX=FlyX-5 : If FlyX<-10 Then FlyX=-10
    If KeyPress(KEY_RIGHT)Then FlyX=FlyX+5 : If FlyX>410 Then FlyX=410
    DrawFly(FlyX,FlyY,0)
    If KeyPress(KEY_SPACE)=0 Then SpaceStatus=0
    If (SpaceStatus=0)And(KeyPress(KEY_SPACE)) Then
        SpaceStatus=1
        If FireCount<2 Then
           FireCount=FireCount+1
           H=Alloc()
           P(H,0)=0
           P(H,1)=FlyX
           P(H,2)=FlyY-8
        End if
    End If
End Sub

Function CheckHit(X,Y,DX,DY,DIS)
    CheckHit=0
    If (Abs(X-DX)<DIS)And(Abs(Y-DY)<DIS) Then CheckHit=1
End Function
'=================== Main  Engine =======================
Declare Sub GameDriver()  ' ��Ҫ����ͨ��ɨ��P����������Ϸ
Declare Sub GameMain()    ' ��Ϸ����ѭ��
Declare Sub GameObject()  ' ��Ϸ�������

Sub GameMain()
    Dim Shared PLAY
    PLAY=1
    TIME=0
    DELAY=1000/30
    ClearData()
    While PLAY > 0
          While (FTIMER()-TIME<DELAY) : WEnd
          TIME=FTIMER()        ' ����ʱ��ͣ��
          If KeyPress(KEY_ESCAPE) Then PLAY=0
          GameObject()        ' �����ǿպ͵���
          Plane()              ' ������ҷɻ�          
          GameDriver()             ' ��Ҫ��������
          ShowCanvas(Layer)    ' ��ʾ����/����
          FillCanvas(Layer,0,0,400,300,0)
    WEnd
End Sub

Sub GameDriver()
    Dim Shared PLAY
    I=0
    While I<31
          If P(I,0)>=0 Then
             X=P(I,1) : Y=P(I,2)
             If P(I,0)=0 Then  ' �����ӵ�
                DrawFly(X,Y,2)
                J=0
                While J<31
                      If P(J,0)=1 Then
                         If CheckHit(X,Y,P(J,1),P(J,2),24) Then
                            FireCount=FireCount-1
                            P(J,0)=2
                            P(J,3)=0
                            P(I,0)=-1
                            J=200
                         End If
                      End If
                      J=J+1
                WEnd
                Y=Y-10
                If (Y<-10) Then 
                    P(I,0)=-1
                    FireCount=FireCount-1
                End If
             End If
             If P(I,0)=1 Then  ' �������
                DrawFly(X,Y,1)
                If CheckHit(X,Y,FlyX,FlyY,28) Then PLAY=-1
                Y=Y+4
                If Not Random(2) Then Y=Y+3
                If Not Random(1) Then 
                   If X<FlyX Then X=X+3
                   If X>FlyX Then X=X-3
                End If
                If Y>300 Then P(I,0)=-1
             End If
             If P(I,0)=2 Then  ' ������˸����
                J=P(I,3)
                If (J Mod 2)=0 Then DrawFly(X,Y,1)
                J=J+1
                If (J>20) Then P(I,0)=-1
                P(I,3)=J
             End If
             P(I,1)=X : P(I,2)=Y             
          End If
          I=I+1
    WEnd
End Sub

Sub GameObject()
    I=0
    While I < 80
          Y=Star(I,2)
          If I<50 Then
             Pixel(Layer,Star(I,1),Y,&H666666)
             Y=Y+2
          Else
             Pixel(Layer,Star(I,1),Y,&HAAAAAA)
             Y=Y+3
          End If
          If Y>410 Then Star(I,1)=Random(400):Y=-300+Random(300)
          Star(I,2)=Y
          I = I + 1
    WEnd 
    If Not Random(20) Then
       H=Alloc()
       P(H,0)=1
       P(H,1)=Random(400)
       P(H,2)=-30
    End If
End Sub
'=================== Main Program =======================
InitBmp()

GameMain()
CloseGraph()
Print
If Play<0 Then Print "������ķɻ��������� ";
Print "��л���� :-)"
