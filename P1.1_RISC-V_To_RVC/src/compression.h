#ifndef COMPRESSION_H
#define COMPRESSION_H

/**
 * @param pos, the index of instrucitons
 * check whether instructions[pos] is compressible,
 * if it is ,compress it in place,
 * otherwise, do nothing
 * put the length of every instruction after compression in prefixSum
 */
void compressor(int pos);

/**
 * comments to all the compress_xxx form functions below
 * @return int, length of the instruction
 * if not compressed, return 32
 * else return 16
 */

int compress_add_mv(int pos); /*compress function*/
int compress_jalr_jr(int pos);
int compress_addi_li(int pos);
int compress_lui(int pos); /*compress function*/
int compress_slli(int pos);
int compress_lw(int pos);
int compress_sw(int pos); /*compress function*/
int compress_and(int pos);
int compress_or(int pos);
int compress_xor(int pos); /*compress function*/
int compress_sub(int pos);
int compress_beq(int pos);
int compress_bne(int pos); /*compress function*/
int compress_srli(int pos);
int compress_srai(int pos);
int compress_andi(int pos); /*compress function*/
int compress_jal_j(int pos);

void jumpDistanceJ(int pos); /*compress function*/

void jumpDistanceB(int pos, int beq);

/**
 * process jal instructions,
 * recalculate the jump distance(with the info in prefixSum),
 */
void relocate();

#endif