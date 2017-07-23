


wish_mem lib:
Wish_mem lib was recreated to solve performance issues saw by previous wish_mem memory allocator
in large memory machines.


Since a lot of alloc \ free actions the dynamic fragmentations cause to various memory allocation time
in the previous wish_mem allocator especially when trying to allocate big edges (O(dynamic holes number) becomes significant in large memory machines))     


  
it does it by saving memory pools to known edge sizes (2^4, 2^5 ... 2^32) which are the biggest size options to allocate(vertex can be maximum 16MB).
so,
when user allocates edge with size 2^i the memory allocator checks if buffer exists in pool i and return it by O(1) time.
if doesn't exist it will try to get buffer by malloc option (from heap).
when user frees edge, the memory allocator pushes the buffer to the free list of its pool if busy thresholds doesn't pass.

The busy threshold dynamically updates per pool each allocation time by defined formula:
	Each allocation time the allocator check how much busy buffers are in using and updates the threshold by next formula:
		(2*last threshold + new threshold)/3 +3.
		so each free time the allocator checks if busy buffers + free_buffers < busy threshold and if so return the buffer to pool else return it to heap(by free option).
		maybe, after some statistics we can tune this formula for better performance.
		
so,
by this method we prevents going to heap for allocation each time which can take a lot of time because fragmentations and mostly takes it by O(1) time from pool.

corner cases:
no memory to allocate buffer by malloc for edge or vertex (many buffers are in pool):
	The allocator find the nearest size pool (round up) which exists - free it to heap and malloc the size from heap.

not enough memory in pools and in malloc:
	return NULL - probably limit of main memory.
	

	
	
limitations:
1. Don't support in multi-threaded allocations - can fix it by adding locks to defends allocator pools DB.
2. Because I can't create destroy API (not in the requested interface) I recognizes the no busy buffer event as end of application and releases all the pool free buffers.
	this adds some checks and can hurt performance in special small Graphs.
3. Other limitations can be saw in h file.


1TB physical memory case:

Yes it works but will not support ~1TB of graph memory in main memory, somewhere before this limit the application will get NULL allocations.
it happens because my allocator and the graph application uses another memory for other DBs and for stack calculations, also the holes in the heap probably takes unnecessary memory:
for address also the  "1TB of graph memory in main memory" we needs more physical memory, can be achieved by:
a. add physical memory (ssd\disc)
b. use cloud memory in the net.
  	
	

 
 
