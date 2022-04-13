#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dma.h" // include the library interface

void test_bit_man();
void test_word_to_binary();
int main(int argc, char **argv)
{

    // test_word_to_binary();
    test_bit_man();
    printf("something\n");


    void *p1;
    void *p2;
    void *p3;
    void *p4;
    int ret;

    ret = dma_init(13); 
    if (ret != 0)
    {
        printf("something was wrong\n");
        exit(20);
    }
    //printf("before calling dma_print_bitmap\n");

    dma_print_bitmap();
    exit(0);

    
    p1 = dma_alloc(100); // allocate space for 100 bytes
    p2 = dma_alloc(1024);
    p3 = dma_alloc(64); // always check the return value
    p4 = dma_alloc(220);
    dma_free(p3);
    p3 = dma_alloc(2048);
    dma_print_blocks();
    dma_free(p1);
    dma_free(p2);
    dma_free(p3);
    dma_free(p4);
}

void test_bit_man() {

    //int num = 16;
    char arr[64];
   
    int is_first = 0;
    int size = 8;
    int start = 0;
    
    word_to_binary(word_manipulator(is_first,size,start), arr);
    // there seems to be a weird behaviour to print
    // 67 chars although the 
    printf("is_first: %d, size: %d, start: %d, output: %64.s \n", is_first,size,start, arr);
    is_first = 1; size = 6; start = 0;     
    word_to_binary(word_manipulator(is_first,size,start), arr);
    printf("is_first: %d, size: %d, start: %d, output: %64.s \n", is_first,size,start, arr);

    is_first = 1; size = 8; start = 4;     
    word_to_binary(word_manipulator(is_first,size,start), arr);
    printf("is_first: %d, size: %d, start: %d, output: %64.s \n", is_first,size,start, arr);
    // printf("test bit man XILAS?");
}

void test_word_to_binary() {
    int num = 16;
    char arr[8];

    word_to_binary(255 << 3, arr);
    printf("FF << (3): %ld : %s\n",255l << 3, arr);

    
    word_to_binary(255 << 5, arr);
    printf("FF << (5): %ld : %s\n",255l << 5, arr);

    word_to_binary(0 << 5, arr);
    printf("0 << (5): %ld : %s\n", 0l << 5, arr);

    word_to_binary(1 << 5, arr);
    printf("1 << (5): %ld : %s\n", 1l << 5, arr);

    
    word_to_binary(1 >> 3, arr);
    printf("1 >> (3): %ld : %s\n",1l >> 3, arr);

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
    num = 128+15;
    word_to_binary(num, arr);
    printf("num: %d, bin: %s\n", num, arr);
    num = 13;
    word_to_binary(num, arr);
    printf("num: %d, bin: %s\n", num, arr);

}