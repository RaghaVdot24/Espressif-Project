#include<stdio.h>
#include<stdlib.h>

#define REQUEST_SIZE 1024*1024*4

typedef struct Header
{
    struct Header *next;
    size_t size;
}Header;

static Header base;
static char *head = NULL;

void* my_malloc(size_t bytes)
{
    char *curr_block,*prev_block;
    Header *curr_header,*prev_header;

    prev_header = head;
    printf("In my alloc : prevhead size = %lu\n",prev_header->size);
    if(prev_header==NULL)
    {
        base.next = &base;
        head = &base;
        prev_block = &base;
        base.size = 0;
    }

    curr_header=prev_header->next;
    curr_block = curr_header+1;
    prev_block = prev_header+1;
    printf("In my alloc : currhead size = %lu, bytes = %lu\n Currblock address = %x\n",curr_header->size,bytes,curr_block);
    do
    {
        if(curr_header->size >= bytes)
        {
            if(curr_header->size == bytes)
                prev_header->next = curr_header->next;            //if block is of exact size return size
            else
            {
                curr_header->size -= bytes;                     //if block is larger split it into -- second half(size=bytes)
                printf("New currhead size = %lu\n",curr_header->size);
                curr_block += curr_header->size;                            //and first half(remaining part of the block)
                printf("New currblock addr = %x\n",curr_block);
                curr_header = curr_block;
                curr_header->size = bytes;
                printf("New currhead size = %lu\n",curr_header->size);
                curr_block = curr_header+1;
                printf("New currblock addr = %x\n",curr_block);
            }
            head = prev_header;
            return curr_block;                                    //to only return the free space and not the header part
        }

        if(curr_block==head)
        {
            return NULL;
        }
        
        prev_header=curr_header;
        curr_header=curr_header->next;
        prev_block = prev_header+1;
        curr_block = prev_header+1;
    }while(1);
}




void myfree(void* new_block)
{
    Header *block_to_insert,*curr_block;

    block_to_insert = (Header*)new_block - 1;
    
    curr_block = head;
    do
    {
        if(curr_block >= curr_block->next)                         //used to cover the edge case where curr block is last block and next block is the 1st block
            if(block_to_insert > curr_block || block_to_insert < curr_block->next)
                break;
        curr_block = curr_block->next;
    } while (!(block_to_insert > curr_block && block_to_insert < curr_block->next));
    
    
    if(((char*)block_to_insert + block_to_insert->size) == (char*)curr_block->next)   //merge with the next block
    {
        block_to_insert->size += curr_block->next->size;
        block_to_insert->next = curr_block->next->next;
    }
    else
        block_to_insert->next = curr_block->next;
    
    
    if(((char*)curr_block + curr_block->size) == (char*)block_to_insert)                //merge with previous block
    {
        curr_block->size += block_to_insert->size;
        curr_block->next = block_to_insert->next;
    }
    else
        curr_block->next = block_to_insert;

    head = curr_block;
}

void display_heap(Header *start)
{
    Header *copy = start;
    int i = 0;
    do
    {
        printf("block: %d , address: %x , size: %lu , next address: %x\n\n",++i,copy,copy->size,copy->next);
        copy = copy -> next;
    }while(copy != start);
}

int main()
{
    head = malloc(REQUEST_SIZE);
    Header *h1 = head;
    Header *start = head;
    h1->next = h1;
    h1->size = REQUEST_SIZE;
    printf("Heap before mymalloc\n");
    display_heap(start);
    int *c = my_malloc(sizeof(int));
    printf("\nHeap after mymalloc\n");
    display_heap(start);
    *c = 12;
    printf("%d\n",*c);
    myfree(c);
    printf("\nHeap after myfree\n");
    display_heap(start);
    free(start);
    return 0;
}