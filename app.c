#include "dma.h"// include the library interface
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

void test_bit_man();
void test_word_to_binary();
void testFirstFit();
struct timeval timevalSubtract(struct timeval *s, struct timeval *e);
void testFree();
void testExtFrag()
{
    void *p1 = NULL;
    int ret;

    ret = dma_init(11);
    if (ret != 0) {
        printf("something was wrong\n");
        //exit(20);
    }
    void *allocs[28] = {NULL};
    //allocate 10 times
    for (int i = 0; i < 28; ++i) {
        allocs[i] = dma_alloc(64);
        p1 = allocs[i];
    }
    printf("\n");

    dma_print_bitmap();
    for (int i = 0; i < 28; i += 2) {
        dma_free(allocs[i]);
    }
    dma_print_bitmap();
    p1 = dma_alloc(128);
    if (p1 == NULL) {
        printf("p1 NULL\n");
    } else {
        printf("p1 allocs\n");
    }
    printf("\n");
    dma_print_bitmap();
}
void testIntFrag()
{
    void *p1 = NULL;
    int ret;
    const int size = (1 << 11) - (1 << 5) - 256;
    ret = dma_init(11);
    if (ret != 0) {
        printf("something was wrong\n");
        //exit(20);
    }

    //allocate 10 times
    int rem = size;
    int realRem = size;
    printf("%6s, %6s, %6s, %6s\n", "alloc", "frag", "rem", "real");
    //printf("%s, %s, %s, %s, %s\n", "alloc", "frag", "rem", "real", "usec");
    int i = 0;
    struct timeval rt[3];

    while (1) {
        gettimeofday(&rt[0], NULL);
        p1 = dma_alloc(1);
        gettimeofday(&rt[1], NULL);
        rt[2] = timevalSubtract(&rt[1], &rt[0]);
        i++;
        if (p1 == NULL) {
            break;
        }
        rem -= 1;
        realRem -= 16;
        printf("%6d, %6d, %6d, %6d\n", i, dma_give_intfrag(), rem, realRem);
        //printf("%d, %d, %d, %d, %ld\n", i, dma_give_intfrag(), rem, realRem, rt[2].tv_sec * 1000000 + rt[2].tv_usec);
    }
    printf("\n");
    dma_print_bitmap();
    p1 = dma_alloc(2);
    if (p1 == NULL) {
        printf("p1 NULL\n");
    } else {
        printf("p1 allocs\n");
    }
    printf("\n");
    dma_print_bitmap();
}
int main(int argc, char **argv)
{
    // test_word_to_binary();
    // test_bit_man();
    printf("App started\n");

    /*testExtFrag();
    printf("\n");
    testIntFrag();
    printf("\n");
    testFirstFit();
    printf("\n");
     */
    testFree();
    exit(0);
}
void testFree()
{
    void *p1 = NULL;
    int ret;
    void *allocs[64496];
    printf("%s, %s\n", "freeSize", "usec");
    for (int asize = 16; asize <= 1024; asize *= 2) {
        ret = dma_init(20);
        if (ret != 0) {
            printf("something was wrong, the libdma couldn't be initialized\n");
            //exit(20);
        }
        int count = 0;
        while (1) {
            p1 = dma_alloc(asize);
            if (p1 == NULL) {
                break;
            }
            allocs[count] = p1;
            count++;
        }
        //printf("%d\n", count);
        //printf("\n");

        //dma_print_bitmap();
        //printf("%s, %s, %s, %s, %s\n", "alloc", "frag", "rem", "real", "usec");
        struct timeval rt[3];
        gettimeofday(&rt[0], NULL);
        for (int j = 0; j < count; j++) {
            dma_free(allocs[j]);
        }
        gettimeofday(&rt[1], NULL);
        rt[2] = timevalSubtract(&rt[1], &rt[0]);
        printf("%d, %ld\n", asize, rt[2].tv_sec * 1000000 + rt[2].tv_usec);
    }
}
struct timeval timevalSubtract(struct timeval *s, struct timeval *e)
{
    if (s->tv_usec < e->tv_usec) {
        long nsec = (e->tv_usec - s->tv_usec) / 1000000 + 1;
        e->tv_usec -= 1000000 * nsec;
        e->tv_sec += nsec;
    }
    if (s->tv_usec - e->tv_usec > 1000000) {
        long nsec = (s->tv_usec - e->tv_usec) / 1000000;
        e->tv_usec += 1000000 * nsec;
        e->tv_sec -= nsec;
    }
    struct timeval result;
    result.tv_sec = s->tv_sec - e->tv_sec;
    result.tv_usec = s->tv_usec - e->tv_usec;
    return result;
}
void testFirstFit()
{
    int ret;
    int size = (1 << 11) - (1 << 5) - 256;
    ret = dma_init(24);
    if (ret != 0) {
        printf("something was wrong\n");
        //exit(20);
    }
    size = 16;
    void *p1;
    struct timeval rt[5][3];
    //printf("%10s %10s\n", "size", "usec");
    printf("%s, %s\n", "size", "usec");
    for (int i = 0; i < 5; ++i) {
        ret = dma_init(20);
        gettimeofday(&rt[i][0], NULL);
        while (1) {
            p1 = dma_alloc(size);
            if (p1 == NULL) {
                break;
            }
        }
        gettimeofday(&rt[i][1], NULL);
        rt[i][2] = timevalSubtract(&rt[i][1], &rt[i][0]);
        printf("%d, %ld\n", size, rt[i][2].tv_sec * 1000000 + rt[i][2].tv_usec);
        size *= 8;
    }
}

void test_bit_man()
{
    // int num = 16;
    char arr[65];
    arr[64] = '\0';

    int is_first = 0;
    int size = 8;
    int start = 0;

    word_to_binary(word_manipulator(is_first, start, size), arr);
    // there seems to be a weird behaviour to print
    // 67 chars although the
    printf("is_first: %d, start: %d, size: %d, output: %s \n\n", is_first, start, size, arr);
    is_first = 1;
    size = 6;
    start = 0;
    word_to_binary(word_manipulator(is_first, start, size), arr);
    printf("is_first: %d, start: %d, size: %d, output: %s \n\n", is_first, start, size, arr);

    is_first = 1;
    size = 8;
    start = 4;
    word_to_binary(word_manipulator(is_first, start, size), arr);
    printf("is_first: %d, start: %d, size: %d, output: %s \n\n", is_first, start, size, arr);
    // printf("test bit man XILAS?");

    is_first = 1;
    size = 60;
    start = 2;
    word_to_binary(word_manipulator(is_first, start, size), arr);
    printf("is_first: %d, start: %d, size: %d, output: %s \n\n", is_first, start, size, arr);
    // printf("test bit man XILAS?");

    is_first = 1;
    size = 28;
    start = 32;
    word_to_binary(word_manipulator(is_first, start, size), arr);
    printf("is_first: %d, start: %d, size: %d, output: %s \n\n", is_first, start, size, arr);
    // printf("test bit man XILAS?");
}

void test_word_to_binary()
{
    int num = 16;
    char arr[8];
    int DMA_BIT_PER_BYTE = 8;
    unsigned long int FFL = 255UL;
    unsigned long int ALL_ONE = FFL << 7 * DMA_BIT_PER_BYTE | FFL << 6 * DMA_BIT_PER_BYTE |
                                FFL << 5 * DMA_BIT_PER_BYTE | FFL << 4 * DMA_BIT_PER_BYTE |
                                FFL << 3 * DMA_BIT_PER_BYTE | FFL << 2 * DMA_BIT_PER_BYTE |
                                FFL << 1 * DMA_BIT_PER_BYTE | FFL;

    word_to_binary(ALL_ONE << 32 << 32, arr);
    printf("ALL_ONE << (64): %lu : %s\n", ALL_ONE << 32 << 32, arr);

    word_to_binary(ALL_ONE >> 32 >> 32, arr);
    printf("ALL_ONE >> (64): %lu : %s\n", ALL_ONE >> 32 >> 32, arr);

    word_to_binary(255 << 3, arr);
    printf("FF << (3): %ld : %s\n", 255l << 3, arr);

    word_to_binary(255 << 5, arr);
    printf("FF << (5): %ld : %s\n", 255l << 5, arr);

    word_to_binary(0 << 5, arr);
    printf("0 << (5): %ld : %s\n", 0l << 5, arr);

    word_to_binary(1 << 5, arr);
    printf("1 << (5): %ld : %s\n", 1l << 5, arr);

    word_to_binary(1 >> 3, arr);
    printf("1 >> (3): %ld : %s\n", 1l >> 3, arr);

    word_to_binary(0 >> 3, arr);
    printf("0 >> (3): %ld : %s\n", 0l >> 3, arr);

    word_to_binary(255 >> 3, arr);
    printf("255 >> (3): %ld : %s\n", 255l >> 3, arr);

    word_to_binary(255 >> 5, arr);
    printf("255 >> (5): %ld : %s\n", 255l >> 5, arr);

    word_to_binary(num, arr);
    printf("num: %d, bin: %s\n", num, arr);
    num = 25;
    word_to_binary(num, arr);
    printf("num: %d, bin: %s\n", num, arr);
    num = 255;
    word_to_binary(num, arr);
    printf("num: %d, bin: %s\n", num, arr);
    num = 128 + 15;
    word_to_binary(num, arr);
    printf("num: %d, bin: %s\n", num, arr);
    num = 13;
    word_to_binary(num, arr);
    printf("num: %d, bin: %s\n", num, arr);
}