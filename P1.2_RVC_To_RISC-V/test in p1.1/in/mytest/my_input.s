00000000000100101000001010010011
00000000000011110000111001100011
11111110000011111001110011100011
11111110000001000000101011100011
00000000000100000000001010010011
00000000001000000000001100010011
00000000000100000000001110010011
11111110000001001001001011100011
11111110000001000001000011100011

000 0 00101 00001 01
0000000 00000 11110 000 10010 1100011   001001
1111111 00000 11111 001 11011 1100011
110 110 000 11011 01
010 000 101 00001 01
010 000 110 00010 01
010 000 111 00001 01
111 101 001 11111 01
111 101 000 11101 01

lable:
addi t0 t0 1
beqz t5 la1
bnez t6 lable
beqz x8 lable
li x5 1
li x6 2
li x7 1
bnez x9 lable
la1:
bnez x8 lable