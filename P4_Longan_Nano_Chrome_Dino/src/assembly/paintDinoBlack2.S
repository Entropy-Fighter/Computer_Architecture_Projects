.section .text
.globl paintDinoBlack2_S
.type paintDinoBlack2_S, @function
#void paintDinoBlack2_S(int x, int y, unsigned char*);
paintDinoBlack2_S:
    # prototype for all funxtions
    addi sp, sp, -32 #reserve 8 regs
    # below regs can be treated as temp vars
	sw ra, 0(sp)# return address reserved
	sw s1, 4(sp)# reserved
	sw s2, 8(sp) # reserved
	sw s3, 12(sp) # reserved
	sw s4, 16(sp) # reserved
	sw s5, 20(sp)# reserved
	sw s6, 24(sp) # reserved
	sw s7, 28(sp) # reserved
    # above regs can bre treated as temp vars
    # s1 - s7 can be used freely
##############################################################################
# implementation here
    # dino.x
    mv s7, a0
    # dino.y
    mv s6, a1 
    # black2Image
    mv s5, a2 

    # DINO_BELOW_LENGTH
    li t1, 27 
    # DINO_BELOW_WIDTH
    li t2, 20 

    # dino.x + picLength - 1
    mv a2, a0
    add a2, a2, t1
    addi a2, a2, -1

    # dino.y + picWidth - 1
    mv a3, s6
    add a3, a3, t2
    addi a3, a3, -1

    # clear them
    li t1, 0
    li t2, 0
    
    # black2mage
    mv a4, s5

    # LCD_ShowPicCustomed
    jal LCD_ShowPicCustomed

############################################################################
    # restore saved regs
    lw ra,0(sp)# return address restored
	lw s1,4(sp) #restored
	lw s2,8(sp) #restored
	lw s3,12(sp) #restored
	lw s4,16(sp) #restored
	lw s5,20(sp) #restored
	lw s6,24(sp) #restored
	lw s7,28(sp) #restored
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
	