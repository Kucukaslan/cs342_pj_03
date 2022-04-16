/**
 * @file dma.c
 * @author Muhammed Can Küçükaslan & Giray Akyol
 * @brief
 * @version 0.1
 * @date 2022-04-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <limits.h>
#include <pthread.h>
#include <stdio.h>// to include stdin, stdout and some constants e.g. NULL
#include <stdlib.h>
#include <sys/mman.h>// mmap etc.
#include <unistd.h>


#include "dma.h"

#define DMA_PAGE_SIZE_BYTE 4096 // 4 KiBhttps://en.wikipedia.org/wiki/Page_(computer_memory)#Multiple_page_sizes
#define DMA_WORD_LENGTH_BYTE 8
#define DMA_BIT_PER_BYTE 8
const int DMA_WORD_LENGTH_BIT = DMA_WORD_LENGTH_BYTE * DMA_BIT_PER_BYTE;
#define DMA_RESERVED_SPACE_BYTE 256
#define FF 255
#define FFL 255l
#define ZERO 0

//#define LEFT_SHIFT = 1 << 6;
unsigned long *themap;
int size_in_bytes;
int bitmap_in_bits;
int reserved_area_size_in_bits = 256 * DMA_BIT_PER_BYTE;
int internal_fragmentation_amount = 0;
int DMA_TOTAL_WORD_COUNT;
int DMA_TOTAL_BITMAP_WORD_COUNT;
pthread_mutex_t themap_mutex;// = PTHREAD_MUTEX_INITIALIZER;



ulong tmIndexOf(ulong i)
{
    return i / 64;
}
ulong ulIndexOf(ulong i)
{
    return i % 64;
}

ulong ul2bits(ulong w, ulong idx)
{
    //left to right
    ulong s = w >> (62 - idx);
    return s & 0b11;
}

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
 * @param m The size of the segment will be 2^m bytes. The value of m can be between 14 and 22
 * @return int If initialization is successful, the function will return 0.
Otherwise, it will return -1.
 */
int dma_init(int m)
{
    if(m <= 0) {
        printf("Segment size must have been positive but is %d, terminating.\n", m);
        exit(-1);
    }
    pthread_mutex_init(&themap_mutex, NULL);
    // if()
    void *p;

    // The size  of the segment will be 2^m bytes.
    // "2^m" this is the time which I waited for a LONG LONG time
    // Now finally I had a chance to use some of the things I learned in CS 223/4. Finally!
    // Refer to A.7.8 Shift Operators section of the "the book of Ritchie et. al."
    /*
     * The value of E1<<E2 is E1 (interpreted as a bit pattern) left-shifted E2 bits;
     * in the absence of overflow, this is equivalent to multiplication by 2^E2.
     * The value of E1>>E2 is E1 right-shifted E2 bit positions.
     * The right shift is equivalent to division by 2^E2 if E1 is unsigned or it has a nonnegative value;
     * otherwise the result is implementation-defined.
     */
    size_in_bytes = 1;
    // printf("1. m: %d, size: %d\n", m, size_in_bytes);

    size_in_bytes = size_in_bytes << m;// 1*2^m
    DMA_TOTAL_WORD_COUNT = size_in_bytes / DMA_WORD_LENGTH_BYTE;
    // one bit for each WORD
    bitmap_in_bits = DMA_TOTAL_WORD_COUNT;
    DMA_TOTAL_BITMAP_WORD_COUNT = bitmap_in_bits / DMA_WORD_LENGTH_BIT;

    // printf("2. m: %d, size: %d, bitmap:%d\n", m, size_in_bytes, bitmap_in_bits);

    p = mmap(NULL, (size_t) size_in_bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // print start address
    //    printf("Address of the allocated map %lx\n", (long)p);

    if (p == MAP_FAILED) {
        printf("failed to allocate map %lx\n", (long) p);

        return -1;
    }
    // printf("not failed to allocate map %lx\n", (long)p);

    if (pthread_mutex_lock(&themap_mutex) != 0) {
        printf("couldn't get  themap_mutex!\n");
        exit(-1);
    }
    //  printf("got the lock\n");
    themap = p;
    for (int i = 0; DMA_BIT_PER_BYTE * i < size_in_bytes; i++) {
        // printf("add %p : val %lu\n",themap +i , themap[i] );

        ((unsigned long *) themap)[i] = ULONG_MAX;
        // printf("add %p : val %lu \n", themap + i, ((unsigned long *)themap)[i]);
    }

    // allocate first bitmap_in_bits + reserved_area_size_in_bits
    int bitmap_word_count = bitmap_in_bits / (DMA_WORD_LENGTH_BIT);

    // since m>=14 it is guaranteed that 2^8|bitmap_in_bits. (in fact bitmap_word_count will be multiple of 4)
    // search word by word to find appropriate place
    unsigned long int new_value = 0;
    // set first to digit to 01
    int i = 0;
    int word_index = 0;
    // printf("before the while\n");
    int size_to_be_allocated = bitmap_in_bits;
    while (i < bitmap_word_count) {
        // printf("in the while, word_index: %d , bitmap_word_count: %d \n", word_index, bitmap_word_count);

        size_to_be_allocated = bitmap_word_count - word_index * (DMA_WORD_LENGTH_BIT);
        if (size_to_be_allocated > (DMA_WORD_LENGTH_BIT)) {
            size_to_be_allocated = (DMA_WORD_LENGTH_BIT);
        }

        if (word_index == 0) {
            new_value = word_manipulator(1, 0, size_to_be_allocated);
            themap[word_index] = new_value & themap[word_index];
            // printf("new_value: %lu,size_to_be_allocated: %d, word_index: %d , bitmap_word_count: %d \n", new_value,
            // size_to_be_allocated, word_index , bitmap_word_count);
        } else {
            // it is guaranteed that it starts from beginning
            new_value = word_manipulator(0, 0, size_to_be_allocated);
            themap[word_index] = new_value & themap[word_index];
            // printf("new_value: %lu,size_to_be_allocated: %d, word_index: %d , bitmap_word_count: %d \n", new_value,
            // size_to_be_allocated, word_index , bitmap_word_count);
        }
        // printf("new_value: %lu,size_to_be_allocated: %d, word_index: %d , bitmap_word_count: %d \n", new_value,
        //        size_to_be_allocated, word_index, bitmap_word_count);

        i = i + size_to_be_allocated;
        word_index = word_index + 1;
    }

    // allocate the place for the "reserved space" of 256 bytes
    int reserved_word_count = DMA_RESERVED_SPACE_BYTE / DMA_WORD_LENGTH_BYTE;
    int start_index_in_word = bitmap_word_count % DMA_WORD_LENGTH_BIT;
    int is_first = 1;
    word_index = bitmap_word_count / (DMA_WORD_LENGTH_BIT);
    size_to_be_allocated = reserved_word_count;
    i = 0;
    while (i < reserved_word_count) {
        // printf("in the reserved while, word_index: %d , reserved_word_count: %d \n", word_index,
        // reserved_word_count);

        size_to_be_allocated = reserved_word_count - i;
        if (size_to_be_allocated > DMA_WORD_LENGTH_BIT) {
            size_to_be_allocated = DMA_WORD_LENGTH_BIT;
        }

        if (is_first) {
            new_value = word_manipulator(1, start_index_in_word, size_to_be_allocated);
            themap[word_index] = new_value & themap[word_index];
            is_first = 0;
            // printf("new_value: %lu,size_to_be_allocated: %d, word_index: %d , reserved_word_count: %d \n", new_value,
            // size_to_be_allocated, word_index , reserved_word_count);
        } else {
            // it is guaranteed that it starts from beginning
            new_value = word_manipulator(0, start_index_in_word, size_to_be_allocated);
            themap[word_index] = new_value & themap[word_index];
            // printf("new_value: %lu,size_to_be_allocated: %d, word_index: %d , reserved_word_count: %d \n", new_value,
            // size_to_be_allocated, word_index , reserved_word_count);
        }
        // printf("new_value: %lu,size_to_be_allocated: %d, word_index: %d , reserved_word_count: %d \n", new_value,
        //        size_to_be_allocated, word_index, reserved_word_count);

        i = i + size_to_be_allocated;
        word_index = word_index + 1;
    }

    pthread_mutex_unlock(&themap_mutex);

    // printf("unlocked the lock\n");

    return 0;
}

/**
 * @brief This function will allocate memory for the calling application
 *  from the segment.
 * The size parameter indicates the size (in bytes) of the requested memory
 * to be allocated. The value of size can be any positive integer value
 * (internally, however, we will allocate space that is a multiple of 16 bytes).
 *
 * @param size The size parameter indicates the size (in BYTEs) of the requested memory
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
void *dma_alloc(int size)
{
    pthread_mutex_lock(&themap_mutex);
    void *pointer = NULL;
    int word_count_to_allocate = size / (2 * DMA_WORD_LENGTH_BYTE); // (half of the floor of word)
    int extra_internal_fragmentation =  size % (2 * DMA_WORD_LENGTH_BYTE);
    if (extra_internal_fragmentation > 0)// handmade ceil function
    {
        word_count_to_allocate = 1 + word_count_to_allocate;
        internal_fragmentation_amount = internal_fragmentation_amount
                                     + (2 * DMA_WORD_LENGTH_BYTE - extra_internal_fragmentation);
        //printf("Internal: %d, fragm: %d \n", internal_fragmentation_amount, extra_internal_fragmentation);
    }
    //
    word_count_to_allocate = 2 * word_count_to_allocate;

    // search wor sufficiently large places
    // current location of search is identified by word_index, bit_index pair.
    // start of current "empty place" is hold in current_pointer
    // which is calculated as themap + word_index*
    int word_index = 0;
    int bit_index = 0;
    void *current_pointer = themap + word_index * DMA_WORD_LENGTH_BIT + bit_index;
    int start_word_index = 0;
    int start_bit_index = 0;
    int has_found = 0;
    int has_failed = 0;
    int size_so_far = 0;
    int bit_alignment_to_shift = 2;
    int current_2_bits;
    //printf("Want to allocate %d words\n", word_count_to_allocate);
    // i.e. search for next first sufficiently large empty place
    while (!has_found && !has_failed) {
        unsigned long int current_word = themap[word_index];
        current_2_bits = 3 & (current_word >> (DMA_WORD_LENGTH_BIT - bit_alignment_to_shift));
        // find next first empty place
        while (current_2_bits != 3 && !has_failed) {
            bit_index = 2 + bit_index;

            // if couldn't find in this word go to the next
            if (bit_index > DMA_WORD_LENGTH_BIT - bit_alignment_to_shift) {
                word_index = 1 + word_index;
                bit_index = 0;
                // check if word_index is still valid if not search is failed otherwise update current word
                if (DMA_TOTAL_BITMAP_WORD_COUNT <= word_index) {
                    has_failed = 1;
                    // break;
                } else {
                    current_word = themap[word_index];
                }
            }
            current_2_bits = 3 & (current_word >> (DMA_WORD_LENGTH_BIT - bit_alignment_to_shift - bit_index));
        }
        // printf("FIRST EMPTY CRUMB: %s, word_index: %d, bit_index: %d\n", has_failed ? "Search Failed" : "Found at",
        //        word_index, bit_index);
        if (!has_failed) {
            current_pointer = themap + word_index * DMA_WORD_LENGTH_BIT + bit_index;
            start_word_index = word_index;
            start_bit_index = bit_index;
            // has_found = 1; // todo acşually had NOT found
            size_so_far = 2;

            // so far we have the next first empty place but we are not sure if there
            // is enough space to allocate
            //
            int is_end_of_this_free_part = 0;
            while (!has_failed && !has_found && !is_end_of_this_free_part) {
                // printf("Search sufficiently large place: %s word_index: %d, bit_index: %d\n",
                //        has_failed ? "Search Failed" : "Travesing at", word_index, bit_index);
                bit_index = 2 + bit_index;
                // if this word ended, go to the next word
                if (bit_index > DMA_WORD_LENGTH_BIT - bit_alignment_to_shift) {
                    word_index = 1 + word_index;
                    bit_index = 0;
                    // check if word_index is still valid if not search is failed otherwise update current word
                    if (DMA_TOTAL_BITMAP_WORD_COUNT <= word_index) {
                        // printf("DMA_TOTAL_BITMAP_WORD_COUNT %d,word_index : %d ",DMA_TOTAL_BITMAP_WORD_COUNT, word_index);
                        has_failed = 1;
                        // break;
                    } else {
                        current_word = themap[word_index];
                    }
                }
                current_2_bits = 3 & (current_word >> (DMA_WORD_LENGTH_BIT - bit_alignment_to_shift - bit_index));
                if (current_2_bits == 3) {
                    size_so_far = 2 + size_so_far;
                }
                else {
                    is_end_of_this_free_part = 1;
                }
                if (size_so_far >= word_count_to_allocate) {
                    // printf("Found sufficiently large place: %s, word_index: %d, bit_index: %d\n",
                    //        has_failed !=0 ? "Search Failed" : "Found at", word_index, bit_index);
                    has_found = 1;
                }
                else{
                    // printf("Still searching for sufficiently large place: %s, word_index: %d, bit_index: %d\n",
                    //        has_failed !=0 ? "Search Failed" : "Found at", word_index, bit_index);

                };
            }
            // printf("Left the inner while: %s, word_index: %d, bit_index: %d\n",
            //        has_failed ? "Search Failed" : "Found at", word_index, bit_index);
        }
    }

    // printf("Left the great while: %s, word_index: %d, bit_index: %d\n", has_failed ? "Search Failed" : "Found at",
    //       word_index, bit_index);
    if (has_found) {
        // printf("Found sufficiently large place, now will mark it: %s, word_index: %d, bit_index: %d\n",
        //        has_failed ? "Search Failed" : "Found at", start_word_index, start_bit_index);
        pointer = current_pointer;
        // mark corresponding places in bitmap as allocated
        int remained_to_mark = word_count_to_allocate;
        int alloc_word_index = start_word_index;
        int alloc_bit_index = start_bit_index;
        unsigned long int new_value;
        int is_first = 1;
        while (remained_to_mark > 0) {
            // printf("Started marking it: %s, word_index: %d, bit_index: %d, remained: %d\n",
            //        has_failed ? "Search Failed" : "Found at", alloc_word_index, alloc_bit_index, remained_to_mark);
            // should mark until the end of the current word is
            if (remained_to_mark > DMA_WORD_LENGTH_BIT - alloc_bit_index) {
                new_value = word_manipulator(is_first, alloc_bit_index, DMA_WORD_LENGTH_BIT - alloc_bit_index);
                remained_to_mark = remained_to_mark - (DMA_WORD_LENGTH_BIT - alloc_bit_index);
                alloc_bit_index = 0;

                // char arr[65];
                // arr[65] = '\0';
                // word_to_binary(new_value, arr);
                // printf("new_value: %s\n", arr);
                themap[alloc_word_index] = themap[alloc_word_index] & new_value;
                alloc_word_index = 1 + alloc_word_index;
            } else {
                new_value = word_manipulator(is_first, alloc_bit_index, remained_to_mark);
                remained_to_mark = remained_to_mark - (remained_to_mark);
                alloc_bit_index = 0;
                // char arr[65];
                // arr[65] = '\0';
                // word_to_binary(new_value, arr);
                // printf("new_value: %s\n", arr);
                themap[alloc_word_index] = themap[alloc_word_index] & new_value;
            }

            is_first = 0;
        }
    }
    pthread_mutex_unlock(&themap_mutex);
    return pointer;
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
void dma_free(void *p)
{
    // although only address of the themap is used, in the condition of the else if,
    // it is better to be safe than sorry.
    // Nevertheless, it is TAs's computer's cpu and Bilkent's electricity
    // why bother that much (on) efficiency?
    // Just joking, writing code at 4 AM has a bad influence on my humour.
    pthread_mutex_lock(&themap_mutex);
    if(p == NULL) {
        pthread_mutex_unlock(&themap_mutex);
        return;
    }
    else if( p > ((void*) themap) + DMA_TOTAL_WORD_COUNT) {
        pthread_mutex_unlock(&themap_mutex);
        return;
    }

    ulong relptr = ((ulong) p) - (ulong) themap;
    ulong word_count = relptr / DMA_WORD_LENGTH_BYTE;
    ulong uli = word_count % 64;
    ulong tmi = word_count / 64;

    ulong ulword = themap[tmi];
    ulong newWord = ulword;

    ulong TorMask = ((ulong) 0b11) << 62;
    ulong orMask = TorMask >> uli;
    newWord = newWord | orMask;
    themap[tmi] = newWord;
    uli = uli+2;

    // points to a single bit in bitmap, at most 2**21, ulong to avoid casts
    for (; tmi < (bitmap_in_bits / 64); ++tmi) {
        ulword = themap[tmi];
        newWord = ulword;
        TorMask = ((ulong) 0b11) << 62;
        for (; uli < 64; uli += 2) {
            ulong twoBits = ul2bits(ulword, uli);
            ulong orMask = TorMask >> uli;
            if (twoBits == 0b11 || twoBits == 0b01) {
                themap[tmi] = newWord;
                pthread_mutex_unlock(&themap_mutex);
                return;
            } else {
                newWord = newWord | orMask;
                themap[tmi] = newWord;
            }
        }
        uli = 0;
    }
    pthread_mutex_unlock(&themap_mutex);
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
void dma_print_page(int pno)
{
    // verify the range
    int max_pno = size_in_bytes / DMA_PAGE_SIZE_BYTE;
    int tmp = size_in_bytes % DMA_PAGE_SIZE_BYTE;
    if(tmp>0) {
        max_pno++;
    }

    if(pno > 0 && pno < max_pno ) // since first pno := 0 {
    {
        printf("Page number was out of range, it should have been"
                   " in the interval  in the interval [0, %d) but it is %d", max_pno, pno);
        return;
    }
    int word_per_page = DMA_PAGE_SIZE_BYTE / DMA_WORD_LENGTH_BYTE;
    int word_index = pno * word_per_page;
    // now we will start using the map
    pthread_mutex_lock(&themap_mutex);
    for(int iwc /*indexed_word_count*/ = 0; word_index < DMA_TOTAL_WORD_COUNT && iwc < word_per_page; iwc++ ) {
        printf("%.16lx", themap[word_index]);
        if( 3 == iwc % 4) {
            printf("\n"); //" iwc #%d, wi:%d\n", iwc, word_index);
        }
        word_index++;
    }
    pthread_mutex_unlock(&themap_mutex);
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
    // printf("dma_print started: \n\n");

    pthread_mutex_lock(&themap_mutex);

    for (int k = 0; 64 * k < bitmap_in_bits; k++) {
        // printf("\nadd %p : val %ld  : \n",themap + k, themap[k] );
        unsigned long num = ((long *) themap)[k];
        // printf("\nadd %p: val %ld  : \n",themap + k, num );

        for (int j = 63; j >= 0; j--) {
            // printf(" num: %ld : ", num);
            putc((num >> j & 1) == 1 ? '1' : '0', stdout);
            // num>>=1;
            if (j % 8 == 0) {
                putc(' ', stdout);
            }
        }

        putc('\n', stdout);
    }

    putc('\n', stdout);

    // dma_print_bitmap_helper(my, 1);

    pthread_mutex_unlock(&themap_mutex);
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
 *
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
    pthread_mutex_lock(&themap_mutex);
    int word_index = 0;
    int bit_index = 0;
    enum BLOCK { ALLOCATED, FREE};
    enum BLOCK current = ALLOCATED;
    void * block_start = themap;
    unsigned int current_2_bits;
    int current_block_size = 2;

    for(int i=2; i < bitmap_in_bits; i = i+2) {
        // find word_index & bit_index
        word_index = i / DMA_WORD_LENGTH_BIT;
        bit_index = i % DMA_WORD_LENGTH_BIT;

        // get dibit (crumb) starting at i
        current_2_bits = 3 & (themap[word_index] >> (DMA_WORD_LENGTH_BIT - bit_index - 2));
        char arr[65];        arr[65] = '\0';
        word_to_binary(themap[word_index], arr);
        if( current == ALLOCATED && current_2_bits == 0b00){
            // fine
            current_block_size += 2;
        }
        else if( current == FREE && current_2_bits == 0b11) {
            // fine, pattern continues
            current_block_size +=2;
        }
        else {
            // printf("word: %s, current_bits: %d, word_index: %d, bit_index: %d, i: %d,  \n", arr, current_2_bits,word_index,bit_index, i);

            // the pattern is changed, now print the results
            //  * A, 0x00007ffff75e4000, 0x400 (1024)
            current_block_size = DMA_WORD_LENGTH_BYTE * current_block_size;
            printf("%s, 0x%.16lx, 0x%x (%d)\n",current == FREE ? "F": "A",
                   (long unsigned int) block_start, current_block_size, current_block_size);

            // start to new pattern
            block_start = themap + DMA_WORD_LENGTH_BIT * (word_index * DMA_WORD_LENGTH_BIT  + bit_index);
            current = current_2_bits == 3 ? FREE : ALLOCATED;
            current_block_size = 2;
        }
    }
    // print the last block:
    current_block_size = DMA_WORD_LENGTH_BYTE * current_block_size;
    printf("%s, 0x%.16lx, %x (%d)\n",current == FREE ? "F": "A",
           (long unsigned int) block_start, current_block_size, current_block_size);
    pthread_mutex_unlock(&themap_mutex);

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
    // what is the point of the mutex?
    return internal_fragmentation_amount;
}

/**
 * @brief Calculates the new value for a word to be (bitwise) ANDed
 * for allocation.
 *
 * @param is_new_allocation if the allocated place started
 * @param start
 * @param size
 * @return unsigned long int
 */
unsigned long int word_manipulator(int is_new_allocation, int start, int size)
{
    unsigned long int ALL_ONE = FFL << 7 * DMA_BIT_PER_BYTE | FFL << 6 * DMA_BIT_PER_BYTE |
                                FFL << 5 * DMA_BIT_PER_BYTE | FFL << 4 * DMA_BIT_PER_BYTE |
                                FFL << 3 * DMA_BIT_PER_BYTE | FFL << 2 * DMA_BIT_PER_BYTE |
                                FFL << 1 * DMA_BIT_PER_BYTE | FFL;
    unsigned long int result = ALL_ONE;
    unsigned long int ALIGN_SIZE_ZEROS;
    unsigned long int ALIGN_START_POINT;
    // printf("result: %lu\n", result);

    ALIGN_SIZE_ZEROS = ALL_ONE >> (start + size);
    if (start + size >= 64) {
        ALIGN_SIZE_ZEROS = 0UL;
    }

    ALIGN_START_POINT = ALL_ONE << (64 - start);
    if (start <= 0) {
        ALIGN_START_POINT = 0UL;
    }

    if (is_new_allocation) {
        result = (1l << (62 - start)) | ALIGN_SIZE_ZEROS | ALIGN_START_POINT;
        // printf("result: %lu, is_first %d\n", result, is_new_allocation);
    } else {
        result = ALIGN_SIZE_ZEROS | ALIGN_START_POINT;
        // printf("result: %lu, ALL_ONE >> (start + size): %lu,  ALL_ONE << (64 - start): %lu\n", result, ALL_ONE >>
        // (start + size), ALL_ONE << (64 - start));
    }
    char tmp[64];
    word_to_binary(result, tmp);
    // printf("PRETEST: %s\n", tmp);
    return result;
}

void word_to_binary(unsigned long int num, char *binary)
{
    // char binary[8] = "12345678";
    // printf("binary: %s\n", binary);

    char ch = 'a';
    int length = DMA_WORD_LENGTH_BIT;
    for (int j = length - 1; j >= 0; j--) {
        // printf(" num: %d \n", num);
        ch = (num >> j & 1) == 1 ? '1' : '0';
        // printf(" num: %d \n", num);

        // printf("binary: %s, ch: %c\n", binary, ch);

        binary[length - 1 - j] = ch;
        // num>>=1;
        // printf("binary: %s\n", binary);
    }
    // printf("result: %lu, bin: %s\n", num, binary);
}