#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compression.h"

char **instructions; /*ordered collection of instructions */
int insSize = 0;     /*size of instructions */

/** the prefix sum array for the length of every processed instrucion,
 * it is designed for compress jal command in O(1) time complexity
 * index of instructions starts at 1, not 0, to better deal with prefix sum
 */
int prefixSum[MAX_INPUT_SCALE] = {0};

void interface(FILE **input, FILE **output) {
    int bufferSize; /*tmp*/
    char *instruction; /*tmp*/
    int i;
    /* 60 is the the max input lines  */
    instructions = (char **)malloc(sizeof(char *) * MAX_INPUT_SCALE);

    bufferSize = 100;

    /*load a string with 100 chars,
    but only need to process first 32 chars
    since fgets() will return when fp meets '\n' or EOF
     */
    instruction = (char *)malloc(sizeof(char) * (bufferSize + 1));
    /* read from input */
    while (fgets(instruction, bufferSize, *input)) {
        /*empty line contains ' ' or '\t' */
        if ((instruction[0] != '0') && (instruction[0] != '1')) {
            continue;
        }
        instruction[32] = '\0'; /*end of instruction*/

        instructions[insSize++] = instruction;
        compressor(insSize - 1); /*do compress*/
        instruction = (char *)malloc(sizeof(char) * (bufferSize + 1));
    }
    free(instruction); /*free the memory*/

    /* deal with jal command below */
    relocate();

    /*write processed instructions into file */
    for (i = 0; i < insSize; ++i) {
        fputs(instructions[i], *output);
    }

    gc(); /*garbage collect*/
}

void gc() { /*garbage collector*/
    int i;
    for (i = 0; i < insSize; ++i) {
        free(instructions[i]); /*free the memory*/
    }
    free(instructions); /*free the memory*/
}

int binToDec(char *bin, int length) { /* convert bin to dec*/
    int level, ans, numBits;
    numBits = length; /*tmp*/
    ans = 0;
    level = 1; /*initialize*/
    --length;
    while (length >= 0) { /*positive*/
        if (bin[length] - '0') {
            ans += level;
        } /* do the convertion*/

        level <<= 1;
        --length;
    }
    /*negative*/
    if (bin[0] == '1') {
        ans -= (int)pow_(2, numBits); /*do the convertion*/
    }

    return ans;
}

char *DecToBin(int val, int length) { /* convert dec to bin*/
    char *ans;
    int comp, i;

    if (val < 0) { /*negative*/
        val += (int)pow_(2, length);
    }

    ans = (char *)malloc(sizeof(char) * length); /*get the memory*/
    --length;
    i = 0;
    comp = 1;
    comp <<= length; /*shift left length*/

    while (length >= 0) {
        if ((comp & val) == comp) {
            ans[i] = '1';
        } else { /* do the convertion*/
            ans[i] = '0';
        }
        ++i;
        --length;
        comp >>= 1; /*shift right 1*/
    }

    return ans;
}

int pow_(int base, int exp) { /* power*/
    int ans = 1;
    if (exp == 0) return 1; /*exp = 0, power is 1*/
    while (exp) { /* do the pow*/
        ans *= base;
        --exp;
    }
    return ans; /* return the power*/
}
