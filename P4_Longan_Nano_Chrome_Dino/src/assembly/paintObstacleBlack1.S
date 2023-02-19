.section .text
.globl paintObstacleBlack1_S
.type paintObstacleBlack1_S, @function
#void paintObstacleBlack1_S(int x, int y, Obstacle obstacle, unsigned char* image);
paintObstacleBlack1_S:
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
# LCD_ShowPicCustomed(x, y, x + getLengthObstacle(obstacle) - 1, y + getWidthObstacle(obstacle) - 1, black3Image);
	#store a0
	
	mv s7, a0
	#store a1
	mv s6, a1
	#store a2
	mv s5, a2
	#store a3
	mv s4, a3

	# getLengthObstacle(obstacle)
	mv a0, s5
	# call getLengthObstacle
	jal getLengthObstacle
	# get the return value
	mv s3, a0

	# getWidthObstacle(obstacle)
	mv a0, s5
	# call getWidthObstacle
	jal getWidthObstacle
	# get the return value
	mv s2, a0

	#get origin a0
	mv a0, s7 
	#get origin a1
	mv a1, s6 

	li a2, 0
	# a2 = a2 + a0
	add a2, a2, a0
	# x + getLengthObstacle(obstacle)
	add a2, a2, s3
	# x + getLengthObstacle(obstacle) - 1
	addi a2, a2, -1
	
	li a3, 0
	# a3 = a3 + a1
	add a3, a3, a1
	# y + getWidthObstacle(obstacle)
	add a3, a3, s2
	# y + getWidthObstacle(obstacle) - 1
	addi a3, a3, -1
	#image
	mv a4, s4

	# call LCD_ShowPicCustomed
    jal LCD_ShowPicCustomed

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
    