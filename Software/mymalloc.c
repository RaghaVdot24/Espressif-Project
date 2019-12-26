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