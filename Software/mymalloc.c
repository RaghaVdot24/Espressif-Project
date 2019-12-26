#include<stdio.h>

typedef unsigned long int restrictive_type;

typedef union Header
{
    struct
    {
        union Header *next;
        size_t size;
    }s;
    restrictive_type filler;
}Header;

static Header base;
static Header *head = NULL;

void* my_malloc(size_t bytes)
{
    Header *curr_block,*prev_block;
    size_t new_size;

    new_size = (bytes + sizeof(Header) - 1)/sizeof(Header) + 1;

    prev_block = head;
    if(prev_block==NULL)
    {
        base.s.next = &base;
        head = &base;
        prev_block = &base;
        base.s.size = 0;
    }

    curr_block=prev_block->s.next;
    do
    {
        if(curr_block->s.size >= new_size)
        {
            if(curr_block->s.size == new_size)
                prev_block->s.next = curr_block->s.next;            //if block is of exact size return size
            else
            {
                curr_block->s.size -= new_size;                     //if block is larger split it into -- second half(size=new_size)
                curr_block += new_size;                             //and first half(remaining part of the block)
                curr_block->s.size = new_size;
            }
            head = prev_block;
            return curr_block+1;                                    //to only return the free space and not the header part
        }

        if(curr_block==head)
        {
            //TODO request memory from OS
            return NULL;
        }
        
        prev_block=curr_block;
        curr_block=curr_block->s.next;
    }while(1);
}




void myfree(void* new_block)
{
    Header *block_to_insert,*curr_block;

    block_to_insert = (Header*)new_block - 1;
    
    curr_block = head;
    do
    {
        if(curr_block >= curr_block->s.next)                         //used to cover the edge case where curr block is last block and next block is the 1st block
            if(block_to_insert > curr_block || block_to_insert < curr_block->s.next)
                break;
        curr_block = curr_block->s.next;
    } while (!(block_to_insert > curr_block && block_to_insert < curr_block->s.next));
    
    
    if((block_to_insert + block_to_insert->s.size) == curr_block->s.next)   //merge with the next block
    {
        block_to_insert->s.size += curr_block->s.next->s.size;
        block_to_insert->s.next = curr_block->s.next->s.next;
    }
    else
        block_to_insert->s.next = curr_block->s.next;
    
    
    if((curr_block + curr_block->s.size) == block_to_insert)                //merge with previous block
    {
        curr_block->s.size += block_to_insert->s.size;
        curr_block->s.next = block_to_insert->s.next;
    }
    else
        curr_block->s.next = block_to_insert;

    head = curr_block;
}