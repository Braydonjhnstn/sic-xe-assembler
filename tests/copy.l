Program: COPY
Start Address: 0000
Program Length: 001B

                      EXTDEF  BUFFER, BUFEND, LENGTH
                      EXTREF  RDREC, WRREC
      17202A    FIRST   STL     RETADR
0003  690030            LDB     #LENGTH
0006                  BASE    LENGTH
0006  4B101033  CLOOP   +JSUB   RDREC
000A  032023            LDA     LENGTH
000D  290000            COMP    #0
0010  332007            JEQ     ENDFIL
0013  4B10002C          +JSUB   WRREC
0017  3F2FEC            J       CLOOP
001A  032061    ENDFIL  LDA     =C'EOF'
001D  0F2013            STA     BUFFER
0020  010003            LDA     #3
0023  0F200A            STA     LENGTH
0026  4B10002C          +JSUB   WRREC
002A  3E2000            J       @RETADR
002D          RETADR  RESW    1
0030          LENGTH  RESW    1
0033                  LTORG
0033  454F46    * =C'EOF'
0036          BUFFER  RESB    4096
1036          BUFEND  EQU     *
1036          MAXLEN  EQU     BUFEND-BUFFER
                      END     FIRST
              RDREC   CSECT
                      EXTREF  BUFFER, LENGTH, BUFEND
      B410              CLEAR   X
0002  B400              CLEAR   A
0004  B440              CLEAR   S
0006  75101033          +LDT    #MAXLEN
000A  E3201B    RLOOP   TD      INPUT
000D  332FFA            JEQ     RLOOP
0010  DB2015            RD      INPUT
0013  A004              COMPR   A,S
0015  332009            JEQ     EXIT
0018  57900033          +STCH   BUFFER,X
001C  B850              TIXR    T
001E  3B2FE9            JLT     RLOOP
0021  13100030  EXIT    +STX    LENGTH
0025  4F0000            RSUB
0028  F1        INPUT   BYTE    X'F1'
0029  000000    MAXLEN  WORD    BUFEND-BUFFER
              WRREC   CSECT
                      EXTREF  LENGTH, BUFFER
      B410              CLEAR   X
0002  77100030          +LDT    LENGTH
0006  E32061    WLOOP   TD      =X'05'
0009  332FFA            JEQ     WLOOP
000C  53900033          +LDCH   BUFFER,X
0010  DF2057            WD      =X'05'
0013  B850              TIXR    T
0015  3B2FEE            JLT     WLOOP
0018  4F0000            RSUB
001B  05        * =X'05'
