/** \mainpage Zagar.io
parody for Agar.io
*/

#include <stdlib.h>
#include <malloc.h>

#ifndef EXAMPLE_H
#define EXAMPLE_H

///Set maximum size.
#define EXAMPLE_BOOLEAN_ARRAY_SIZE 1

///Struct for boolean.
struct boolean {
	int state; ///<	Boolean state.
};

/**Reverse boolean state.
\param boolean*	Boolean to reverse.
\return int		Returns 0 if success, else -1.
\warning		Some warnings.
*/
int reverse_boolean(struct boolean*);
#endif
