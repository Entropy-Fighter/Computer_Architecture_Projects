.data
    gameover: .string "Game over!"
.section .text
.globl paintEndMenuText_S
.type paintEndMenuText_S, @function

paintEndMenuText_S:
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

    #LCD_Clear(BLACK);
    li a0, 0x0000
    jal LCD_Clear
    
    # x = FRAME_LENGTH / 4
    li a0, 40
    # y = FRAME_WIDTH / 2
    li a1, 40
    # (u8*)"Game over!"
    la a2, gameover
    # color white
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
    