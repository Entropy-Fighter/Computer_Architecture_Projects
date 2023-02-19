#include "compression.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

extern char **instructions;
extern int prefixSum[MAX_INPUT_SCALE];

/*jump means the following instruciton:
    (1)jal (2)j (3)beq (4)bne
*/
int jumpAt[MAX_INPUT_SCALE];
int jumpDistance[MAX_INPUT_SCALE]; /*distance is counted by the number of
                                      instructions*/
char *jumpType[MAX_INPUT_SCALE];   /*j or beq or bne or othertype*/
int jumpNum = 0;

char op_1[7] = "0000011"; /*different opcode*/
char op_2[7] = "0010011";
char op_3[7] = "0010111";
char op_4[7] = "0100011"; /*different opcode*/
char op_5[7] = "0110011";
char op_6[7] = "0111011";
char op_7[7] = "1100011"; /*different opcode*/
char op_8[7] = "1100111";
char op_9[7] = "1101111";
char op_10[7] = "0110111"; /*different opcode*/

char func3_1[3] = "000"; /*different func3*/
char func3_2[3] = "001";
char func3_3[3] = "010"; /*different func3*/
char func3_4[3] = "011";
char func3_5[3] = "100"; /*different func3*/
char func3_6[3] = "101";
char func3_7[3] = "110"; /*different func3*/
char func3_8[3] = "111";

char func7_1[7] = "0000000"; /*different func7*/
char func7_2[7] = "0100000";

char typeBeq[3] = "beq"; /*different types which need to modify their imm*/
char typeBne[3] = "bne";
char typeJ[1] = "j"; /*different types which need to modify their imm*/
char typeO[1] = "o"; 

void compressor(int pos) { /*compressor main function*/
    char *instruction;
    char op[7]; /*compare op*/
    char func3[3]; /*compare func3*/
    char func7[7]; /*compare func7*/
    instruction = instructions[pos]; /* take the instruction*/
    memcpy(op, instruction + 25, 7); /*take op*/
    memcpy(func3, instruction + 17, 3);
    memcpy(func7, instruction, 7); /*take func7*/
    if (!strncmp(op, op_1, 7)) {
        if (!strncmp(func3, func3_3, 3)) {
            /*lw*/
            prefixSum[pos + 1] = prefixSum[pos] + compress_lw(pos);
        } else {
            prefixSum[pos + 1] = prefixSum[pos] + 32;
            /*others*/
        }
    } else if (!strncmp(op, op_4, 7)) {
        if (!strncmp(func3, func3_3, 3)) {
            /*sw*/
            prefixSum[pos + 1] = prefixSum[pos] + compress_sw(pos);
        } else {
            prefixSum[pos + 1] = prefixSum[pos] + 32;
            /*others*/
        }
    } else if (!strncmp(op, op_5, 7)) { /*check*/
        if (!strncmp(func3, func3_1, 3)) {
            if (!strncmp(func7, func7_1, 7)) {
                /* add mv*/
                prefixSum[pos + 1] = prefixSum[pos] + compress_add_mv(pos);
            } else {
                /*sub*/
                prefixSum[pos + 1] = prefixSum[pos] + compress_sub(pos);
            }
        } else if (!strncmp(func3, func3_7, 3)) {
            /*or*/
            prefixSum[pos + 1] = prefixSum[pos] + compress_or(pos);
        } else if (!strncmp(func3, func3_8, 3)) {
            /*and*/
            prefixSum[pos + 1] = prefixSum[pos] + compress_and(pos);
        } else if (!strncmp(func3, func3_5, 3)) {
            /*xor*/
            prefixSum[pos + 1] = prefixSum[pos] + compress_xor(pos);
        } else {
            prefixSum[pos + 1] = prefixSum[pos] + 32;
            /*others*/
        }
    } else if (!strncmp(op, op_8, 7)) {
        /*jalr jr*/
        prefixSum[pos + 1] = prefixSum[pos] + compress_jalr_jr(pos);
    } else if (!strncmp(op, op_10, 7)) {
        /*lui*/
        prefixSum[pos + 1] = prefixSum[pos] + compress_lui(pos);
    } else if (!strncmp(op, op_2, 7)) {
        if (!strncmp(func3, func3_1, 3)) { /*check*/
            /*addi li mv*/
            prefixSum[pos + 1] = prefixSum[pos] + compress_addi_li(pos);
        } else if (!strncmp(func3, func3_2, 3)) {
            /*slli*/
            prefixSum[pos + 1] = prefixSum[pos] + compress_slli(pos);
        } else if (!strncmp(func3, func3_6, 3)) {
            if (!strncmp(func7, func7_1, 7)) {
                /*srli*/
                prefixSum[pos + 1] = prefixSum[pos] + compress_srli(pos);
            } else {
                /*srai*/
                prefixSum[pos + 1] = prefixSum[pos] + compress_srai(pos);
            }
        } else if (!strncmp(func3, func3_8, 3)) {
            /*andi*/
            prefixSum[pos + 1] = prefixSum[pos] + compress_andi(pos);
        } else {
            prefixSum[pos + 1] = prefixSum[pos] + 32;
            /*others*/
        }
    } else if (!strncmp(op, op_7, 7)) {
        if (!strncmp(func3, func3_1, 3)) {
            /*beq*/
            jumpDistanceB(pos, 1);
            prefixSum[pos + 1] = prefixSum[pos] + compress_beq(pos); /*update prefix*/
            
        } else if (!strncmp(func3, func3_2, 3)) {
            /*bne*/
            jumpDistanceB(pos, 0);

            prefixSum[pos + 1] = prefixSum[pos] + compress_bne(pos);
        } else { /*other*/
            jumpDistanceB(pos, -1);
            /*other SBtype*/
            prefixSum[pos + 1] = prefixSum[pos] + 32;
        } /*other sb type*/
    } else if (!strncmp(op, op_9, 7)) {
        jumpDistanceJ(pos);

        /*compress_jal_j(int pos); j offset -> jal x0 offset*/
        prefixSum[pos + 1] = prefixSum[pos] + compress_jal_j(pos);
    } else {
        prefixSum[pos + 1] = prefixSum[pos] + 32;
        /*others*/
    }

    /*
    do not forget to put '\0' at the end of the processed instruction
    */

    strcat(instructions[pos], "\n"); /* add \n*/
}

int compress_add_mv(int pos) {
    char rs2[6] = "00000"; /*tmp char[] to store something*/
    char rs1[6] = "00000";
    char rd[6] = "00000"; /*tmp char[] to store something*/
    memcpy(rs2, instructions[pos] + 7, 5); /*copy register*/
    memcpy(rs1, instructions[pos] + 12, 5);
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    if (memcmp(rd, "00000", 5) && memcmp(rs2, "00000", 5)) {
        if (!strncmp(rd, rs1, 5)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            dest = (char *)malloc(sizeof(char) * (33));
            memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
            strcpy(dest, "1001");
            strcat(dest, rs1); /*make split blocks joint*/
            strcat(dest, rs2); /*make split blocks joint*/
            strcat(dest, "10");
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        } else if (!memcmp(rs1, "00000", 5)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            dest = (char *)malloc(sizeof(char) * (33));
            memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
            strcpy(dest, "1000");
            strcat(dest, rd); /*make split blocks joint*/
            strcat(dest, rs2); /*make split blocks joint*/
            strcat(dest, "10"); /*make split blocks joint*/
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_jalr_jr(int pos) {
    char rs2[6] = "00000"; /*tmp char[] to store something*/
    char rs1[6] = "00000";
    char rd[6] = "00000"; /*tmp char[] to store something*/
    char imm[12] = "0"; /*tmp char[] to store something*/
    memcpy(imm, instructions[pos], 12); /*copy imm*/
    memcpy(rs1, instructions[pos] + 12, 5);
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    if(!memcmp(imm, "000000000000", 12)){
        if (memcmp(rs1, "00000", 5)) {
            if (!memcmp(rd, "00001", 5)) { /*jalr*/
                char *dest; /* get new dest to replace instructions[pos]*/
                dest = (char *)malloc(sizeof(char) * (33));
                memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
                strcpy(dest, "1001"); /*make split blocks joint*/
                strcat(dest, rs1);
                strcat(dest, rs2); /*make split blocks joint*/
                strcat(dest, "10");
                free(instructions[pos]); /* get new dest to replace instructions[pos]*/
                instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
                return 16;
            } else if(!memcmp(rd, "00000", 5)){ /*jr*/
                char *dest; /* get new dest to replace instructions[pos]*/
                dest = (char *)malloc(sizeof(char) * (33));
                memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
                strcpy(dest, "1000");
                strcat(dest, rs1); /*make split blocks joint*/
                strcat(dest, rs2); /*make split blocks joint*/
                strcat(dest, "10");
                free(instructions[pos]); /* get new dest to replace instructions[pos]*/
                instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
                return 16;
            }
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_addi_li(int pos) {
    char imm[13] = "0"; /*tmp char[] to store something*/
    char imms[2] = "0";
    char immc[6] = "00000"; /*tmp char[] to store something*/
    char rd[6] = "00000";
    char rs1[6] = "00000"; /*tmp char[] to store something*/
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    memcpy(imm, instructions[pos], 12); 
    memcpy(rs1, instructions[pos] + 12, 5); /*copy register*/
    if (memcmp(rd, "00000", 5)) {
        /*
        if(!memcmp(imm, "000000000000", 12)){
            if(memcmp(rs1, "00000", 5)){
                char* dest;
                dest = (char *)malloc(sizeof(char) * (33));
                memset(dest, '\0', 33);
                strcpy(dest, "1000");
                strcat(dest, rd);
                strcat(dest, rs1);
                strcat(dest, "10");
                free(instructions[pos]);
                instructions[pos] = dest;
                return 16;
            }
        }*/ /* why???? mv use add?? the green card says addi??*/

        if (!memcmp(imm, "0000000", 7) || !memcmp(imm, "1111111", 7)) {
            memcpy(imms, imm + 6, 1); /*copy imm*/
            memcpy(immc, imm + 7, 5);
            if (!memcmp(rd, rs1, 5) &&
                memcmp(imm, "000000000000", 12)) { /*addi*/
                char *dest; /* get new dest to replace instructions[pos]*/
                dest = (char *)malloc(sizeof(char) * (33)); 
                memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
                strcpy(dest, "000");
                strcat(dest, imms); /*make split blocks joint*/
                strcat(dest, rd);
                strcat(dest, immc); /*make split blocks joint*/
                strcat(dest, "01");
                free(instructions[pos]); /* get new dest to replace instructions[pos]*/
                instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
                return 16;
            } else if (!memcmp(rs1, "00000", 5)) { /*li*/
                char *dest; /* get new dest to replace instructions[pos]*/
                dest = (char *)malloc(sizeof(char) * (33));
                memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
                strcpy(dest, "010");
                strcat(dest, imms); /*make split blocks joint*/
                strcat(dest, rd);
                strcat(dest, immc); /*make split blocks joint*/
                strcat(dest, "01");
                free(instructions[pos]); /* get new dest to replace instructions[pos]*/
                instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
                return 16;
            }
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_lui(int pos) {
    char imm[21] = "0"; /*tmp char[] to store something*/
    char imms[2] = "0";
    char immc[6] = "00000"; /*tmp char[] to store something*/
    char rd[6] = "00000"; /*tmp char[] to store something*/
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    memcpy(imm, instructions[pos], 20);
    if (memcmp(imm, "00000000000000000000", 20) && memcmp(rd, "00000", 5) &&
        memcmp(rd, "00010", 5)) { /* check*/
        if (!memcmp(imm, "000000000000000", 15) || !memcmp(imm, "111111111111111", 15)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            dest = (char *)malloc(sizeof(char) * (33));
            memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
            memcpy(imms, imm + 14, 1);
            memcpy(immc, imm + 15, 5); /*make split blocks joint*/
            strcpy(dest, "011");
            strcat(dest, imms); /*make split blocks joint*/
            strcat(dest, rd);
            strcat(dest, immc); /*make split blocks joint*/
            strcat(dest, "01");
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_slli(int pos) {
    char imm[13] = "0"; /*tmp char[] to store something*/
    char immc[6] = "00000";
    char rd[6] = "00000"; /*tmp char[] to store something*/
    char rs1[6] = "00000";
    memcpy(rs1, instructions[pos] + 12, 5); /*copy register*/
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    memcpy(imm, instructions[pos], 12);
    if (!memcmp(imm, "0000000", 7) && !memcmp(rd, rs1, 5) &&
        memcmp(rd, "00000", 5)) {
        char *dest; /* get new dest to replace instructions[pos]*/
        dest = (char *)malloc(sizeof(char) * (33));
        memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
        memcpy(immc, imm + 7, 5);
        strcpy(dest, "0000"); /*make split blocks joint*/
        strcat(dest, rd);
        strcat(dest, immc); /*make split blocks joint*/
        strcat(dest, "10");
        free(instructions[pos]); /* get new dest to replace instructions[pos]*/
        instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
        return 16;
    }
    return 32; /*return unchanged 32*/
}

int compress_lw(int pos) {
    char imm[13] = "0"; /*tmp char[] to store something*/
    char immc_1[2] = "0";
    char immc_2[2] = "0"; /*tmp char[] to store something*/
    char immc[4] = "000";
    char rd_1[4] = "000"; /*tmp char[] to store something*/
    char rs1_1[4] = "000";
    memcpy(imm, instructions[pos], 12);
    if (!memcmp(imm, "00000", 5)) { /* check*/
        if (!memcmp(imm + 10, "00", 2) &&
            !memcmp(instructions[pos] + 12, "01", 2) && /*check whether between x8 - x15*/
            !memcmp(instructions[pos] + 20, "01", 2)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            memcpy(rd_1, instructions[pos] + 22, 3);
            memcpy(rs1_1, instructions[pos] + 14, 3);
            memcpy(immc_1, imm + 9, 1); /*copy info*/
            memcpy(immc_2, imm + 5, 1);
            memcpy(immc, imm + 6, 3); /*copy info*/
            dest = (char *)malloc(sizeof(char) * (33));
            memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
            strcpy(dest, "010"); /*make split blocks joint*/
            strcat(dest, immc);
            strcat(dest, rs1_1); /*make split blocks joint*/
            strcat(dest, immc_1);
            strcat(dest, immc_2); /*make split blocks joint*/
            strcat(dest, rd_1);
            strcat(dest, "00"); /*make split blocks joint*/
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_sw(int pos) {
    char imm_1[8] = "0"; /*tmp char[] to store something*/
    char imm_2[6] = "000000";
    char immc_1[2] = "0"; /*tmp char[] to store something*/
    char immc_2[2] = "0";
    char immc_3[2] = "0"; /*tmp char[] to store something*/
    char immc_4[3] = "00";
    char rs2_1[4] = "000"; /*tmp char[] to store something*/
    char rs1_1[4] = "000"; /*tmp char[] to store something*/
    memcpy(imm_1, instructions[pos], 7);
    memcpy(imm_2, instructions[pos] + 20, 5); /*copy imm*/
    /*strcat(imm_1, imm_2); wrong!!*/
    if (!memcmp(imm_1, "00000", 5)) { /* check*/
        if (!memcmp(imm_2 + 3, "00", 2) &&
            !memcmp(instructions[pos] + 7, "01", 2) &&
            !memcmp(instructions[pos] + 12, "01", 2)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            memcpy(rs2_1, instructions[pos] + 9, 3);
            memcpy(rs1_1, instructions[pos] + 14, 3); /*copy info*/
            memcpy(immc_1, imm_2 + 2, 1);
            memcpy(immc_2, imm_1 + 5, 1); /*copy info*/
            memcpy(immc_3, imm_1 + 6, 1);
            memcpy(immc_4, imm_2, 2); /*copy info*/
            dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
            memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
            strcpy(dest, "110");
            strcat(dest, immc_3); /*make split blocks joint*/
            strcat(dest, immc_4);
            strcat(dest, rs1_1); /*make split blocks joint*/
            strcat(dest, immc_1); /*make split blocks joint*/
            strcat(dest, immc_2);
            strcat(dest, rs2_1); /*make split blocks joint*/
            strcat(dest, "00");
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_and(int pos) {
    char rd[6] = "00000"; /*tmp char[] to store something*/
    char rs1[6] = "00000";
    char rs2[6] = "00000"; /*tmp char[] to store something*/
    char rd_1[4] = "000";
    char rs2_1[4] = "000"; /*tmp char[] to store something*/
    memcpy(rs2, instructions[pos] + 7, 5); /*copy register*/
    memcpy(rs1, instructions[pos] + 12, 5); /*copy register*/
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    if (!memcmp(rs1, rd, 5)) {
        if (!memcmp(rd, "01", 2) && !memcmp(rs2, "01", 2)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            memcpy(rd_1, rd + 2, 3);
            memcpy(rs2_1, rs2 + 2, 3);
            dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
            memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
            strcat(dest, "100011"); 
            strcat(dest, rd_1); /*make split blocks joint*/
            strcat(dest, "11");
            strcat(dest, rs2_1); /*make split blocks joint*/
            strcat(dest, "01");
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_or(int pos) {
    char rd[6] = "00000"; /*tmp char[] to store something*/
    char rs1[6] = "00000";
    char rs2[6] = "00000"; /*tmp char[] to store something*/
    char rd_1[4] = "000";
    char rs2_1[4] = "000"; /*tmp char[] to store something*/
    memcpy(rs2, instructions[pos] + 7, 5); /*copy register*/
    memcpy(rs1, instructions[pos] + 12, 5);
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    if (!memcmp(rs1, rd, 5)) {
        if (!memcmp(rd, "01", 2) && !memcmp(rs2, "01", 2)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            memcpy(rd_1, rd + 2, 3);
            memcpy(rs2_1, rs2 + 2, 3);
            dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
            memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
            strcat(dest, "100011");
            strcat(dest, rd_1); /*make split blocks joint*/
            strcat(dest, "10");
            strcat(dest, rs2_1); /*make split blocks joint*/
            strcat(dest, "01");
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_xor(int pos) {
    char rd[6] = "00000"; /*tmp char[] to store something*/
    char rs1[6] = "00000"; 
    char rs2[6] = "00000"; /*tmp char[] to store something*/
    char rd_1[4] = "000";
    char rs2_1[4] = "000"; /*tmp char[] to store something*/
    memcpy(rs2, instructions[pos] + 7, 5); /*copy register*/
    memcpy(rs1, instructions[pos] + 12, 5);
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    if (!memcmp(rs1, rd, 5)) {
        if (!memcmp(rd, "01", 2) && !memcmp(rs2, "01", 2)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            memcpy(rd_1, rd + 2, 3);
            memcpy(rs2_1, rs2 + 2, 3);
            dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
            memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
            strcat(dest, "100011");
            strcat(dest, rd_1); /*make split blocks joint*/
            strcat(dest, "01");
            strcat(dest, rs2_1); /*make split blocks joint*/
            strcat(dest, "01");
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_sub(int pos) {
    char rd[6] = "00000"; /*tmp char[] to store something*/
    char rs1[6] = "00000";
    char rs2[6] = "00000"; /*tmp char[] to store something*/
    char rd_1[4] = "000";
    char rs2_1[4] = "000"; /*tmp char[] to store something*/
    memcpy(rs2, instructions[pos] + 7, 5); /*copy register*/
    memcpy(rs1, instructions[pos] + 12, 5);
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    if (!memcmp(rs1, rd, 5)) {
        if (!memcmp(rd, "01", 2) && !memcmp(rs2, "01", 2)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            memcpy(rd_1, rd + 2, 3);
            memcpy(rs2_1, rs2 + 2, 3);
            dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
            memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
            strcat(dest, "100011");
            strcat(dest, rd_1); /*make split blocks joint*/
            strcat(dest, "00");
            strcat(dest, rs2_1); /*make split blocks joint*/
            strcat(dest, "01");
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_beq(int pos) { /*incomplete version*/
    char rs1[6] = "00000"; /*tmp char[] to store something*/
    char rs1_1[4] = "000";
    char rs2[6] = "00000"; /*tmp char[] to store something*/
    memcpy(rs2, instructions[pos] + 7, 5);
    memcpy(rs1, instructions[pos] + 12, 5); /*copy register*/
    if (!memcmp(rs2, "00000", 5) && !memcmp(rs1, "01", 2)) {
        char *dest; /* get new dest to replace instructions[pos]*/
        memcpy(rs1_1, rs1 + 2, 3);
        dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
        memset(dest, '\0', 33);
        strcpy(dest, "110");
        strcat(dest, "000"); /*wrong offset*/
        strcat(dest, rs1_1); /*make split blocks joint*/
        strcat(dest, "00000"); /*wrong offset*/
        strcat(dest, "01");
        free(instructions[pos]); /* get new dest to replace instructions[pos]*/
        instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
        return 16;
    }
    return 32; /*return unchanged 32*/
}

int compress_bne(int pos) { /*incomplete version*/
    char rs1[6] = "00000"; /*tmp char[] to store something*/
    char rs1_1[4] = "000";
    char rs2[6] = "00000"; /*tmp char[] to store something*/
    memcpy(rs2, instructions[pos] + 7, 5);
    memcpy(rs1, instructions[pos] + 12, 5); /*copy register*/
    if (!memcmp(rs2, "00000", 5) && !memcmp(rs1, "01", 2)) {
        char *dest; /* get new dest to replace instructions[pos]*/
        memcpy(rs1_1, rs1 + 2, 3);
        dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
        memset(dest, '\0', 33);
        strcpy(dest, "111");
        strcat(dest, "000"); /*wrong offset*/
        strcat(dest, rs1_1); /*make split blocks joint*/
        strcat(dest, "00000"); /*wrong offset*/
        strcat(dest, "01");
        free(instructions[pos]); /* get new dest to replace instructions[pos]*/
        instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
        return 16;
    }
    return 32; /*return unchanged 32*/
}

int compress_srli(int pos) {
    char imm[13] = "0"; /*tmp char[] to store something*/
    char immc[6] = "00000";
    char rd[6] = "00000"; /*tmp char[] to store something*/
    char rs1[6] = "00000";
    char rd_1[4] = "000"; /*tmp char[] to store something*/
    memcpy(rs1, instructions[pos] + 12, 5); /*copy register*/
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    memcpy(imm, instructions[pos], 12);
    if (!memcmp(imm, "0000000", 7) && !memcmp(rd, rs1, 5)) {
        if (!memcmp(rd, "01", 2)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            memcpy(rd_1, rd + 2, 3);
            dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
            memset(dest, '\0', 33);
            memcpy(immc, imm + 7, 5);
            strcpy(dest, "1000"); 
            strcat(dest, "00"); /*make split blocks joint*/
            strcat(dest, rd_1);
            strcat(dest, immc); /*make split blocks joint*/
            strcat(dest, "01");
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_srai(int pos) {
    char imm[13] = "0"; /*tmp char[] to store something*/
    char immc[6] = "00000";
    char rd[6] = "00000"; /*tmp char[] to store something*/
    char rs1[6] = "00000";
    char rd_1[4] = "000"; /*tmp char[] to store something*/
    memcpy(rs1, instructions[pos] + 12, 5); /*copy register*/
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    memcpy(imm, instructions[pos], 12);
    if (!memcmp(imm, "0100000", 7) && !memcmp(rd, rs1, 5)) {
        if (!memcmp(rd, "01", 2)) {
            char *dest; /* get new dest to replace instructions[pos]*/
            memcpy(rd_1, rd + 2, 3);
            dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
            memset(dest, '\0', 33);
            memcpy(immc, imm + 7, 5);
            strcpy(dest, "1000"); /*make split blocks joint*/
            strcat(dest, "01");
            strcat(dest, rd_1); /*make split blocks joint*/
            strcat(dest, immc); /*make split blocks joint*/
            strcat(dest, "01");
            free(instructions[pos]); /* get new dest to replace instructions[pos]*/
            instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
            return 16;
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_andi(int pos) {
    char imm[13] = "0"; /*tmp char[] to store something*/
    char immc[6] = "00000"; 
    char imms[2] = "0"; /*tmp char[] to store something*/
    char rd[6] = "00000";
    char rs1[6] = "00000"; /*tmp char[] to store something*/
    char rd_1[4] = "000"; /*tmp char[] to store something*/
    memcpy(rs1, instructions[pos] + 12, 5);
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    memcpy(imm, instructions[pos], 12);
    if (!memcmp(imm, "0000000", 7) || !memcmp(imm, "1111111", 7)) { /*check*/
        if (!memcmp(rd, rs1, 5)) {
            if (!memcmp(rd, "01", 2)) {
                char *dest; /* get new dest to replace instructions[pos]*/
                memcpy(rd_1, rd + 2, 3);
                dest = (char *)malloc(sizeof(char) * (33)); /* get new dest to replace instructions[pos]*/
                memset(dest, '\0', 33);
                memcpy(immc, imm + 7, 5);/*copy info*/
                memcpy(imms, imm + 6, 1);
                strcpy(dest, "100"); /*make split blocks joint*/
                strcat(dest, imms);
                strcat(dest, "10"); /*make split blocks joint*/
                strcat(dest, rd_1); /*make split blocks joint*/
                strcat(dest, immc); /*make split blocks joint*/
                strcat(dest, "01");
                free(instructions[pos]); /* get new dest to replace instructions[pos]*/
                instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
                return 16;
            }
        }
    }
    return 32; /*return unchanged 32*/
}

int compress_jal_j(int pos) { /*incomplete version*/
    /*char imm[12] = "0";*/
    char rd[5] = "00000"; /*tmp char[] to store something*/
    memcpy(rd, instructions[pos] + 20, 5); /*copy register*/
    if (!memcmp(rd, "00000", 5)) { /*j*/
        char *dest; /* get new dest to replace instructions[pos]*/
        dest = (char *)malloc(sizeof(char) * (33));
        memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
        strcpy(dest, "101");
        strcat(dest, "00000000000"); /*wrong offset*/
        strcat(dest, "01");
        free(instructions[pos]); /* get new dest to replace instructions[pos]*/
        instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
        return 16;
    } else if (!memcmp(rd, "00001", 5)) { /*jal*/
        char *dest; /* get new dest to replace instructions[pos]*/
        dest = (char *)malloc(sizeof(char) * (33));
        memset(dest, '\0', 33); /* get new dest to replace instructions[pos]*/
        strcpy(dest, "001");
        strcat(dest, "00000000000"); /*wrong offset*/
        strcat(dest, "01");
        free(instructions[pos]); /* get new dest to replace instructions[pos]*/
        instructions[pos] = dest; /* get new dest to replace instructions[pos]*/
        return 16;
    }
    return 32; /*return unchanged 32*/
}

void jumpDistanceJ(int pos) {
    char imm[20];
    memcpy(imm + 19 - 19, instructions[pos] + 31 - 31, 1); /*copy right split imm*/
    memcpy(imm + 19 - 9, instructions[pos] + 31 - 30, 10);
    memcpy(imm + 19 - 10, instructions[pos] + 31 - 20, 1); /*copy right split imm*/
    memcpy(imm + 19 - 18, instructions[pos] + 31 - 19, 8);

    jumpAt[jumpNum] = pos;
    /*whether divided by 2 is not sure, (my fault), when debugging ,take notice
     */
    jumpDistance[jumpNum] = binToDec(imm, 20) / 2; /* get distance*/

    jumpType[jumpNum] = typeJ;

    ++jumpNum;
}

void jumpDistanceB(int pos, int beq) {
    char imm[12]; /*tmp imm*/
    imm[0] = instructions[pos][0];
    imm[1] = instructions[pos][24];
    memcpy(imm + 2, instructions[pos] + 1, 6); /*copy right split imm*/
    memcpy(imm + 8, instructions[pos] + 20, 4); /*copy right split imm*/

    jumpAt[jumpNum] = pos;
    /*whether divided by 2 is not sure,(my falut), when debugging ,take notice
     */
    jumpDistance[jumpNum] = binToDec(imm, 12) / 2;

    if (beq == 1) { /*judge type according to beq*/
        jumpType[jumpNum] = typeBeq;
    } else if (beq == -1) { /*judge type according to beq*/
        jumpType[jumpNum] = typeO;
    } else { /*judge type according to beq*/
        jumpType[jumpNum] = typeBne;
    }

    ++jumpNum;
}

void relocate() { /*reset the imm*/
    int i, offset, pos;
    char *bin;
    char *instruction; /*tmp char* */
    offset = 0;
    ++offset; /*no meaning, just to pass warning*/
    for (i = 0; i < jumpNum; ++i) {
        pos = jumpAt[i];
        instruction = instructions[pos];
        /* divided by 8 to get bytes, and multiplied by 2 to get offset */
        /* another point to take care when debug*/

        /*when debug,try following:
        offset = (prefixSum[pos + jumpDistance[i] + 1] - prefixSum[pos + 1]) / 8
        / 2; offset = (prefixSum[pos + jumpDistance[i]] - prefixSum[pos + 1]) /
        8 / 2; offset = (prefixSum[pos + jumpDistance[i] + 1] - prefixSum[pos])
        / 8 / 2;
        */
        offset = (prefixSum[pos + jumpDistance[i]] - prefixSum[pos]) / 8 / 2;

        /*remember there is a '\n' in the end*/
        if (strlen(instruction) == 17) {
            if ((jumpType[i] == typeBeq) || (jumpType[i] == typeBne)) {
                bin = DecToBin(offset, 8);
                instruction[3] = bin[0]; /*set the correct imm*/
                instruction[4] = bin[4];
                instruction[5] = bin[5]; /*set the correct imm*/

                instruction[9] = bin[1]; /*set the correct imm*/
                instruction[10] = bin[2];
                instruction[11] = bin[6]; /*set the correct imm*/
                instruction[12] = bin[7];
                instruction[13] = bin[3]; /*set the correct imm*/

            } else { /*typeJ*/
                bin = DecToBin(offset, 11);
                instruction[3] = bin[0]; /*set the correct imm*/
                instruction[4] = bin[7];
                instruction[5] = bin[2]; /*set the correct imm*/
                instruction[6] = bin[3];
                instruction[7] = bin[1]; /*set the correct imm*/
                instruction[8] = bin[5];
                instruction[9] = bin[4]; /*set the correct imm*/
                instruction[10] = bin[8];
                instruction[11] = bin[9]; /*set the correct imm*/
                instruction[12] = bin[10];
                instruction[13] = bin[6]; /*set the correct imm*/
            }
        } else { /*32 bit instruction*/
            if ((jumpType[i] == typeBeq) || (jumpType[i] == typeBne) ||
                (jumpType[i] == typeO)) {
                bin = DecToBin(offset, 12);
                instruction[31 - 31] = bin[11 - 11]; /*set the correct imm*/
                memcpy(instruction + 31 - 30, bin + 11 - 9, 6); /*set the correct imm*/
                memcpy(instruction + 31 - 11, bin + 11 - 3, 4); /*set the correct imm*/
                instruction[31 - 7] = bin[11 - 10]; /*set the correct imm*/

            } else {
                bin = DecToBin(offset, 20);
                instruction[31 - 31] = bin[19 - 19]; /*set the correct imm*/
                memcpy(instruction + 31 - 30, bin + 19 - 9, 10);
                instruction[31 - 20] = bin[19 - 10]; /*set the correct imm*/
                memcpy(instruction + 31 - 19, bin + 19 - 18, 8); /*set the correct imm*/
            }
        }

        free(bin);
    }
}
