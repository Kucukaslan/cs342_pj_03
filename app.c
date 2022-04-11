#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dma.h" // include the library interface
int main(int argc, char **argv)
{
    void *p1;
    void *p2;
    void *p3;
    void *p4;
    int ret;

    ret = dma_init(20); 
    if (ret != 0)
    {
        printf("something was wrong\n");
        exit(13);
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