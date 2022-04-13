#ifndef DMA_H
#define DMA_H
int dma_init (int m);
void *dma_alloc (int size);
void dma_free (void *p);
void dma_print_page(int pno);
void dma_print_bitmap();
void dma_print_blocks();
int dma_give_intfrag();

void word_to_binary( unsigned long int num, char *binary);
unsigned long int word_manipulator(int is_first, int start, int size);
#endif