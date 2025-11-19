0000    COPY    START   0
0000            EXTDEF  BUFFER, BUFEND, LENGTH
0000            EXTREF  RDREC, WRREC
0000    FIRST   STL     RETADR                  17202A
0003            LDB     #LENGTH                 690030
0006            BASE    LENGTH
0006    CLOOP   +JSUB   RDREC                   4B101033
000A            LDA     LENGTH                  032023
000D            COMP    #0                      290000
0010            JEQ     ENDFIL                  332007
0013            +JSUB   WRREC                   4B10002C
0017            J       CLOOP                   3F2FEC
001A    ENDFIL  LDA     =C'EOF'                 032061
001D            STA     BUFFER                  0F2013
0020            LDA     #3                      010003
0023            STA     LENGTH                  0F200A
0026            +JSUB   WRREC                   4B10002C
002A            J       @RETADR                 3E2000
002D    RETADR  RESW    1
0030    LENGTH  RESW    1
0033            LTORG
0033    * =C'EOF'                               454F46
0036    BUFFER  RESB    4096
1036    BUFEND  EQU     *
1036    MAXLEN  EQU     BUFEND-BUFFER
                         END     FIRST
0000    RDREC   CSECT
0000            EXTREF  BUFFER, LENGTH, BUFEND
0000            CLEAR   X                       B410
0002            CLEAR   A                       B400
0004            CLEAR   S                       B440
0006            +LDT    #MAXLEN                 75101033
000A    RLOOP   TD      INPUT                   E3201B
000D            JEQ     RLOOP                   332FFA
0010            RD      INPUT                   DB2015
0013            COMPR   A,S                     A004
0015            JEQ     EXIT                    332009
0018            +STCH   BUFFER,X                57900033
001C            TIXR    T                       B850
001E            JLT     RLOOP                   3B2FE9
0021    EXIT    +STX    LENGTH                  13100030
0025            RSUB                            4F0000
0028    INPUT   BYTE    X'F1'                   F1
0029    MAXLEN  WORD    BUFEND-BUFFER           000000
0000    WRREC   CSECT
0000            EXTREF  LENGTH, BUFFER
0000            CLEAR   X                       B410
0002            +LDT    LENGTH                  77100030
0006    WLOOP   TD      =X'05'                  E32061
0009            JEQ     WLOOP                   332FFA
000C            +LDCH   BUFFER,X                53900033
0010            WD      =X'05'                  DF2057
0013            TIXR    T                       B850
0015            JLT     WLOOP                   3B2FEE
0018            RSUB                            4F0000
