' °Ë»ÊºóµÄº¯Êý°æ

DIM SHARED A(8),MAX

SUB SEARCH(N)
    A(N)=0
    WHILE A(N)<8
          OK=1
	  IF (N>0) THEN
	      Y=0
	      WHILE Y<N 
	            IF (A(N)=A(Y)) THEN OK=-1
		    IF (A(N)-A(Y)=N-Y)OR(A(Y)-A(N)=N-Y) THEN OK=-2
	            Y=Y+1
	      WEND
	  END IF
	  IF OK>0 THEN
	     IF N=7 THEN 
   	        MAX=MAX+1
   	        PRINT "FIND ";MAX
	     ELSE
	        SEARCH(N+1)
	     END IF
	  END IF
          A(N)=A(N)+1
    WEND
END SUB

MAX=0
PRINT "HELLO"
SEARCH(0)
