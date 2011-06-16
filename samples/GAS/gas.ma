[top]
components : gas

[gas]
type : cell
width : 15
height : 15
delay : transport
defaultDelayTime  : 100
border : nowrapped 
neighbors :             life(-1,0)
neighbors : life(0,-1)  life(0,0)  life(0,1)
neighbors :             life(1,0)
initialvalue : 0
initialrow :  4      0  0  0  0  0  2  0  0  0  0  0  0  0  0  0
initialrow :  5      0  0  0  0  3  5  1  0  0  0  0  0  0  0  0
initialrow :  6      0  0  0  0  0  1  0  0  0  0  0  0  0  0  0
initialrow :  11     0  0  0  0  0  0  0  0  0  5  4  0  0  0  0
initialrow :  12     0  0  0  0  0  0  0  0  0 10  4  3  0  0  0
localtransition : gas-rule

[gas-rule]
rule : 10 100 { IF( (-1,0) > 11 or ((-1,0) > 3 and (-1,0) < 8), 4, 0 ) = 4 and
                IF( Odd( (1,0) ), 1, 0 ) = 1 and
                IF( (0,1) >= 8, 8, 0 ) = 0 and
                IF( (0,-1) = 2 or (0,-1) = 3 or (0,-1) = 6 or (0,-1) = 7 or (0,-1) = 10 or (0,-1) = 11 or (0,-1) = 14 or (0,-1) = 15, 2, 0 ) = 0 }
rule :  5 100 { IF( (0,1) >= 8, 8, 0 ) = 8 and
                IF( (0,-1) = 2 or (0,-1) = 3 or (0,-1) = 6 or (0,-1) = 7 or (0,-1) = 10 or (0,-1) = 11 or (0,-1) = 14 or (0,-1) = 15, 2, 0 ) = 2 and
                IF( (-1,0) > 11 or ((-1,0) > 3 and (-1,0) < 8), 4, 0 ) = 0 and
                IF( Odd( (1,0) ), 1, 0 ) = 0 }
rule : { IF( (-1,0) > 11 or ((-1,0) > 3 and (-1,0) < 8), 4, 0 ) +
         IF( Odd( (1,0) ), 1, 0 ) +
         IF( (0,-1) = 2 or (0,-1) = 3 or (0,-1) = 6 or (0,-1) = 7 or (0,-1) = 10 or (0,-1) = 11 or (0,-1) = 14 or (0,-1) = 15, 2, 0 ) +
         IF( (0,1) >= 8, 8, 0 ) }  100  { t }

[comentarios]
# north = IF( (-1,0) > 11 or ((-1,0) > 3 and (-1,0) < 8), 4, 0 )
# south = IF( Odd( (1,0) ), 1, 0 )
# east  = IF( (0,1) >= 8, 8, 0 )
# west  = IF( (0,-1) = 2 or (0,-1) = 3 or (0,-1) = 6 or (0,-1) = 7 or (0,-1) = 10 or (0,-1) = 11 or (0,-1) = 14 or (0,-1) = 15, 2, 0 )