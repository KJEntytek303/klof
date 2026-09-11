#ifndef KJENTYTEK_LOF_LIB
#define KJENTYTEK_LOF_LIB

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "dynamic_array.h"

#define KJEntytekLofGetArraySize( x ) ((KjentytekLofArrayHeader*)(void*)( x )-1)->size
#define KJEntytekLofGetArrayAlloc( x ) ((KjentytekLofArrayHeader*)(void*)( x )-1)->allocated

enum KjentytekLofElementTypes {/* {{{ */
	//Typesystem inherited from perl with inclusion of NULL from java-script.
	KJENTYTEK_LOF_NULL = 0,
	KJENTYTEK_LOF_SCALAR = 1,
	KJENTYTEK_LOF_ARRAY = 2,
	KJENTYTEK_LOF_STRUCT = 3,
	KJENTYTEK_LOF_UNDEFINED = 4,

	//To enable optimisation and JSON compat
	KJENTYTEK_LOF_STRING = KJENTYTEK_LOF_SCALAR,
	KJENTYTEK_LOF_INT = 0x12,
	KJENTYTEK_LOF_DOUBLE = 0x1a,
	KJENTYTEK_LOF_BOOLEAN = 0x0f
};

//If strict typing is enabled, stripping whitespace shouldn't matter
typedef struct {
	bool strict_typing;
	bool strip_predeeding_whitespace;
	bool strip_leading_whitespace;
	bool allow_underscores_in_keys;
} KjentytekLofInterpreterConfig;

typedef union {
	double double_holder;
	long int int_holder;
	void* object_holder;
} kjentytek_lof_data ;

typedef struct {
	kjentytek_lof_data data;
	char type;
} KjentytekLofDataObject;

typedef struct {
	KjentytekLofDataObject object;
	char* key;
} KjentytekLofStructEntry //extends KjentytekLofDataObject

/* }}} */

extern bool kjentytek_lof_zero_out_memory;

//Malloc and free klofs {{{
extern void kjentytekLofFreeStruct(KjentytekLofStruct** klof);
extern void kjentytekLofFreeArray(KjentytekLofDataObject** klof); //done
extern void kjentytekLofFreeScalar(KjentytekLofDataObject* klof); //done
/* }}} */

//Change between klof objects and built-in types. {{{ //done
extern KjentytekLofDataObject kjentytekLofFromInt( const int num);
extern KjentytekLofDataObject kjentytekLofFromDouble(const double num);
extern KjentytekLofDataObject kjentytekLofFromBoolean(const bool val );

//We copy the pointer and take ownership of it.
extern KjentytekLofDataObject kjentytekLofFromString(const char* str);
/* }}} */

//add, remove and set elements into Klofs {{{
extern int kjentytekLofSetIntoStruct( KjentytekLofStruct** klof, const char* key, KjentytekLofDataObject data );
extern KjentytekLofDataObject kjentytekLofGetFromStruct( const KjentytekLofStruct* klof, const char* key );
extern void kjentytekLofRemoveFromStruct( KjentytekLofStruct** klof, const char* key);

extern void kjentytekLofRemoveFromArray( KjentytekLofDataObject* array, const size_t index );
/* }}} */

//File IO of klofs {{{
extern KjentytekLofStruct* kjentytekLofReadFromFile( FILE* file, const KjentytekLofInterpreterConfig cfg );
extern int kjentytekLofWriteIntoFile( FILE* file, const KjentytekLofInterpreterConfig cfg );

extern char* kjentytekLofStructToString( KjentytekLofStruct* klof );
extern char* kjentytekLofArrayToString( KjentytekLofDataObject* klof_array  );
extern char* kjentytekLofScalarToString( KjentytekLofDataObject scalar );
/* }}} */

//Deep copying of klofs{{{
extern KjentytekLofStruct* kjentytekLofStructDeepCopy( const KjentytekLofStruct* );
extern KjentytekLofDataObject* kjentytekLofArrayDeepCopy( const KjentytekLofDataObject* klof );
extern KjentytekLofDataObject kjentytekLofScalarDeepCopy( const KjentytekLofDataObject klof);
/* }}} */

//Suggest Klof to resize it's internal buffers.{{{
extern int kjentytekLofResizeStruct( KjentytekLofStruct**, const size_t new_size );
extern int kjentytekLofResizeArray( KjentytekLofDataObject** array, const size_t new_size);
/* }}} */

#endif // KJENTYTEK_LOF_LIB

#ifdef KJENTYTEK_LOF_IMPLEMENTATION
#undef KJENTYTEK_LOF_IMPLEMENTATION


void kjentytekLofFreeScalar(KjentytekLofDataObject* klof) {/* {{{ */
	switch ( (int) klof->type ) {
		case KJENTYTEK_LOF_SCALAR:
			free(klof->data.object_holder);
			klof->data.object_holder = NULL;
			klof->type = KJENTYTEK_LOF_NULL;
			break;
		case KJENTYTEK_LOF_ARRAY:
		//	kjentytekLofFreeArray( &(klof.data.object_holder) );
		//	klof->data.object_holder = NULL;
		//	klof->type = KJENTYTEK_LOF_NULL;

			break;
		case KJENTYTEK_LOF_STRUCT:
		//	kjentytekLofFreeStruct( &(klof->data.object_holder) );
		//	klof->data.object_holder = NULL;
		//	klof->type = KJENTYTEK_LOF_NULL;
			break;
	}
}/* }}} */

void kjentytekLofFreeArray( KjentytekLofDataObject** arr ) {/* {{{ */
	size_t size = KJEntytekLofGetArraySize( *arr );
	for( size_t i=0; i<size; i++) {
		kjentytekLofFreeScalar( *arr + i );
	}
	free( ((KjentytekLofArrayHeader*)(void*)(*arr)-1) );
	*arr = NULL;
}/* }}} */

void kjentytekLofRemoveFromArray( KjentytekLofDataObject* array, const size_t index ) {
	kjentytekLofFreeScalar( array + index );
	memmove(array + index, array + index + 1, KJEntytekLofGetArraySize(array) - index -1);
}

void kjentytekLofInsertIntoStruct( KjentytekLofStructEntry** hash_map, const char* key, KjentytekLofDataObject
void kjentytekLofRemoveFromStruct( KjentytekLofStructEntry* hash_map, const char* key ) {

}



//Convertions {{{
KjentytekLofDataObject kjentytekLofFromInt( const int num ) {
	KjentytekLofDataObject ret = { .data.int_holder = num, .type = KJENTYTEK_LOF_INT };
	return ret;
}

KjentytekLofDataObject kjentytekLofFromDouble( const double num ) {
	KjentytekLofDataObject ret = { .data.double_holder = num, .type = KJENTYTEK_LOF_DOUBLE };
	return ret;
}

KjentytekLofDataObject kjentytekLofFromBoolean( const bool val ) {
	KjentytekLofDataObject ret = { .data.int_holder = val, .type = KJENTYTEK_LOF_BOOLEAN };
	return ret;
}

KjentytekLofDataObject kjentytekLofFromString( const char* str ) {
	int length = strlen(str);
	char* stored_str = malloc( length + sizeof *stored_str );
	strcpy( stored_str, str );
	KjentytekLofDataObject ret = { .data.object_holder = stored_str, .type = KJENTYTEK_LOF_STRING };
	return ret;
}
/* }}} */



#endif // KJENTYTEK_LOF_IMPLEMENTATION
