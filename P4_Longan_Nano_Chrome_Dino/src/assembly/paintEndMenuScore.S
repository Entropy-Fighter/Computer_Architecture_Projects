.data
    L: .string "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
.section .text
.globl paintEndMenuScore_S
.type paintEndMenuScore_S, @function
#void paintEndMenuScore_S(char*,int x);

paintEndMenuScore_S:
    # prototype for all funxtions
    addi sp, sp, -32 #reserve 8 regs
    # below regs can be treated as temp vars
	sw ra, 0(sp)# return address reserved
	sw s1, 4(sp)# reserved
	sw s2, 8(sp)# reserved
	sw s3, 12(sp)# reserved
	sw s4, 16(sp)# reserved
	sw s5, 20(sp)# reserved
	sw s6, 24(sp)# reserved
	sw s7, 28(sp)# reserved
    # above regs can bre treated as temp vars
    # s1 - s7 can be used freely
##############################################################################
# implementation here
    # a0 is "Score is: %d"
    mv s7, a0
    # a1 is score
    mv s6, a1

    # a0: char scorestring[20]
    la a0, L
    # a1 is "Score is: %d"
    mv a1, s7
    # a2 is score
    mv a2, s6
    # call sprintf
    jal sprintf;

    # x
    li a0, 33
    # y
    li a1, 20
    # storestring
    la a2, L
    # color
    li a3, 0xffff 
    # call LCD_ShowString
    jal LCD_ShowString

############################################################################
    #restore saved regs
    lw ra,0(sp)# return address restored
	lw s1,4(sp)# restored
	lw s2,8(sp)# restored
	lw s3,12(sp)# restored
	lw s4,16(sp)# restored
	lw s5,20(sp)# restored
	lw s6,24(sp)# restored
	lw s7,28(sp)# restored
	addi sp,sp,32

    jr ra
# end of function
	# The above lines declares a function that can be accessed from C code.
	# When you're implementing your own function, you make a new file in
	# this folder, copy the above 4 lines, and replace all
	# "your_function_name" to your function's name.

	# Then you can add code after the label on line 4.

	# The rest of things work the same as in Venus, except that the directive
	# ".asciiz" is removed. Use ".string" instead.

	# Also, please make sure that each assembly function has a filename with 
	# extension name ".S", not ".s". Otherwise it will not be recognized
	# by the riscv-nuclei-elf-as and will throw an error.

	# You're free to remove this function after you've read the above.
