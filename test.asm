; teste de comentario

.db nums 1, 2, 3, 4, 5
.ascii str "hello world"

    XOR R0, R0, R0
    LDI R1, str
    LDI R2, 11
    LDI R3, 1

loop:
    SUB R2, R2, R3
    MOV R4, [R1]
    LLI R6, 0xFF
    AND R4, R6, R4
    ADD R5, R4, R5
    ADD R1, R3, R1
    CMP R2, R0
    LDI R7, end
    JEQ R7
    LDI R7, loop
    JMP R7

end:
    HLT