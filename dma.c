#include <stdio.h>



/**
 * @file dma.c
 * @author Muhammed Can Küçükaslan
 * @brief 
 * @version 0.1
 * @date 2022-04-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/**
 * @brief This function will initialize the library.
 * As part of the initialization, the function will allocate 
 * a contiguous segment of free memory from the virtual memory
 * of the process using mmap() system call. 
 * 
 * This segment will be your heap. Your library will manage it. 
 * The size  of the segment will be 2^m bytes. 
 * 
 * For example, if m is equal to 16, then the  segment size will be 64 KB (2^16). 
 * The value of m can be between 14 and 22 (inclusive).
 * Hence, the smallest segment can be 16 KB long and the largest 
 * segment can be 4 MB long. The function will do other initializations, 
 * like  initializing the bitmap. 
 * 
 * You can assume that dma_init() will be called  by the main thread 
 * in a multi-threaded program, before any other thread is created. 
 * 
 * @param m The size of the segment will be 2^m bytes. 
 * @return int If initialization is successful, the function will return 0. 
Otherwise, it will return -1.
 */
int dma_init (int m){

    return -1;
}


/**
 * @brief This function will allocate memory for the calling application
 *  from the segment. 
 * The size parameter indicates the size (in bytes) of the requested memory
 * to be allocated. The value of size can be any positive integer value 
 * (internally, however, we will allocate space that is a multiple of 16 bytes). 
 *  
 * @param size The size parameter indicates the size (in bytes) of the requested memory
 * to be allocated. The value of size can be any positive integer value
 * 
 * @return void* 
 * A pointer to the beginning of the allocated space will be returned, 
 * if memory is successfully allocated. 
 * Otherwise, if the requested memory can not be allocated, 
 * for example due to lack of a large enough free block, NULL will be returned. 
 * 
 * If indicated size is too big, the library will not be able to satisfy 
 * the request and will return NULL in this case as well. 
 * 
 * By checking the return value, we can understand 
 * if a request could be satisfied or not. 
 */
void *dma_alloc (int size){

    return NULL;
}


/**
 * @brief This function will free the allocated memory block pointed by p. 
 * From the pointer value (which is an unsigned long int) 
 * and the start virtual address of the segment, 
 * you can drive the start bit position of the block representation in the bitmap.
 * From the bitmap encoding for the block,
 * you can also drive the size of the block. 
 * The corresponding bits in the bitmap need to be set to 1. 
 * We assume a pointer (such as void *) is 8 bytes long. 
 * Similarly, a long int is also 8 bytes long. 
 * Therefore, we can consider pointer values as unsigned long integers.
 * 
 * @param p pointer to the allocated memory block to be freed. 
 */
void dma_free (void *p) {
    exit(47);
}

/**
 * @brief . This function will print a page of the segment 
 * to the screen using hexadecimal digits. 
 * 
 * The first page of the segment 
 * (starting at the address returned from mmap() call) will be considered as page 0.
 * 
 * Hence, pno parameter can have a value in the interval [0, SS/PS), 
 * where SS is segment size and PS is the pagesize. 
 * 
 * Each output line will contain 64 hexadecimal characters. 
 * An example output can be as follows (use this format). 
 * ffff8000ffffffff800000000000ffffffff55d0ffff
 * 
 * @param pno Page number of the page to be printed.
 * Hence, pno parameter can have a value in the interval [0, SS/PS), 
 * where SS is segment size and PS is the pagesize. 
 */
void dma_print_page(int pno){
    exit(47);
}

/**
 * @brief This function will print the bitmap to the
 *  screen as a sequence of 1’s and 0’s. 
 * Each line will have 64 bits. We will put a space character 
 * after each 8 bit sequence in the line, like below (use this format).
 * 
 * 01000000 … 00000000 00000000 00000000 00000000
 * 00000000 … 00000000 00000000 00000000 00000000 
 * …
 * 11111111 … 01000000 11111111 01000000 00000000
 */
void dma_print_bitmap()
{
    exit(47);
}

/**
 * @brief This function will print information 
 * about the allocated and free blocks. 
 * 
 * For each block it will print the start address using hexadecimal digits,
 *  and the size (in bytes) using both hexadecimal digits 
 * and decimal digits (in parentheses), in a separate line.
 * It will also print at the beginning of the line
 *  if the block is allocated (A) or is free (F). 
 * The printing will be in the order of start addresses. 
 * You will also print information about the used blocks 
 * containing the bitmap and reserved space. 
 * 
 * An example output can be as follows (use this format):
 * 
 * A, 0x00007ffff75e4000, 0x400 (1024)
 * A, 0x00007ffff75e4400, 0x100 (256)
 * A, 0x00007ffff75e4500, 0x200 (512)
 * A, 0x00007ffff75e4700, 0x100 (256)
 * F, 0x00007ffff75e4800, 0x300 (768)
 * A, 0x00007ffff75e4b00, 0x20 (32)
 * F, 0x00007ffff75e4b20, 0x50 (80)
 * A, 0x00007ffff75e4b70, 0x230 (560)
 * 
 * Note that the printed addresses must be 16 hexadecimal digits long (64bits,
 *  even though only 48 bits are used). 
 * They are real virtual addresses, not offsets into the segment.
 * We are assuming that the machine architecture is x86-64. 
 * 
 * If the architecture is different, still the output should use 16
 *  hexadecimal digits for the addresses. 
 */
void dma_print_blocks()
{
    exit(47);
}

/**
 * @brief This function will return the internal fragmentation amount.
 *  Hence, in your library you need to maintain 
 * the total internal fragmentation amount
 *  while you are allocating memory to requests.
 * 
 * @return int amount of the accumulated internal fragmentation, 
 * while we are allocating.
 */
int dma_give_intfrag() 
{
    exit(47);
}