#ifndef	_WISH_MEM_H_
#define	_WISH_MEM_H_

#ifdef __cplusplus
extern"C" {
#endif /* __cplusplus */

/**
 * Allocate an edge for the graph in 2^i resolution
 * 
 * @param size
 *   Any size lower than 2^32 + 1 and higher than 2^4 - 1,
 *   non power of 2 size will be rounded up to next
 *   and will waste memory.
 * @return
 *   - NULL on error
 */
void* allocateEdge(unsigned int size);

/**
 * Allocate an vertex for the graph 
 * 
 * @param size
 *   Any size.
 * @return
 *   - NULL on error
 */
void* allocateVertex(unsigned int size);

/**
 * Free an edge of the graph 
 * 
 * @param data
 *    Must be valid pointer else unexpected behaviour will happen. 
 */
void freeEdge(void* data);

/**
 * Free an vertex of the graph 
 * 
 * @param data
 *    Must be valid pointer else unexpected behaviour will happen. 
 */
void freeVertex(void* data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WISH_MEM_H_ */