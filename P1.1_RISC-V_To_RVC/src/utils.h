#ifndef UTILS_H
#define UTILS_H

/*otherwise, FILE is not found -_-*/
#include <stdio.h>

#define MAX_INPUT_SCALE 60

/**
 * @param input ,readable file stream of the RISC-V codes
 * @param output ,writerable file stream of the compressed RISC-V codes
 * this function serves as the central controller of the project
 */
void interface(FILE **input, FILE **output);

/**
 * garbage collector
 */
void gc();

/**
 * use 2's complement to fit negative number
 * @param bin a string of binary number, e.g. "0110"
 * @param length length of the string, e.g. 4
 * @return  decimal value of the binary number, e.g. 6
 */
int binToDec(char *bin, int length);

/**
 * use 2's complement to fit negative number
 * @param val,a decimal number, e.g.10
 * @param length the length of the reutrn string, e.g. 5
 * @return  a string of binary number, e.g. "01010"
 */
char *DecToBin(int val, int length);

/**
 * note that exp >= 0 and base >=1
 * @return base^exp
 */
int pow_(int base, int exp);

#endif