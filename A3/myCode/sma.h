/*
 * =====================================================================================
 *
 *  Filename:  		sma.h
 *
 *  Description:	Header file for SMA.
 *
 *  Version:  		1.0
 *  Created:  		3/11/2020 9:30:00 AM
 *  Revised:  		-
 *  Compiler:  		gcc
 *
 *  Author:  		Mohammad Mushfiqur Rahman
 *      
 *  Instructions:   Please address all the "TODO"s in the code below and modify them
 *                  accordingly. Refer to the Assignment Handout for further info.
 * =====================================================================================
 */

/* Includes */
//  TODO: Add any libraries you might use here.
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//  Policies definition
#define WORST_FIT	1
#define NEXT_FIT	2

extern char *sma_malloc_error;

//  Public Functions declaration
void *sma_malloc(int size);
void sma_free(void* ptr);
void sma_mallopt(int policy);
void sma_mallinfo();
void *sma_realloc(void *ptr, int size);

//  Private Functions declaration
static void* allocate_pBrk(int size);
static void* allocate_freeList(int size);
static void* allocate_worst_fit(int size);
static void* allocate_next_fit(int size);
static void allocate_block(void* newBlock, int size, int excessSize, int fromFreeList);
static void replace_block_freeList(void *oldBlock, void *newBlock, void *P, void *N, int size);
static void add_block_freeList(void* block);
static void remove_block_freeList(void* block);
static int get_blockSize(void *ptr);
static int get_largest_freeBlock();
//  TODO: Declare any private functions that you intend to add in your code.
static void set_blockSize(void *ptr, int size);
static int get_blockType(void *ptr);
static void set_blockType(void *ptr, int type);
static unsigned long get_blockN(void *ptr);
static void set_blockN(void *ptr, unsigned long address);
static unsigned long get_blockP(void *ptr);
static void set_blockP(void *ptr, unsigned long address);