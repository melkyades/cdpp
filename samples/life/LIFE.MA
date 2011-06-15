[top]
components : life

[life]
type : cell
width : 20
height : 20
delay : transport
defaultDelayTime  : 100
border : wrapped 
neighbors : life(-1,-1) life(-1,0) life(-1,1) 
neighbors : life(0,-1)  life(0,0)  life(0,1)
neighbors : life(1,-1)  life(1,0)  life(1,1)
initialvalue : 0
initialrowvalue :  0      01111000111100011110
initialrowvalue :  1      00010001111000000000
initialrowvalue :  2      00110111100010111100
initialrowvalue :  3      00110000011110000010
initialrowvalue :  4      00101111000111100011
initialrowvalue : 10      01111000111100011110
initialrowvalue : 11      00010001111000000000
initialrowvalue : 12      00110111100010111100
initialrowvalue : 13      00110000011110000010
initialrowvalue : 14      00101111000111100011
localtransition : life-rule

[life-rule]
rule : 1 100 { (0,0) = 1 and (truecount = 3 or truecount = 4) } 
rule : 1 100 { (0,0) = 0 and truecount = 3 } 
rule : 0 100 { t } 
