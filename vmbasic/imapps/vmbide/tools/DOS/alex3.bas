' 八皇后问题的递归解法

DIM A(8), S, MAX
S = 0: MAX = 0
GOTO MAIN

SEARCH:
A(S) = 0
WHILE A(S) < 8
      OK = 1
      X = 0
      WHILE (X < S)
	    IF (A(X) = A(S)) THEN OK = 0' 哈哈还没有ABS靠下面两句实现
	    IF (A(X) > A(S)) THEN IF (S - X = A(X) - A(S)) THEN OK = 0
	    IF (A(X) < A(S)) THEN IF (S - X = A(S) - A(X)) THEN OK = 0
	    X = X + 1
      WEND
      IF OK = 1 THEN		' 如果正确
	 IF S = 7 THEN ' 是否到终点？
	    MAX = MAX + 1
	    PRINT "FIND!!"; MAX
	 ELSE           ' 递归回溯
	    S = S + 1
	    GOSUB SEARCH
	    S = S - 1
	 END IF
      END IF
      A(S) = A(S) + 1
WEND
RETURN

MAIN: ' 主程序
GOSUB SEARCH
PRINT "RESULT="; MAX

