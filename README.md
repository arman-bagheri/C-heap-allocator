A basic heap memory allocator written in C. i keep track of the space as a linked list of the free space in an ordered manner, so on malloc or free calls coalescinig happends automatically and easily.
as of now the program only implements malloc and free, all using the standard linux system calls. the algorithm for allocating is also that of a first fit approach.
