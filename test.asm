; teste de comentario

comeco: LDI R0, 10
LDI R1, 20
LDI R2, meio
JMP R2

fim:
HLT

meio:
LDI R2, fim
CMP R0, R1
JGT R2