# Espressif-Project

Steps for implementing a storage allocator :

1. Make sure that memory block to be pointed at is aligned correctly for the most restrictive data type
2. Each block has a header to store meta information and point at the next block
3. Maintain a list of such blocks
4. If a block is bigger than required split it and return the second half
~~5. If block is not found request memory from the OS~~
