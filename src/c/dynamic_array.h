#ifndef KJENTYTEK_DYNAMIC_ARRAY_LIB
#define KJENTYTEK_DYNAMIC_ARRAY_LIB

#include <stdlib.h>
#include <string.h>

typedef struct KjentytekArrayHeader { 
	size_t size;
	size_t allocated;
} KjentytekArrayHeader;

#define KJEntytekArrayMax(x, y) (((x) > (y)) ? (x) : (y))

extern int kjentytek_array_errno;

#define KJENTYTEK_ARRAY_ERRNO_MALLOC_FAILURE 1



#define KJEntytekArrayNew( elem, initial_capacity ) NULL; \
{\
	KjentytekArrayHeader* ret = malloc( sizeof( *ret ) + ( initial_capacity ) * sizeof( *elem) );\
	if( ret == NULL ) {\
		kjentytek_array_errno = KJENTYTEK_ARRAY_ERRNO_MALLOC_FAILURE ;\
		( elem ) = NULL; \
	} else { \
		ret->size=0;\
		ret->allocated = ( initial_capacity );\
		( elem ) = (void*) (ret+1);\
	} \
}

#define KJEntytekArrayFree( x ) free( (KjentytekArrayHeader*) (void*)x -1 ); x = NULL


#define KJEntytekArraySize( x ) ( (KjentytekArrayHeader*)(void*)( x )-1)->size
#define KJEntytekArrayAlloc( x ) ( (KjentytekArrayHeader*)(void*)( x )-1)->allocated

#define KJEntytekArrayRealloc( array, new_alloc ) { \
	KjentytekArrayHeader* kjentytek_array_tmp_ptr = realloc( (KjentytekArrayHeader*)(void*)(array) -1, sizeof(*kjentytek_array_tmp_ptr) + (new_alloc) * sizeof( *(array)) );\
	if( kjentytek_array_tmp_ptr == NULL ) {\
		kjentytek_array_errno = KJENTYTEK_ARRAY_ERRNO_MALLOC_FAILURE;\
	} else {\
		(array) = (void*) (kjentytek_array_tmp_ptr +1);\
		KJEntytekArrayAlloc=(new_alloc);\
	}\
}

#define KJEntytekArrayAdd(array,index,elem) \
{ \
	if( KJEntytekArrayAlloc ( array ) == KJEntytekArraySize( array ) ) {\
		size_t new_size = KJEntytekArrayAlloc( array ) + KJEntytekArrayAlloc( array ) / 2 + 1;\
		KjentytekArrayHeader* kjentytek_array_tmp_ptr = realloc( ( KjentytekArrayHeader* )(void*) (array) - 1 , sizeof(*kjentytek_array_tmp_ptr) + new_size * sizeof(*(array)) );\
\
		if( kjentytek_array_tmp_ptr == NULL ) {\
			kjentytek_array_errno = KJENTYTEK_ARRAY_ERRNO_MALLOC_FAILURE;\
		} else {\
			( array ) = (void*) (kjentytek_array_tmp_ptr + 1); \
			kjentytek_array_tmp_ptr->allocated = new_size; \
			memmove( (array) + (index) + 1, (array) + (index), ( KJEntytekArraySize( (array) ) - ( index )) * sizeof(*(array)) ); \
			( array ) [( index )] = ( elem );\
			(kjentytek_array_tmp_ptr->size)++; \
		}\
	} else { \
		memmove( (array) + (index) + 1, (array) + (index), (KJEntytekArraySize( (array) ) - ( index )) * sizeof( *(array) ) ); \
		( array )[( index )] = ( elem );\
		KJEntytekArraySize( array )++; \
	} \
}

#define KJEntytekArrayRemove( array, index ) \
	memmove( ((array) + (index)), ((array) + (index) + 1), ((KJEntytekArraySize( array )) - (index) -1)*sizeof( *array)); \
	KJEntytekArraySize( array )--

#define KJEntytekArrayEmpty( array ) ( (KjentytekArrayHeader*)(void*)(array) -1 )->size = 0;
#define KJEntytekArrayAllocToSize( array ) KJEntytekArrayRealloc( (array), KJEntytekArraySize( (array)) )
#define KJEntytekArrayPop( array ) KJEntytekArraySize( array )--;
#define KJEntytekArrayPush( array, elem ) KJEntytekArrayAdd( (array), KJEntytekArraySize( (array) ), (elem))


#endif //KJENTYTEK_DYNAMIC_ARRAY_LIB
#ifdef KJENTYTEK_DYNAMIC_ARRAY_IMPLEMENTATION
#undef KJENTYTEK_DYNAMIC_ARRAY_IMPLEMENTATION

int kjentytek_array_errno = 0;
#endif //KJENTYTEK_DYNAMIC_ARRAY_IMPLEMENTATION

