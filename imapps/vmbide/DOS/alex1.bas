' 产生非波拉数列

DIM A(10)
A(0) = 1
A(1) = 1
X = 2
LABEL:
A(X) = A(X - 1) + A(X - 2)
X = X + 1
IF (X < 10) THEN GOTO LABEL
X = 0
LOOP2:
PRINT "A["; X; "]="; A(X)
X = X + 1
IF (X < 10) THEN GOTO LOOP2
END

