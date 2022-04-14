#include "dma.h" // include the library interface
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

void test_bit_man();
void test_word_to_binary();
int main(int argc, char **argv)
{

    //test_word_to_binary();
    test_bit_man();
    printf("something\n");


    void *p1;
    void *p2;
    void *p3;
    void *p4;
    void *ret;

    ret = dma_init(17);
    if (ret != 0)
    {
        printf("something was wrong\n");
        //exit(20);
    }
    // printf("before calling dma_print_bitmap\n");
    dma_print_bitmap();

    dma_free(ret);
    //printf("before calling dma_print_bitmap\n");

    dma_print_bitmap();

    p1 = dma_alloc(100000000); // allocate space for 100 bytes: 14 WORDS
    printf("\n\np1: %p, p2: %p, p3: %p, p4: %p\n\n", p1, p2, p3, p4);
    // dma_print_bitmap();

    p2 = dma_alloc(1024); // 128 WORDS
    printf("\n\np1: %p, p2: %p, p3: %p, p4: %p\n\n", p1, p2, p3, p4);
    // dma_print_bitmap();

    p3 = dma_alloc(64); // 8 WORDS
    printf("\n\np1: %p, p2: %p, p3: %p, p4: %p\n\n", p1, p2, p3, p4);
    // dma_print_bitmap();

    p4 = dma_alloc(220); // 28 WORDS
    printf("\n\np1: %p, p2: %p, p3: %p, p4: %p\n\n", p1, p2, p3, p4);
    dma_print_bitmap();

    exit(0);

    dma_free(p3);
    p3 = dma_alloc(2048);
    dma_print_blocks();
    dma_free(p1);
    dma_free(p2);
    dma_free(p3);
    dma_free(p4);
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