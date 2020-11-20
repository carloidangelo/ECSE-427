/*
 * =====================================================================================
 *
 *	Filename:  		sma.c
 *
 *  Description:	Base code for Assignment 3 for ECSE-427 / COMP-310
 *
 *  Version:  		1.0
 *  Created:  		6/11/2020 9:30:00 AM
 *  Revised:  		-
 *  Compiler:  		gcc
 *
 *  Author:  		Mohammad Mushfiqur Rahman
 *      
 *  Instructions:   Please address all the "TODO"s in the code below and modify 
 * 					them accordingly. Feel free to modify the "PRIVATE" functions.
 * 					Don't modify the "PUBLIC" functions (except the TODO part), unless
 * 					you find a bug! Refer to the Assignment Handout for further info.
 * =====================================================================================
 */

/* Includes */
#include "sma.h" // Please add any libraries you plan to use inside this file

/* Definitions*/
#define MAX_TOP_FREE (128 * 1024) // Max top free block size = 128 Kbytes
//	TODO: Change the Header size if required
// Header SIze = Bottom Header + Top Header
#define FREE_BLOCK_HEADER_SIZE  32 * sizeof(char) // Size of the Header in a free memory block
//	TODO: Add constants here
#define FREE_BLOCK_BOTTOM_HEADER_SIZE  8 * sizeof(char) // Size of the bottom Header in a free memory block

typedef enum //	Policy type definition
{
	WORST,
	NEXT
} Policy;

char *sma_malloc_error;
void *freeListHead = NULL;			  //	The pointer to the HEAD of the doubly linked free memory list
void *freeListTail = NULL;			  //	The pointer to the TAIL of the doubly linked free memory list
unsigned long totalAllocatedSize = 0; //	Total Allocated memory in Bytes
unsigned long totalFreeSize = 0;	  //	Total Free memory in Bytes in the free memory list
Policy currentPolicy = WORST;		  //	Current Policy
//	TODO: Add any global variables here
int totalFreeLength = 0;              //    Number of free memory blocks in free memory list
void *nextFitPointer = NULL;		  //    Pointer used in Next fit
/*
 * =====================================================================================
 *	Public Functions for SMA
 * =====================================================================================
 */

/*
 *	Funcation Name: sma_malloc
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates a memory block of input size from the heap, and returns a 
 * 					pointer pointing to it. Returns NULL if failed and sets a global error.
 */
void *sma_malloc(int size)
{
	void *pMemory = NULL;

	// Checks if the free list is empty
	if (freeListHead == NULL)
	{
		// Allocate memory by increasing the Program Break
		pMemory = allocate_pBrk(size);
	}
	// If free list is not empty
	else
	{
		// Allocate memory from the free memory list
		pMemory = allocate_freeList(size);

		// If a valid memory could NOT be allocated from the free memory list
		if (pMemory == (void *)-2)
		{
			// Allocate memory by increasing the Program Break
			pMemory = allocate_pBrk(size);
		}
	}

	// Validates memory allocation
	if (pMemory < 0 || pMemory == NULL)
	{
		sma_malloc_error = "Error: Memory allocation failed!";
		return NULL;
	}

	// Updates SMA Info
	totalAllocatedSize += size;

	return pMemory;
}

/*
 *	Funcation Name: sma_free
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Deallocates the memory block pointed by the input pointer
 */
void sma_free(void *ptr)
{
	//	Checks if the ptr is NULL
	if (ptr == NULL)
	{
		puts("Error: Attempting to free NULL!");
	}
	//	Checks if the ptr is beyond Program Break
	else if (ptr > sbrk(0))
	{
		puts("Error: Attempting to free unallocated space!");
	}
	else
	{
		//	Adds the block to the free memory list
		add_block_freeList(ptr);
	}
}

/*
 *	Funcation Name: sma_mallopt
 *	Input type:		int
 * 	Output type:	void
 * 	Description:	Specifies the memory allocation policy
 */
void sma_mallopt(int policy)
{
	// Assigns the appropriate Policy
	if (policy == 1)
	{
		currentPolicy = WORST;
	}
	else if (policy == 2)
	{
		currentPolicy = NEXT;
	}
}

/*
 *	Funcation Name: sma_mallinfo
 *	Input type:		void
 * 	Output type:	void
 * 	Description:	Prints statistics about current memory allocation by SMA.
 */
void sma_mallinfo()
{
	//	Finds the largest Contiguous Free Space (should be the largest free block)
	int largestFreeBlock = get_largest_freeBlock();
	char str[60];

	//	Prints the SMA Stats
	sprintf(str, "Total number of bytes allocated: %lu", totalAllocatedSize);
	puts(str);
	sprintf(str, "Total free space: %lu", totalFreeSize);
	puts(str);
	sprintf(str, "Size of largest contigious free space (in bytes): %d", largestFreeBlock);
	puts(str);
}

/*
 *	Funcation Name: sma_realloc
 *	Input type:		void*, int
 * 	Output type:	void*
 * 	Description:	Reallocates memory pointed to by the input pointer by resizing the
 * 					memory block according to the input size.
 */
void *sma_realloc(void *ptr, int size)
{
	// TODO: 	Should be similar to sma_malloc, except you need to check if the pointer address
	//			had been previously allocated.
	// Hint:	Check if you need to expand or contract the memory. If new size is smaller, then
	//			chop off the current allocated memory and add to the free list. If new size is bigger
	//			then check if there is sufficient adjacent free space to expand, otherwise find a new block
	//			like sma_malloc.
	//			Should not accept a NULL pointer, and the size should be greater than 0.
}

/*
 * =====================================================================================
 *	Private Functions for SMA
 * =====================================================================================
 */

/*
 *	Funcation Name: allocate_pBrk
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory by increasing the Program Break
 */
void *allocate_pBrk(int size)
{
	void *newBlock = NULL;
	int excessSize;

	//	TODO: 	Allocate memory by incrementing the Program Break by calling sbrk() or brk()
	//	Hint:	Getting an exact "size" of memory might not be the best idea. Why?
	//			Also, if you are getting a larger memory, you need to put the excess in the free list
	
	excessSize = 0 * 1024; // 50 KB excess memory
	newBlock = sbrk((size + FREE_BLOCK_HEADER_SIZE) + excessSize);
	newBlock = (void *)((char *)newBlock + FREE_BLOCK_BOTTOM_HEADER_SIZE);
 
	//	Allocates the Memory Block
	allocate_block(newBlock, size, excessSize, 0);

	return newBlock;
}

/*
 *	Funcation Name: allocate_freeList
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory from the free memory list
 */
void *allocate_freeList(int size)
{
	void *pMemory = NULL;

	if (currentPolicy == WORST)
	{
		// Allocates memory using Worst Fit Policy
		pMemory = allocate_worst_fit(size);
	}
	else if (currentPolicy == NEXT)
	{
		// Allocates memory using Next Fit Policy
		pMemory = allocate_next_fit(size);
	}
	else
	{
		pMemory = NULL;
	}

	return pMemory;
}

/*
 *	Funcation Name: allocate_worst_fit
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory using Worst Fit from the free memory list
 */
void *allocate_worst_fit(int size)
{
	void *worstBlock = NULL;
	int excessSize;
	int blockFound = 0;

	//	TODO: 	Allocate memory by using Worst Fit Policy
	//	Hint:	Start off with the freeListHead and iterate through the entire list to 
	//			get the largest block
	int i;
	unsigned long currentAddress = (unsigned long)freeListHead;
	int currentBlockSize;
	int worstBlockSize = 0;
	for (i = 0; i < totalFreeLength; i++){ 
		currentBlockSize = get_blockSize((void *)currentAddress);
		if (currentBlockSize > size){
			blockFound = 1;
			if (worstBlockSize < currentBlockSize){
				worstBlockSize = currentBlockSize;
				worstBlock = (void *)currentAddress;
			}	
		}
		currentAddress = get_blockN((void *)currentAddress);
	}
	//	Checks if appropriate block is found.
	if (blockFound)
	{
		excessSize = worstBlockSize - size;
		//	Allocates the Memory Block
		allocate_block(worstBlock, size, excessSize, 1);
	}
	else
	{
		//	Assigns invalid address if appropriate block not found in free list
		worstBlock = (void *)-2;
	}
	return worstBlock;
}

/*
 *	Funcation Name: allocate_next_fit
 *	Input type:		int
 * 	Output type:	void*
 * 	Description:	Allocates memory using Next Fit from the free memory list
 */
void *allocate_next_fit(int size)
{
	void *nextBlock = NULL;
	int excessSize;
	int blockFound = 0;

	//	TODO: 	Allocate memory by using Next Fit Policy
	//	Hint:	You should use a global pointer to keep track of your last allocated memory address, and 
	//			allocate free blocks that come after that address (i.e. on top of it). Once you reach 
	//			Program Break, you start from the beginning of your heap, as in with the free block with
	//			the smallest address)

	int i;
	int currentBlockSize;
	unsigned long currentAddress;
	if (nextFitPointer == NULL){
		currentAddress = (unsigned long)freeListHead;
	}else {
		currentAddress = (unsigned long)nextFitPointer;
	}
	for (i = 0; i < totalFreeLength; i++){ 
		currentBlockSize = get_blockSize((void *)currentAddress);
		if (currentBlockSize > size){
			blockFound = 1;
			nextBlock = (void *)currentAddress;
			break;
		}
		currentAddress = get_blockN((void *)currentAddress);
		if (currentAddress == 0){
			currentAddress = (unsigned long)freeListHead;
		}
	}
	//	Checks if appropriate found is found.
	if (blockFound)
	{
		excessSize = currentBlockSize - size;
		//	Allocates the Memory Block
		allocate_block(nextBlock, size, excessSize, 1);
	}
	else
	{
		//	Assigns invalid address if appropriate block not found in free list
		nextBlock = (void *)-2;
	}

	return nextBlock;
}

/*
 *	Funcation Name: allocate_block
 *	Input type:		void*, int, int, int
 * 	Output type:	void
 * 	Description:	Performs routine operations for allocating a memory block
 */
void allocate_block(void *newBlock, int size, int excessSize, int fromFreeList)
{
	void *excessFreeBlock; //	pointer for any excess free block
	int addFreeBlock;

	// 	Checks if excess free size is big enough to be added to the free memory list
	//	Helps to reduce external fragmentation

	//	TODO: Adjust the condition based on your Head and Tail size (depends on your TAG system)
	//	Hint: Might want to have a minimum size greater than the Head/Tail sizes
	addFreeBlock = excessSize > FREE_BLOCK_HEADER_SIZE;
	//	If excess free size is big enough
	if (addFreeBlock)
	{
		//	TODO: Create a free block using the excess memory size, then assign it to the Excess Free Block
		char *pointer_temp = (char *)newBlock;
		pointer_temp = pointer_temp + (size + FREE_BLOCK_HEADER_SIZE);
		excessFreeBlock = (void *)pointer_temp;

		//	Checks if the new block was allocated from the free memory list
		if (fromFreeList)
		{
			void *P = (void *)get_blockP(newBlock);
			void *N = (void *)get_blockN(newBlock);
			int current_size = get_blockSize(newBlock);
			set_blockSize(excessFreeBlock, excessSize - FREE_BLOCK_HEADER_SIZE);
			set_blockType(excessFreeBlock, 0);
			set_blockSize(newBlock, size);
			set_blockType(newBlock, 1);

			//	Removes new block and adds the excess free block to the free list
			replace_block_freeList(newBlock, excessFreeBlock, P, N, current_size);
		}
		else
		{
			set_blockSize(excessFreeBlock, excessSize - FREE_BLOCK_HEADER_SIZE);
			set_blockType(excessFreeBlock, 0);
			set_blockSize(newBlock, size);
			set_blockType(newBlock, 1);
			//	Adds excess free block to the free list
			add_block_freeList(excessFreeBlock);
		}
	}
	//	Otherwise add the excess memory to the new block
	else
	{
		//	TODO: Add excessSize to size and assign it to the new Block
		set_blockSize(newBlock, size + excessSize);
		set_blockType(newBlock, 1);

		//	Checks if the new block was allocated from the free memory list
		if (fromFreeList)
		{
			//	Removes the new block from the free list
			remove_block_freeList(newBlock);
		}
	}
}

/*
 *	Funcation Name: replace_block_freeList
 *	Input type:		void*, void*
 * 	Output type:	void
 * 	Description:	Replaces old block with the new block in the free list
 */
void replace_block_freeList(void *oldBlock, void *newBlock, void *P, void *N, int size)
{
	//	TODO: Replace the old block with the new block
	if ((oldBlock == freeListHead) && (oldBlock == freeListTail)){
		freeListHead = newBlock;
		freeListTail = newBlock;
	}else if (oldBlock == freeListHead){
		set_blockP(N, (unsigned long)newBlock);
		freeListHead = newBlock;
	}else if (oldBlock == freeListTail){
		set_blockN(P, (unsigned long)newBlock);
		freeListTail = newBlock;
	}else{
		set_blockN(P, (unsigned long)newBlock);
		set_blockP(N, (unsigned long)newBlock);
	}
	set_blockN(newBlock, (unsigned long)N);
	set_blockP(newBlock, (unsigned long)P);
	nextFitPointer = newBlock;

	//	Updates SMA info
	totalFreeSize += -(get_blockSize(oldBlock) + FREE_BLOCK_HEADER_SIZE);
}

/*
 *	Funcation Name: add_block_freeList
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Adds a memory block to the the free memory list
 */
void add_block_freeList(void *block)
{
	//	TODO: 	Add the block to the free list
	//	Hint: 	You could add the free block at the end of the list, but need to check if there
	//			exits a list. You need to add the AGT to the list.
	//			Also, you would need to check if merging with the "adjacent" blocks is possible or not.
	//			Merging would be tideous. Check adjacent blocks, then also check if the merged
	//			block is at the top and is bigger than the largest free block allowed (128kB).
	if (get_blockType(block) == 0){ // check if the block is in-use or not
		if (freeListHead == NULL) { // check if there exists a list
			// update N and P
			set_blockN(block, 0); // 0 represents NULL
			set_blockP(block, 0); // 0 represents NULL
			freeListHead = block;
			freeListTail = block;
			totalFreeLength = totalFreeLength + 1;
		}else {
			// update current last free block in list
			set_blockN(freeListTail, (unsigned long)block);

			// update list by adding new last free block
			set_blockN(block, 0); // 0 represents NULL
			set_blockP(block, (unsigned long)freeListTail);
			freeListTail = block;
			totalFreeLength = totalFreeLength + 1;
		}
		//	Updates SMA info
		totalFreeSize += get_blockSize(block);
	}else if (get_blockType(block) == 1){
		// change block Type
		set_blockType(block, 0);
		if (freeListHead == NULL) { // check if there exists a list
			// update N and P
			set_blockN(block, 0); // 0 represents NULL
			set_blockP(block, 0); // 0 represents NULL
			freeListHead = block;
			freeListTail = block;
			totalFreeLength = totalFreeLength + 1;
			//	Updates SMA info
			totalAllocatedSize -= get_blockSize(block);
			totalFreeSize += get_blockSize(block);
		}else {
			// update free memory list
			unsigned long currentAddress = (unsigned long)freeListHead;
			int i;
			for (i = 0; i < totalFreeLength; i++){ // iterate through free memory list
				if (block < (void *)currentAddress){
					// update TAGs
					set_blockP(block, get_blockP((void *)currentAddress));
					set_blockN(block, currentAddress);
					if (currentAddress == (unsigned long)freeListHead){ // check if list Head should change
						freeListHead = block;
					}else {
						set_blockN((void *)get_blockP((void *)currentAddress), (unsigned long)block);
					}
					set_blockP((void *)currentAddress, (unsigned long)block);
					break;
				}
				currentAddress = get_blockN((void *)currentAddress);
			}
			if (i == totalFreeLength){ // block is last block in free memory list
				// update TAGs
				set_blockN(freeListTail, (unsigned long)block);
				set_blockN(block, 0);
				set_blockP(block, (unsigned long)freeListTail);
				freeListTail = block;
			}
			totalFreeLength = totalFreeLength + 1;
			//	Updates SMA info
			totalAllocatedSize -= get_blockSize(block);
			totalFreeSize += get_blockSize(block);

			// MERGE IF NEEDED
			unsigned long N, P, blockAddress;
			int NSize, PSize, blockSize;
			N = get_blockN(block);
			if (block == freeListTail){
				NSize = -1; // wrong size 
			}else{
				NSize = get_blockSize((void *)N);
			}

			P = get_blockP(block);
			if (block == freeListHead){
				PSize = -1; // wrong size
			}else{
				PSize = get_blockSize((void *)P);
			}
			blockAddress = (unsigned long)block;
			blockSize = get_blockSize(block);
			
			if ( ((N - blockAddress) == (blockSize + FREE_BLOCK_HEADER_SIZE))
			&& ((blockAddress - P) == (PSize + FREE_BLOCK_HEADER_SIZE)) ){
				unsigned long NN = get_blockN((void *)N);
				unsigned long PP = get_blockP((void *)P);

				// merge blocks
				set_blockSize((void *)P, NSize + PSize + blockSize + (2*FREE_BLOCK_HEADER_SIZE));
				set_blockType((void *)P, 0);
				set_blockP((void *)P, PP);
				set_blockN((void *)P, NN);

				if (freeListTail == (void *)N){
					freeListTail = (void *)P;
				}
				//	Updates SMA info
				totalFreeSize += 2*FREE_BLOCK_HEADER_SIZE;
				totalFreeLength = totalFreeLength - 2;
			}else if( (N - blockAddress) == (blockSize + FREE_BLOCK_HEADER_SIZE) ){
				unsigned long NN = get_blockN((void *)N);

				// merge blocks
				set_blockSize(block, NSize + blockSize + FREE_BLOCK_HEADER_SIZE);
				set_blockType(block, 0);
				set_blockP(block, P);
				set_blockN(block, NN);
				if (freeListTail == (void *)N){
					freeListTail = block;
				}
				if (nextFitPointer == (void *)N){
					nextFitPointer = freeListHead;
				}
				//	Updates SMA info
				totalFreeSize += FREE_BLOCK_HEADER_SIZE;
				totalFreeLength = totalFreeLength - 1;
			}else if ((blockAddress - P) == (PSize + FREE_BLOCK_HEADER_SIZE)){
				unsigned long PP = get_blockP((void *)P);

				// merge blocks
				set_blockSize((void *)P, PSize + blockSize + FREE_BLOCK_HEADER_SIZE);
				set_blockType((void *)P, 0);
				set_blockP((void *)P, PP);
				set_blockN((void *)P, N);
				if (freeListTail == block){
					freeListTail = (void *)P;
				}
				//	Updates SMA info
				totalFreeSize += FREE_BLOCK_HEADER_SIZE;
				totalFreeLength = totalFreeLength - 1;
			}
			
		}
	}
}

/*
 *	Funcation Name: remove_block_freeList
 *	Input type:		void*
 * 	Output type:	void
 * 	Description:	Removes a memory block from the the free memory list
 */
void remove_block_freeList(void *block)
{
	//	TODO: 	Remove the block from the free list
	//	Hint: 	You need to update the pointers in the free blocks before and after this block.
	//			You also need to remove any TAG in the free block.
	if ((block == freeListHead) && (block == freeListTail)){
		freeListHead = NULL;
		freeListTail = NULL;
		nextFitPointer = NULL;
	}else if (block == freeListHead){
		set_blockP((void *)get_blockN(block), 0); // 0 represents NULL
		freeListHead = (void *)get_blockN(block);
		nextFitPointer = freeListHead;
	}else if (block == freeListTail){
		set_blockN((void *)get_blockP(block), 0); // 0 represents NULL
		freeListTail = (void *)get_blockP(block);
		nextFitPointer = freeListHead;
	}else{
		set_blockN((void *)get_blockP(block), get_blockN(block));
		set_blockP((void *)get_blockN(block), get_blockP(block));
		nextFitPointer = (void *)get_blockN(block);
	}
	set_blockType(block, 1); // block is in-use

	//	Updates SMA info
	totalAllocatedSize += get_blockSize(block);
	totalFreeSize -= get_blockSize(block);
	totalFreeLength = totalFreeLength - 1;
}

/*
 *	Funcation Name: get_largest_freeBlock
 *	Input type:		void
 * 	Output type:	int
 * 	Description:	Extracts the largest Block Size
 */
int get_largest_freeBlock()
{
	int largestBlockSize = 0;
	//	TODO: Iterate through the Free Block List to find the largest free block and return its size
	if (totalFreeLength != 0){
		int i;
		unsigned long currentAddress = (unsigned long)freeListHead;
		int currentBlockSize;
		for (i = 0; i < totalFreeLength; i++){ 
			currentBlockSize = get_blockSize((void *)currentAddress);
			if (largestBlockSize < currentBlockSize){
				largestBlockSize = currentBlockSize;
			}
			currentAddress = get_blockN((void *)currentAddress);
		}
	}
	return largestBlockSize;
}

/*
 *	Funcation Name: get_blockSize
 *	Input type:		void*
 * 	Output type:	int
 * 	Description:	Extracts the Block Size
 */
int get_blockSize(void *ptr)
{
	int *pSize;

	//	Points to the address where the Size of the block is stored
	pSize = (int *)ptr;
	pSize--;

	return *pSize;
}

/*
 *	Funcation Name: set_blockSize
 *	Input type:		void* and int
 * 	Output type:	void
 * 	Description:	Sets the Block Size
 */
void set_blockSize(void *ptr, int size)
{
	int *pSize;
	pSize = (int *)ptr;
	pSize = pSize - 1;
	*pSize = size;
	pSize = pSize + 1;
	pSize = (int *)(((char *)pSize) + size);
	pSize = pSize + 4;
	*pSize = size;
}

/*
 *	Funcation Name: get_blockType
 *	Input type:		void*
 * 	Output type:	int
 * 	Description:	Extracts the Block Type
 */
int get_blockType(void *ptr)
{
	int *pType;

	//	Points to the address where the Type of the block is stored
	pType = (int *)ptr;
	pType = pType - 2;

	return *pType;
}

/*
 *	Funcation Name: set_blockType
 *	Input type:		void* and int
 * 	Output type:	void
 * 	Description:	Sets the Block Type
 */
void set_blockType(void *ptr, int type)
{
	int *pType;
	int blockSize = get_blockSize(ptr);
	pType = (int *)ptr;
	pType = pType - 2;
	*pType = type;
	pType = pType + 2;
	pType = (int *)(((char *)pType) + blockSize);
	pType = pType + 5;
	*pType = type;
}

/*
 *	Funcation Name: get_blockN
 *	Input type:		void*
 * 	Output type:	unsigned long
 * 	Description:	Extracts N
 */
unsigned long get_blockN(void *ptr)
{
	char *pN;
	int blockSize = get_blockSize(ptr);

	//	Points to the address where the N of the block is stored
	pN = (char *)ptr;
	pN = pN + blockSize;

	return *(unsigned long *)pN;
}

/*
 *	Funcation Name: set_blockN
 *	Input type:		void* and unsigned long
 * 	Output type:	void
 * 	Description:	Sets the Block N
 */
void set_blockN(void *ptr, unsigned long address)
{
	char *pN;
	int blockSize = get_blockSize(ptr);
	pN = (char *)ptr;
	pN = pN + blockSize;
	*(unsigned long *)pN = address;
}

/*
 *	Funcation Name: get_blockP
 *	Input type:		void*
 * 	Output type:	unsigned long
 * 	Description:	Extracts P
 */
unsigned long get_blockP(void *ptr)
{
	char *pP;
	int blockSize = get_blockSize(ptr);

	//	Points to the address where the P of the block is stored
	pP = (char *)ptr;
	pP = pP + blockSize + 8;

	return *(unsigned long *)pP;
}

/*
 *	Funcation Name: set_blockP
 *	Input type:		void* and unsigned long
 * 	Output type:	void
 * 	Description:	Sets the Block P
 */
void set_blockP(void *ptr, unsigned long address)
{
	char *pP;
	int blockSize = get_blockSize(ptr);
	pP = (char *)ptr;
	pP = pP + blockSize + 8;
	*(unsigned long *)pP = address;
}