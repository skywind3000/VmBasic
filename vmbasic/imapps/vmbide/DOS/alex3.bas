' �˻ʺ�����ĵݹ�ⷨ

DIM A(8), S, MAX
S = 0: MAX = 0
GOTO MAIN

SEARCH:
A(S) = 0
WHILE A(S) < 8
      OK = 1
      X = 0
      WHILE (X < S)
	    IF (A(X) = A(S)) THEN OK = 0' ������û��ABS����������ʵ��
	    IF (A(X) > A(S)) THEN IF (S - X = A(X) - A(S)) THEN OK = 0
	    IF (A(X) < A(S)) THEN IF (S - X = A(S) - A(X)) THEN OK = 0
	    X = X + 1
      WEND
      IF OK = 1 THEN		' �����ȷ
	 IF S = 7 THEN ' �Ƿ��յ㣿
	    MAX = MAX + 1
	    PRINT "FIND!!"; MAX
	 ELSE           ' �ݹ����
	    S = S + 1
	    GOSUB SEARCH
	    S = S - 1
	 END IF
      END IF
      A(S) = A(S) + 1
WEND
RETURN

MAIN: ' ������
GOSUB SEARCH
PRINT "RESULT="; MAX

