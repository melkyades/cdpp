[top]
components : pinball

[Pinball]
type : cell
width : 19
height : 19
delay : transport
defaultDelayTime  : 100
border : wrapped 
neighbors : life(-1,-1) life(-1,0) life(-1,1) 
neighbors : life(0,-1)  life(0,0)  life(0,1)
neighbors : life(1,-1)  life(1,0)  life(1,1)
initialvalue : 0
initialrowvalue :  0      0000099999999990000 
initialrowvalue :  1      0000990000000099000 
initialrowvalue :  2      0009900000000009900 
initialrowvalue :  3      0099000000000000990 
initialrowvalue :  4      0990000000000000099 
initialrowvalue :  5      9900000009999000099 
initialrowvalue :  6      9000000000999900009 
initialrowvalue :  7      9000000000099990009 
initialrowvalue :  8      9000090000000000009 
initialrowvalue :  9      9000990000000099009 
initialrowvalue :  10     9000090000000099909 
initialrowvalue :  11     9000000000000099009 
initialrowvalue :  12     9000000009999000009 
initialrowvalue :  13     9000000099999000009 
initialrowvalue :  14     9900000000999000009 
initialrowvalue :  15     0990000000000000099 
initialrowvalue :  16     0099000000000000990 
initialrowvalue :  17     0009900029900099900
initialrowvalue :  18     0000999999999999000 
localtransition : pinball-rule

[pinball-rule]
rule : 9 100 { (0,0) = 9 } 

rule : 1 100 { (0,0) = 0 and (1,1) = 1 }
rule : 4 100 { (0,0) = 0 and (0,-1) = 9 and (1,0) = 1 }
rule : 7 100 { (0,0) = 0 and (-1,0) = 9 and (-1,1) = 9 and (0,1) = 1 }
rule : 5 100 { (0,0) = 1 and (-1,0) = 9 and (0,-1) = 9 }

rule : 2 100 { (0,0) = 0 and (1,0) = 2 }
rule : 5 100 { (0,0) = 0 and (0,-1) = 2 and (-1,-1) = 9 }
rule : 7 100 { (0,0) = 2 and (-1,0) = 9 and (0,1) = 9 }

rule : 3 100 { (1,-1) = 3 and (0,0) = 0 }
rule : 6 100 { (0,0) = 0 and (-1,0) = 9 and (0,-1) = 3 }
rule : 1 100 { (0,0) = 0 and (1,0) = 3 and (0,1) = 9 and (1,1) = 9 }
rule : 7 100 { (0,0) = 3 and (-1,0) = 9 and (0,1) = 9 }

rule : 4 100 { (0,0) = 0 and (0,-1) = 4 }
rule : 7 100 { (0,0) = 0 and (-1,0) = 4 and (-1,1) = 9 }
rule : 1 100 { (0,0) = 4 and (0,1) = 9 and (1,0) = 9 }

rule : 5 100 { (0,0) = 0 and (-1,-1) = 5 }
rule : 8 100 { (0,0) = 0 and (-1,0) = 5 and (0,1) = 9 }
rule : 3 100 { (0,0) = 0 and (0,-1) = 5 and (1,-1) = 9 and (1,0) = 9 }
rule : 1 100 { (0,0) = 5 and (1,0) = 9 and (0,1) = 9 }

rule : 6 100 { (0,0) = 0 and (-1,0) = 6 }
rule : 1 100 { (0,0) = 0 and (0,1) = 6 and (1,1) = 9 }
rule : 3 100 { (0,0) = 6 and (0,-1) = 9 and (1,0) = 9 }

rule : 7 100 { (0,0) = 0 and (-1,1) = 7 }
rule : 1 100 { (0,0) = 0 and (1,0) = 9 and (0,1) = 7 }
rule : 5 100 { (0,0) = 0 and (-1,0) = 7 and (0,-1) = 9 and (-1,-1) = 9 }
rule : 3 100 { (0,0) = 7 and (0,-1) = 9 and (1,0) = 9 }

rule : 8 100 { (0,0) = 0 and (0,1) = 8 }
rule : 3 100 { (0,0) = 0 and (1,-1) = 9 and (1,0) = 8 }
rule : 5 100 { (0,0) = 8 and (0,-1) = 9 and (-1,0) = 9 }

rule : 0 100 { t }
