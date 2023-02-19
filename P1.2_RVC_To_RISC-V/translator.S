#   Project 1.2 RVC instructions to RISC-V instructions in RISC-V

#   You can suppose you will get the number of machine codes you should translate from 
#   "lines_of_rvc_codes" and get the 16 or 32 bits-machine codes from "rvc_codes". 
#   Both of them are stored in file "input.S"

#   When we test your project, we will change the input files content.

# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

# Your code to convert the RVC codes to RVI codes.

.text

main:

############# define global variables ################################

    lw a2, lines_of_rvc_codes
    la a3, rvc_codes

    # answer[a2 + 1]
    mv t1, a2   # t1 = a2
    addi t1, t1, 10 #To guarantee data safety, the 10 words space are left for redundancy
    slli t1, t1, 2  #t1 *= 4
    neg t1, t1      #t1 = -t1 
    add sp, sp, t1 #use stack in the form of heap
    mv a4, sp

    # originalLength[a2 + 1]
    mv t1, a2 # t1 = a2
    addi t1, t1, 10 #To guarantee data safety, the 10 words space are left for redundancy
    slli t1, t1, 2  #t1 *= 4
    neg t1, t1      #t1 = -t1 
    add sp, sp, t1 #use stack in the form of heap
    mv a5, sp


    li s2, 1    # s2 = 1
    mv s3, a3   # s3 = a3  current instruction
    li s4, 1    # s4 = 1  cnt of jal and branch

    # jumpAt[] in p1.1    
    mv t1, a2 # t1 = a2
    addi t1, t1, 10 #To guarantee data safety, the 10 words space are left for redundancy
    slli t1, t1, 2  #t1 *= 4
    neg t1, t1      #t1 = -t1 
    add sp, sp, t1 #use stack in the form of heap
    mv s5, sp

    # jumpDistance[] in p1.1     
    mv t1, a2 # t1 = a2
    addi t1, t1, 10 #To guarantee data safety, the 10 words space are left for redundancy
    slli t1, t1, 2  #t1 *= 4
    neg t1, t1      #t1 = -t1 
    add sp, sp, t1 #use stack in the form of heap
    mv s6, sp

    # jumpType[] in p1.1     
    mv t1, a2 # t1 = a2
    addi t1, t1, 10 #To guarantee data safety, the 10 words space are left for redundancy
    slli t1, t1, 2  #t1 *= 4
    neg t1, t1      #t1 = -t1 
    add sp, sp, t1 #use stack in the form of heap
    mv s7, sp


################################# start processing instructions ###########################

    mainLoop:
        # judge whether the current instruction is 16 bits long
        lhu t1, 0(s3)# t1 is the current instruction


##################################  CR Format ############################


judgeCADD:

        li t2, 0b1001000000000010 #fun4 and opcode 
        li t4, 0b1111000000000011
        and t3, t4, t1 
        xor t3, t3, t2 # check fun4 opcode
        bne t3, x0, notCADD

        li t2, 0b111110000000 #rs1 == 0 ?
        and t3, t2, t1
        beq t3, x0, notCADD

        li t2, 0b1111100 #rs2 == 0 ?
        and t3, t2, t1
        beq t3, x0, notCADD

        #### t1 is C.ADD  ######
        jal decompressCADD
        j s3add2

        notCADD:



judgeCMV:

        li t2, 0b1000000000000010 #fun4 and opcode 
        li t4, 0b1111000000000011
        and t3, t4, t1 
        xor t3, t3, t2 # check fun4 opcode
        bne t3, x0, notCMV

        li t2, 0b111110000000 #rs1 == 0 ?
        and t3, t2, t1
        beq t3, x0, notCMV

        li t2, 0b1111100 #rs2 == 0 ?
        and t3, t2, t1
        beq t3, x0, notCMV

        #### t1 is C.MV  ######
        jal decompressCMV
        j s3add2

        notCMV:


judgeCJR:

        li t2, 0b1000000000000010 #fun4 and opcode 
        li t4, 0b1111000000000011
        and t3, t4, t1 
        xor t3, t3, t2 # check fun4 opcode
        bne t3, x0, notCJR

        li t2, 0b111110000000 #rs1 == 0 ?
        and t3, t2, t1
        beq t3, x0, notCJR

        li t2, 0b1111100 #rs2 == 0 ?
        and t3, t2, t1
        bne t3, x0, notCJR

        #### t1 is C.JR  ######
        jal decompressCJR
        j s3add2

        notCJR:


judgeCJALR:

        li t2, 0b1001000000000010 #fun4 and opcode 
        li t4, 0b1111000000000011
        and t3, t4, t1 
        xor t3, t3, t2 # check fun4 opcode
        bne t3, x0, notCJALR

        li t2, 0b111110000000 #rs1 == 0 ?
        and t3, t2, t1
        beq t3, x0, notCJALR

        li t2, 0b1111100 #rs2 == 0 ?
        and t3, t2, t1
        bne t3, x0, notCJALR

        #### t1 is C.JALR  ######
        jal decompressCJALR
        j s3add2

        notCJALR:


##################################  CI Format ############################

judgeCLI:

        li t2, 0b0100000000000001 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1
        xor t3, t2, t3 # check fun4 opcode
        bne t3, x0, notCLI

        li t2, 0b111110000000 # rd == 0?
        and t3, t2, t1
        beq t3, x0, notCLI

        #### t1 is C.LI  ######
        jal decompressCLI
        j s3add2

        notCLI:


judgeCLUI: 

        li t2, 0b0110000000000001 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1
        xor t3, t2, t3 # check fun3 opcode
        bne t3, x0, notCLUI
        
        li t2, 0b111110000000 # rd == 0?
        and t3, t2, t1
        beq t3, x0, notCLUI

        li t4, 0b000100000000 # rd == 2?
        and t3, t3, t4
        beq t3, t4, notCLUI

        li t2, 0b0001000001111100 # imm == 0?
        and t3, t1, t2
        beq t3, x0, notCLUI

        #### t1 is C.LUI  ######
        jal decompressCLUI
        j s3add2

        notCLUI:


judgeCADDI:

        li t2, 0b0000000000000001 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1
        xor t3, t2, t3 # check fun3 opcode
        bne t3, x0, notCADDI

        li t2, 0b111110000000 # rd == 0?
        and t3, t2, t1

        beq t3, x0, notCADDI

        li t2, 0b0001000001111100 # imm == 0?
        and t3, t1, t2
        beq t3, x0, notCADDI

        #### t1 is C.ADDI  ######
        jal decompressCADDI
        j s3add2

        notCADDI:


judgeCSLLI:

        li t2, 0b0000000000000010 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1
        xor t3, t2, t3 # check fun3 opcode
        bne t3, x0, notCSLLI

        li t2, 0b111110000000 # rd == 0?
        and t3, t2, t1
        beq t3, x0, notCSLLI

        li t2, 0b0001000000000000 # shamt[5] == 0?
        and t3, t2, t1
        bne t3, x0, notCSLLI

        #### t1 is C.SLLI  ######
        jal decompressCSLLI
        j s3add2


        notCSLLI:

##################################  CL & CS Format ############################


judgeCLW:
        li t2, 0b0100000000000000 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1
        xor t3, t2, t3 # check fun3 opcode
        bne t3, x0, notCLW

        #### t1 is C.LW  ######
        jal decompressCLW
        j s3add2

        notCLW:

judgeCSW:
        li t2, 0b1100000000000000 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1 
        xor t3, t2, t3 # check fun3 opcode
        bne t3, x0, notCSW

        #### t1 is C.SW  ######
        jal decompressCSW
        j s3add2

        notCSW:


##################################  CA Format ############################

judgeCAND:
        li t2, 0b1000110001100001 #funct6, funct2 and opcode
        li t4, 0b1111110001100011
        and t3, t4, t1
        xor t3, t2, t3 # check fun6 opcode funct2
        bne t3, x0, notCAND

        #### t1 is C.AND  ######
        jal decompressCAND
        j s3add2

        notCAND:


judgeCOR:
        li t2, 0b1000110001000001 #funct6, funct2 and opcode
        li t4, 0b1111110001100011
        and t3, t4, t1
        xor t3, t2, t3 # check fun6 opcode funct2
        bne t3, x0, notCOR

        #### t1 is C.OR  ######
        jal decompressCOR
        j s3add2

        notCOR:


judgeCXOR:
        li t2, 0b1000110000100001 #funct6, funct2 and opcode
        li t4, 0b1111110001100011
        and t3, t4, t1
        xor t3, t2, t3 # check fun6 opcode funct2
        bne t3, x0, notCXOR

        #### t1 is C.XOR  ######
        jal decompressCXOR
        j s3add2

        notCXOR:

judgeCSUB:
        li t2, 0b1000110000000001 #funct6, funct2 and opcode
        li t4, 0b1111110001100011
        and t3, t4, t1
        xor t3, t2, t3 # check fun6 opcode funct2
        bne t3, x0, notCSUB

        #### t1 is C.SUB  ######
        jal decompressCSUB
        j s3add2

        notCSUB:


##################################  CB Format ############################

judgeCBEQZ:
        li t2, 0b1100000000000001 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1
        xor t3, t2, t3 # check fun3 opcode
        bne t3, x0, notCBEQZ

        #### t1 is C.BEQZ  ######
        jal decompressCBEQZ
        j s3add2

        notCBEQZ:


judgeCBNEZ:
        li t2, 0b1110000000000001 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1
        xor t3, t2, t3 # check fun3 opcode
        bne t3, x0, notCBNEZ

        #### t1 is C.BNEZ  ######
        jal decompressCBNEZ
        j s3add2

        notCBNEZ:


judgeCSRLI:
        li t2, 0b1000000000000001 #funct3, funct2 and opcode
        li t4, 0b1110110000000011
        and t3, t4, t1
        xor t3, t2, t3 # check fun3 opcode funct2
        bne t3, x0, notCSRLI

        li t2, 0b1000000000000 # shamt[5] == 0?
        and t3, t2, t1
        bne t3, x0, notCSRLI

        #### t1 is C.SRLI  ######
        jal decompressCSRLI
        j s3add2

        notCSRLI:

judgeCSRAI:
        li t2, 0b1000010000000001 #funct3, funct2 and opcode
        li t4, 0b1110110000000011
        and t3, t4, t1
        xor t3, t2, t3 # check fun3 opcode funct2
        bne t3, x0, notCSRAI

        li t2, 0b1000000000000 # shamt[5] == 0?
        and t3, t2, t1
        bne t3, x0, notCSRAI

        #### t1 is C.SRAI  ######
        jal decompressCSRAI
        j s3add2

        notCSRAI:

judgeCANDI:
        li t2, 0b1000100000000001 #funct3, funct2 and opcode
        li t4, 0b1110110000000011
        and t3, t4, t1
        xor t3, t2, t3 #funct3, funct2 and opcode
        bne t3, x0, notCANDI

        #### t1 is C.ANDI  ######
        jal decompressCANDI
        j s3add2

        notCANDI:


##################################  CJ Format ############################


judgeCJ:
        li t2, 0b1010000000000001 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1
        xor t3, t2, t3 #funct3 and opcode
        bne t3, x0, notCJ

        #### t1 is C.J  ######
        jal decompressCJ
        j s3add2

        notCJ:

judgeCJAL:
        li t2, 0b0010000000000001 #funct3 and opcode
        li t4, 0b1110000000000011
        and t3, t4, t1
        xor t3, t2, t3 #funct3 and opcode
        bne t3, x0, notCJAL

        #### t1 is C.JAL  ######
        jal decompressCJAL
        j s3add2

        notCJAL:

        # we can draw the conclusion that:
        ######################### t1 is a 32 bit long instruction ######################

        lw t1, 0(s3)

        #just store t1 into answer[s2]
        mv t3, s2 
        slli t3, t3, 2
        add t3, a4, t3  # t3 = answer + s2
        sw t1, 0(t3)    #answer[s2] = t1

judgeSB:

        li t2, 0b1100011 # opcode
        li t4, 0b1111111
        and t3, t4, t1
        xor t3, t2, t3 # opcode
        bne t3, x0, notSB

        mv t2, s4
        slli t2, t2, 2
        add t2, t2, s5 # t2 =jumpAt[s4]
        sw s2, 0(t2)   # jumpAt[s4] = s2
        
        jal getImmSB

        j s3add4

        notSB:

judgeUJ:
        li t2, 0b1101111 # opcode
        li t4, 0b1111111
        and t3, t4, t1
        xor t3, t2, t3 # opcode
        bne t3, x0, notUJ

        mv t2, s4
        slli t2, t2, 2 
        add t2, t2, s5 # t2 =jumpAt[s4]
        sw s2, 0(t2) # jumpAt[s4] = s2
        
        jal getImmUJ

        j s3add4

        notUJ:

############################## almost at the end of the current loop #############################


        s3add4:
        addi s3, s3, 4  #for next instruction

        slli, t2, s2, 2
        add t2, t2, a5  # t2 = originalLength + s2
        li t3, 32       # t3 = 32
        sw t3, 0(t2)    #originalLength[s2] = 32(bits)
        j endingOfMainLoop 


        s3add2:
        addi s3, s3, 2  #for next instruction 

        slli t2, s2, 2
        add t2, t2, a5  # t2 = originalLength + s2
        li t3, 16       # t3 = 16
        sw t3, 0(t2)    #originalLength[s2] = 16(bits)
        j endingOfMainLoop
        

        endingOfMainLoop:

        addi s2, s2, 1       # ++s2
        ble s2, a2, mainLoop # if s2 <= a2, continue

        

        li s2, 1 # s2 = 1

    relocate:
        bge s2, s4, outputAns # if (s2 >= s4) break;

        mv t2, s2       # t2 = s2
        slli t2, t2, 2  # t2 *=4
        mv t3, t2       # t3 = t2, for later use
        mv t5, t2       # t5 = t2, for later use
        add t2, t2, s6  # t2 = jumpDistance + s2
        lw t2, 0(t2)    # t2 = jumpDistance[s2] (bits)

        add t3, t3, s5  # t3 = jumpAt + s2
        lw t3, 0(t3)    # t3 = jumpAt[s2]
        mv s10, t3      # for later use
        slli t3, t3, 2  # t3 *=4
        add t3, t3, a5  # t3 = originalLength + s2


        mv s11, x0 #  to store the TRUE imm, first record how many rows intended to move
        beq t2, x0, SBorUJ     # if t2 == 0, no need to recalculate

        bgt t2, x0, recalImmPositive # if t2 > 0
        blt t2, x0, recalImmNegative # if t2 < 0

        recalImmPositive:

        loopInRIP:  # a loop in recalImmPositive
            addi t3, t3, 4      # ++t3
            lw t4, 0(t3)        # t4 = originalLength[t3]
            sub t2, t2, t4      # jumpDistance[s2] -= originalLength[t3]

            beq t2, x0, endOfRIP #if t2 == 0 , break
            addi s11, s11, 1    # ++s11
            j loopInRIP

        endOfRIP:
            addi s11, s11, 1
        j SBorUJ


        recalImmNegative:

        loopInRIN:  # a loop in recalImmNegative
            addi t3, t3, -4      # --t3
            lw t4, 0(t3)        # t4 = originalLength[t3]
            add t2, t2, t4      # jumpDistance[s2] += originalLength[t3]

            beq t2, x0, endOfRIN #if t2 == 0 , break
            addi s11, s11, -1    # --s11
            j loopInRIN

        endOfRIN:
            addi s11, s11, -1
        j SBorUJ
        
        
        SBorUJ:         #which type of instruction it is?

        slli s11, s11, 2        # the imm we neeeed!!!!

        add t5, t5, s7  # t5 = jumpType + s2 
        lw t5, 0(t5)    # t5 = jumpType[s2]

        beq x0, t5, itisSB      # t5 ==0, SB type
        bne x0, t5, itisUJ      # t5 ==1, UJ type


        itisSB: # itisSB
        jal fillImmSB
        j endingOfRelocate

        itisUJ: # itisUJ
        jal fillImmUJ
        j endingOfRelocate


        endingOfRelocate:
        
        addi s2, s2, 1 # ++s2
        j relocate



    outputAns: # print the processed instructions on screen
        li s2, 1
        mv t2, a4 
        addi t2, t2, 4 #t2 = answer + 1

    loopOfInstructions:

        lw t3, 0(t2) # t3 = answer[s2 + 1]
        li t4, 0b10000000000000000000000000000000
        li t5, 1
        li t6, 32

        li a0, 1 #print_int: prints integer in a1
        printInstruction: # print a instruction 
            and t1, t4, t3
            beq t1, x0, print0
            bne t1, x0, print1

            print0: # print bin
            li a1, 0
            j nextINPI

            print1: # print bin
            li a1, 1
            j nextINPI


            nextINPI:
            ecall #print this bit
            srli t4, t4, 1 # t4 >>= 1
            addi t5, t5, 1

            ble t5, t6, printInstruction # if t5 <= 32 ,continue

        #print a '\n'
        li a0,11 #print_character	prints ASCII character in a1
        li a1,10 #Each line of instruction ends with a \n (ASCII: 10).
        ecall 

        bge s2, a2, returnOfMain # if s2 >= a2 ,break

        addi t2, t2, 4  # ++t2
        addi s2, s2, 1  
        j loopOfInstructions


    returnOfMain:

    # Exit with error code 0
    add	   a1, x0, x0
    addi   a0, x0, 17
    ecall



################################################################################################
#                #
##              #
# #            #
#  #          #       a separator from main function
#   #        #
#    #      #
#     #    #
#      #  #
#       #
################################################################################################



##################################  CR Format ############################



decompressCADD:
    li t2, 0b00000000000000000000000000110011 #funct7, funct3 and opcode
    li t3, 0b111110000000

    and t3, t1, t3 #get rs1/rd
    or t2, t2, t3 #rd of t2

    slli t3, t3, 8
    or t2, t2, t3 #rs1 of t2

    li t3, 0b1111100
    and t3, t3, t1 #get rs2
    slli t3, t3, 18  ###### ? t3

    or t2, t2, t3 #rs2 of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    #answer[s2] = t2

    jr ra   # jump to ra
    

decompressCMV:
    ############ be care #####################
    #this function is based on decompressCADD
    li t2, 0b00000000000000000000000000110011 #funct7, funct3 and opcode
    li t3, 0b111110000000

    and t3, t1, t3 #get rd
    or t2, t2, t3 #rd of t2

    #rs1 of t2 is x0, so no need to bother

    li t3, 0b1111100
    and t3, t3, t1 #get rs2
    slli t3, t3, 18
    or t2, t2, t3 #rs2 of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    #answer[s2] = t2

    jr ra   # jump to ra
    

decompressCJR:

    li t2, 0b00000000000000000000000001100111
    li t3, 0b111110000000
    and t3, t3, t1 #get rs1
    slli t3, t3, 8
    or t2, t2, t3 #rs1 of t2


    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    #answer[s2] = t2

    jr ra   # jump to ra
    

decompressCJALR:
    ############ be care #####################
    #this function is based on decompressCJR

    li t2, 0b00000000000000000000000011100111
    li t3, 0b111110000000
    and t3, t3, t1 #get rs1
    slli t3, t3, 8
    or t2, t2, t3 #rs1 of t2


    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    #answer[s2] = t2

    jr ra   # jump to ra


##################################  CI Format ############################

    

decompressCLI:
    li t2, 0b00000000000000000000000000010011 # funct3 and opcode
    li t3, 0b111110000000

    and t3, t1, t3 # get rd
    or t2, t2, t3 # rd of t2

    li t3, 0b1111100
    and t3, t3, t1 # get imm[4:0]
    slli t3, t3, 18
    or t2, t2, t3 # imm [4:0] of t2
     
    li t3, 0b1000000000000
    and t3, t3, t1 # get nzimm[5]
    slli t3, t3, 19
    srai t3, t3, 6 # imm sign extension
    or t2, t2, t3 # imm sign of t2  

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra
    

decompressCLUI:
    li t2, 0b00000000000000000000000000110111 # opcode
    li t3, 0b111110000000

    and t3, t1, t3 # get rd
    or t2, t2, t3 # rd of t2

    li t3, 0b1111100
    and t3, t3, t1 # get nzimm[16:12]
    slli t3, t3, 10
    or t2, t2, t3 # nzimm [16:12] of t2

    li t3, 0b1000000000000
    and t3, t3, t1 # get nzimm[17]
    slli t3, t3, 19
    srai t3, t3, 14 # imm sign extension
    or t2, t2, t3 # imm sign of t2  

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra


decompressCADDI:
    li t2, 0b00000000000000000000000000010011 # funct3 and opcode
    li t3, 0b111110000000

    and t3, t1, t3 # get rd
    or t2, t2, t3 # rd of t2

    slli t3, t3, 8 # get rs1
    or t2, t2, t3 # rs1 of t2

    li t3, 0b1111100
    and t3, t3, t1 # get nzimm[4:0]
    slli t3, t3, 18
    or t2, t2, t3 # nzimm [4:0] of t2
     
    li t3, 0b1000000000000
    and t3, t3, t1 # get nzimm[5]
    slli t3, t3, 19
    srai t3, t3, 6 # imm sign extension
    or t2, t2, t3 # imm sign of t2  

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra

decompressCSLLI:
    li t2, 0b00000000000000000001000000010011 # funct3 and opcode
    li t3, 0b111110000000

    and t3, t1, t3 # get rd
    or t2, t2, t3 # rd of t2

    slli t3, t3, 8 # get rs1
    or t2, t2, t3 # rs1 of t2

    li t3, 0b1111100
    and t3, t3, t1 # get nzimm[4:0]
    slli t3, t3, 18
    or t2, t2, t3 # nzimm [4:0] of t2
     
    li t3, 0b1000000000000
    and t3, t3, t1 # get nzimm[5]
    slli t3, t3, 19
    srai t3, t3, 6 # imm sign extension
    or t2, t2, t3 # imm sign of t2  

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra


##################################  CL Format ############################


decompressCLW:
    li t2, 0b00000000000001000010010000000011 # funct3 and opcode and add "01" to rs' rd'
    li t3, 0b11100

    and t3, t1, t3 # get rd'
    slli t3, t3, 5
    or t2, t2, t3 # rd of t2
    
    li t3, 0b1110000000
    and t3, t3, t1 # get rs1'
    slli t3, t3, 8
    or t2, t2, t3 # rs1 of t2

    li t3, 0b100000
    and t3, t3, t1 # get offset[6]
    slli t3, t3, 21
    or t2, t2, t3 # offset[6] of t2
     
    li t3, 0b1000000
    and t3, t3, t1 # get offset[2]
    slli t3, t3, 16
    or t2, t2, t3 # offset[2] of t2  

    li t3, 0b1110000000000
    and t3, t3, t1 # get offset[5:3]
    slli t3, t3, 13
    or t2, t2, t3 # offset[5:3] of t2 

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra

##################################  CS Format ############################


decompressCSW:
    li t2, 0b00000000100001000010000000100011 # funct3 and opcode and add "01" to rs' rd'
    li t3, 0b11100

    and t3, t1, t3 # get rs2'
    slli t3, t3, 18
    or t2, t2, t3 # rs2 of t2
    
    li t3, 0b1110000000
    and t3, t3, t1 # get rs1'
    slli t3, t3, 8
    or t2, t2, t3 # rs1 of t2

    li t3, 0b100000
    and t3, t3, t1 # get offset[6]
    slli t3, t3, 21
    or t2, t2, t3 # offset[6] of t2
     
    li t3, 0b1000000
    and t3, t3, t1 # get offset[2]
    slli t3, t3, 3
    or t2, t2, t3 # offset[2] of t2  

    li t3, 0b110000000000
    and t3, t3, t1 # get offset[4:3]
    or t2, t2, t3 # offset[4:3] of t2 

    li t3, 0b1000000000000
    and t3, t3, t1 # get offset[5]
    slli t3, t3, 13
    or t2, t2, t3 # offset[5] of t2 

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra

##################################  CA Format ############################



decompressCAND:
    li t2, 0b00000000100001000111010000110011 #funct7, funct3 and opcode
    li t3, 0b1110000000

    and t3, t1, t3 #get rs1'/rd'
    or t2, t2, t3 #rd of t2

    slli t3, t3, 8
    or t2, t2, t3 #rs1' of t2

    li t3, 0b11100
    and t3, t3, t1 #get rs2'
    slli t3, t3, 18

    or t2, t2, t3 #rs2 of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    #answer[s2] = t2
    jr ra   # jump to ra

decompressCOR:
    li t2, 0b00000000100001000110010000110011 #funct7, funct3 and opcode
    li t3, 0b1110000000

    and t3, t1, t3 #get rs1'/rd'
    or t2, t2, t3 #rd of t2

    slli t3, t3, 8
    or t2, t2, t3 #rs1' of t2

    li t3, 0b11100
    and t3, t3, t1 #get rs2'
    slli t3, t3, 18

    or t2, t2, t3 #rs2 of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    #answer[s2] = t2
    jr ra   # jump to ra

decompressCXOR:
    li t2, 0b00000000100001000100010000110011 #funct7, funct3 and opcode
    li t3, 0b1110000000

    and t3, t1, t3 #get rs1'/rd'
    or t2, t2, t3 #rd of t2

    slli t3, t3, 8
    or t2, t2, t3 #rs1' of t2

    li t3, 0b11100
    and t3, t3, t1 #get rs2'
    slli t3, t3, 18

    or t2, t2, t3 #rs2 of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    #answer[s2] = t2
    jr ra   # jump to ra


decompressCSUB:
    li t2, 0b01000000100001000000010000110011 #funct7, funct3 and opcode
    li t3, 0b1110000000

    and t3, t1, t3 #get rs1'/rd'
    or t2, t2, t3 #rd of t2

    slli t3, t3, 8
    or t2, t2, t3 #rs1' of t2

    li t3, 0b11100
    and t3, t3, t1 #get rs2'
    slli t3, t3, 18

    or t2, t2, t3 #rs2 of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    #answer[s2] = t2
    jr ra   # jump to ra

##################################  CB Format ############################


decompressCBEQZ:
    ################ jumpAt #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s5 # t2 =jumpAt[s4]
    sw s2, 0(t2)   # jumpAt[s4] = s2
    
    ################ jumpType #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s7 # t2 =jumpType[s4]
    li t3, 0
    sw t3, 0(t2)   # jumpType[s4] = 0

    ################ answer #################
    li t2, 0b00000000000001000000000001100011 # funct3 and opcode
    li t3, 0b1110000000

    and t3, t1, t3 # get rs1'
    slli t3, t3, 8 # get rs1
    or t2, t2, t3 # rs1 of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    ################ jumpDistance #################
    li t4, 0 # t4 to temporarily store imm
    li t3, 0b100
    and t3, t1, t3 # get imm[5]
    slli t3, t3, 3
    or t4, t4, t3 # imm[5] of t4

    li t3, 0b11000
    and t3, t1, t3 # get imm[2:1]
    srli t3, t3, 2
    or t4, t4, t3 # imm[2:1] of t4

    li t3, 0b110000000000
    and t3, t1, t3 # get imm[4:3]
    srli t3, t3, 7
    or t4, t4, t3 # imm[4:3] of t4

    li t3, 0b1100000
    and t3, t1, t3 # get imm[7:6]
    slli t3, t3, 1
    or t4, t4, t3 # imm[7:6] of t4

    li t3, 0b1000000000000
    and t3, t1, t3 # get imm[8]
    slli t3, t3, 19
    srai t3, t3, 23 # imm sign extension
    or t4, t4, t3 # imm sign of t4  

    slli t4, t4, 3 # imm*8 to get bits

    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s6 # t2 =jumpDistance[s4]
    sw t4, 0(t2)   # jumpDistance[s4] = t4

    addi s4, s4, 1 # s4++

    jr ra   # jump to ra

decompressCBNEZ:
    ################ jumpAt #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s5 # t2 =jumpAt[s4]
    sw s2, 0(t2)   # jumpAt[s4] = s2
    
    ################ jumpType #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s7 # t2 =jumpType[s4]
    li t3, 0
    sw t3, 0(t2)   # jumpType[s4] = 0

    ################ answer ################
    li t2, 0b00000000000001000001000001100011 # funct3 and opcode
    li t3, 0b1110000000

    and t3, t1, t3 # get rs1'
    slli t3, t3, 8 # get rs1
    or t2, t2, t3 # rs1 of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    ################ jumpDistance #################
    li t4, 0 # t4 to temporarily store imm
    li t3, 0b100
    and t3, t1, t3 # get imm[5]
    slli t3, t3, 3
    or t4, t4, t3 # imm[5] of t4

    li t3, 0b11000
    and t3, t1, t3 # get imm[2:1]
    srli t3, t3, 2
    or t4, t4, t3 # imm[2:1] of t4

    li t3, 0b110000000000
    and t3, t1, t3 # get imm[4:3]
    srli t3, t3, 7
    or t4, t4, t3 # imm[4:3] of t4

    li t3, 0b1100000
    and t3, t1, t3 # get imm[7:6]
    slli t3, t3, 1
    or t4, t4, t3 # imm[7:6] of t4

    li t3, 0b1000000000000
    and t3, t1, t3 # get imm[8]
    slli t3, t3, 19
    srai t3, t3, 23 # imm sign extension
    or t4, t4, t3 # imm sign of t4  

    slli t4, t4, 3 # imm*8 to get bits

    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s6 # t2 =jumpDistance[s4]
    sw t4, 0(t2)   # jumpDistance[s4] = t4

    addi s4, s4, 1 # s4++

    jr ra   # jump to ra

decompressCSRLI:
    li t2, 0b00000000000001000101010000010011 # funct3 and opcode
    li t3, 0b1110000000

    and t3, t1, t3 # get rd
    or t2, t2, t3 # rd of t2

    slli t3, t3, 8 # get rs1
    or t2, t2, t3 # rs1 of t2

    li t3, 0b1111100
    and t3, t3, t1 # get nzimm[4:0]
    slli t3, t3, 18
    or t2, t2, t3 # nzimm [4:0] of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra

decompressCSRAI:
    li t2, 0b01000000000001000101010000010011 # funct3 and opcode
    li t3, 0b1110000000

    and t3, t1, t3 # get rd
    or t2, t2, t3 # rd of t2

    slli t3, t3, 8 # get rs1
    or t2, t2, t3 # rs1 of t2

    li t3, 0b1111100
    and t3, t3, t1 # get nzimm[4:0]
    slli t3, t3, 18
    or t2, t2, t3 # nzimm [4:0] of t2

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra

decompressCANDI:
    li t2, 0b00000000000001000111010000010011 # funct3 and opcode
    li t3, 0b1110000000

    and t3, t1, t3 # get rd
    or t2, t2, t3 # rd of t2

    slli t3, t3, 8 # get rs1
    or t2, t2, t3 # rs1 of t2

    li t3, 0b1111100
    and t3, t3, t1 # get nzimm[4:0]
    slli t3, t3, 18
    or t2, t2, t3 # nzimm [4:0] of t2
     
    li t3, 0b1000000000000
    and t3, t3, t1 # get nzimm[5]
    slli t3, t3, 19
    srai t3, t3, 6 # imm sign extension
    or t2, t2, t3 # imm sign of t2  

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra


##################################  CJ Format ############################

decompressCJ:
    ################ jumpAt #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s5 # t2 =jumpAt[s4]
    sw s2, 0(t2)   # jumpAt[s4] = s2
    
    ################ jumpType #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s7 # t2 =jumpType[s4]
    li t3, 1
    sw t3, 0(t2)   # jumpType[s4] = 0

    ################ answer #################
    li t2, 0b00000000000000000000000001101111 # opcode

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    ################ jumpDistance #################
    li t4, 0 # t4 to temporarily store imm
    li t3, 0b100
    and t3, t1, t3 # get imm[5]
    slli t3, t3, 3
    or t4, t4, t3 # imm[5] of t4

    li t3, 0b111000
    and t3, t1, t3 # get imm[3:1]
    srli t3, t3, 2
    or t4, t4, t3 # imm[3:1] of t4

    li t3, 0b100000000000
    and t3, t1, t3 # get imm[4]
    srli t3, t3, 7
    or t4, t4, t3 # imm[4] of t4

    li t3, 0b10000000
    and t3, t1, t3 # get imm[6]
    srli t3, t3, 1
    or t4, t4, t3 # imm[6] of t4

    li t3, 0b1000000
    and t3, t1, t3 # get imm[7]
    slli t3, t3, 1
    or t4, t4, t3 # imm[7] of t4

    li t3, 0b11000000000
    and t3, t1, t3 # get imm[9:8]
    srli t3, t3, 1
    or t4, t4, t3 # imm[9:8] of t4

    li t3, 0b100000000
    and t3, t1, t3 # get imm[10]
    slli t3, t3, 2
    or t4, t4, t3 # imm[10] of t4

    li t3, 0b1000000000000
    and t3, t1, t3 # get imm[11]
    slli t3, t3, 19
    srai t3, t3, 20 # imm sign extension
    or t4, t4, t3 # imm sign of t4  

    slli t4, t4, 3 # imm*8 to get bits

    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s6 # t2 =jumpDistance[s4]
    sw t4, 0(t2)   # jumpDistance[s4] = t4

    addi s4, s4, 1 # s4++

    jr ra   # jump to ra

decompressCJAL:
    ################ jumpAt #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s5 # t2 =jumpAt[s4]
    sw s2, 0(t2)   # jumpAt[s4] = s2
    
    ################ jumpType #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s7 # t2 =jumpType[s4]
    li t3, 1
    sw t3, 0(t2)   # jumpType[s4] = 0

    ################ answer #################
    li t2, 0b00000000000000000000000011101111 # opcode

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    ################ jumpDistance #################
    li t4, 0 # t4 to temporarily store imm
    li t3, 0b100
    and t3, t1, t3 # get imm[5]
    slli t3, t3, 3
    or t4, t4, t3 # imm[5] of t4

    li t3, 0b111000
    and t3, t1, t3 # get imm[3:1]
    srli t3, t3, 2
    or t4, t4, t3 # imm[3:1] of t4

    li t3, 0b100000000000
    and t3, t1, t3 # get imm[4]
    srli t3, t3, 7
    or t4, t4, t3 # imm[4] of t4

    li t3, 0b10000000
    and t3, t1, t3 # get imm[6]
    srli t3, t3, 1
    or t4, t4, t3 # imm[6] of t4

    li t3, 0b1000000
    and t3, t1, t3 # get imm[7]
    slli t3, t3, 1
    or t4, t4, t3 # imm[7] of t4

    li t3, 0b11000000000
    and t3, t1, t3 # get imm[9:8]
    srli t3, t3, 1
    or t4, t4, t3 # imm[9:8] of t4

    li t3, 0b100000000
    and t3, t1, t3 # get imm[10]
    slli t3, t3, 2
    or t4, t4, t3 # imm[10] of t4

    li t3, 0b1000000000000
    and t3, t1, t3 # get imm[11]
    slli t3, t3, 19
    srai t3, t3, 20 # imm sign extension
    or t4, t4, t3 # imm sign of t4  

    slli t4, t4, 3 # imm*8 to get bits

    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s6 # t2 =jumpDistance[s4]
    sw t4, 0(t2)   # jumpDistance[s4] = t4

    addi s4, s4, 1 # s4++

    jr ra   # jump to ra

################################### SB & UJ Format ############################

getImmSB:
    ################ jumpType #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s7 # t2 =jumpType[s4]
    li t3, 0
    sw t3, 0(t2)   # jumpType[s4] = 0

    ################ jumpDistance #################
    li t4, 0 # t4 to temporarily store imm
    li t3, 0b10000000
    and t3, t1, t3 # get imm[11]
    slli t3, t3, 4
    or t4, t4, t3 # imm[11] of t4

    li t3, 0b111100000000
    and t3, t1, t3 # get imm[4:1]
    srli t3, t3, 7
    or t4, t4, t3 # imm[4:1] of t4

    li t3, 0b1111110000000000000000000000000
    and t3, t1, t3 # get imm[10:5]
    srli t3, t3, 20
    or t4, t4, t3 # imm[10:5] of t4

    li t3, 0b10000000000000000000000000000000
    and t3, t1, t3 # get imm[12]
    srai t3, t3, 19 # imm sign extension
    or t4, t4, t3 # imm sign of t4  

    slli t4, t4, 3 # imm*8 to get bits

    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s6 # t2 =jumpDistance[s4]
    sw t4, 0(t2)   # jumpDistance[s4] = t4

    addi s4, s4, 1 # s4++

    ################ answer #################
    li t2, 0b1111111111111000001100011 # opcode and rs2 rs1 func3
    and t2, t2, t1

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra


getImmUJ:
    ################ jumpType #################
    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s7 # t2 =jumpType[s4]
    li t3, 1
    sw t3, 0(t2)   # jumpType[s4] = 0

    ################ jumpDistance #################
    li t4, 0 # t4 to temporarily store imm
    li t3, 0b11111111000000000000
    and t3, t1, t3 # get imm[19:12]
    or t4, t4, t3 # imm[11] of t4

    li t3, 0b100000000000000000000
    and t3, t1, t3 # get imm[11]
    srli t3, t3, 9
    or t4, t4, t3 # imm[11] of t4

    li t3, 0b1111111111000000000000000000000
    and t3, t1, t3 # get imm[10:1]
    srli t3, t3, 20
    or t4, t4, t3 # imm[10:1] of t4

    li t3, 0b10000000000000000000000000000000
    and t3, t1, t3 # get imm[20]
    srai t3, t3, 11 # imm sign extension
    or t4, t4, t3 # imm sign of t4  

    slli t4, t4, 3 # imm*8 to get bits

    mv t2, s4
    slli t2, t2, 2
    add t2, t2, s6 # t2 =jumpDistance[s4]
    sw t4, 0(t2)   # jumpDistance[s4] = t4

    addi s4, s4, 1 # s4++

    ################ answer #################
    li t2, 0b111111101111 # opcode and rd
    and t2, t2, t1

    mv t3, s2
    slli t3, t3, 2
    add t3, a4, t3  # t3 = answer + s2
    sw t2, 0(t3)    # answer[s2] = t2

    jr ra   # jump to ra


fillImmSB:
    mv t2, s10
    slli t2, t2, 2
    add t2, t2, a4 # get answer[s10]
    lw t1, 0(t2) # store it in t1

    li t3, 0b11110
    and t3, s11, t3 # get imm[4:1]
    slli t3, t3, 7
    or t1, t1, t3 #imm[4:1] of t1

    li t3, 0b11111100000
    and t3, s11, t3 # get imm[10:5]
    slli t3, t3, 20
    or t1, t1, t3 #imm[10:5] of t1

    li t3, 0b100000000000
    and t3, s11, t3 # get imm[11]
    srli t3, t3, 4
    or t1, t1, t3 #imm[11] of t1

    li t3, 0b1000000000000
    and t3, s11, t3 # get imm[12]
    slli t3, t3, 19
    or t1, t1, t3 #imm[12] of t1

    sw t1, 0(t2) # get correct answer[s10]

    jr ra   # jump to ra


fillImmUJ:
    mv t2, s10
    slli t2, t2, 2
    add t2, t2, a4 # get answer[s10]
    lw t1, 0(t2) # store it in t1

    li t3, 0b11111111110
    and t3, s11, t3 # get imm[10:1]
    slli t3, t3, 20
    or t1, t1, t3 #imm[10:1] of t1

    li t3, 0b100000000000
    and t3, s11, t3 # get imm[11]
    slli t3, t3, 9
    or t1, t1, t3 #imm[11] of t1

    li t3, 0b11111111000000000000
    and t3, s11, t3 # get imm[19:12]
    or t1, t1, t3 #imm[19:12] of t1

    li t3, 0b100000000000000000000
    and t3, s11, t3 # get imm[20]
    slli t3, t3, 11
    or t1, t1, t3 #imm[20] of t1
    
    sw t1, 0(t2) # get correct answer[s10]

    jr ra   # jump to ra
