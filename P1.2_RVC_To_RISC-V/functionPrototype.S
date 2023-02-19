function:
    addi sp, sp, -32 #reserve 8 regs
    #below regs can be treated as temp vars
	sw ra, 0(sp)# return address reserved
	sw s1, 4(sp)
	sw s2, 8(sp)
	sw s3, 12(sp)
	sw s4, 16(sp)
	sw s5, 20(sp)
	sw s6, 24(sp)
	sw s7, 28(sp)
    #above regs can bre treated as temp vars
    #s1 - s7 can be used freely
##############################################################################
在这里写函数的实现


############################################################################
    #restore saved regs
    lw ra,0(sp)#return address restored
	lw s1,4(sp)
	lw s2,8(sp)
	lw s3,12(sp)
	lw s4,16(sp)
	lw s5,20(sp)
	lw s6,24(sp)
	lw s7,28(sp)
	addi sp,sp,32

    jr ra
#end of function