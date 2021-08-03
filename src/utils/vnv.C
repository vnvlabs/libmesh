
#include <stddef.h>
#include "VnV.h"

INJECTION_EXECUTABLE(LIBMESH) 

static const char* libmesh_vnv_schema = "{\"type\": \"object\", \"required\":[]}";

/**
 * VnV allows users to set options in hypre using the input file. This callback
 * registers the json schema for the available options with the toolkit and defines
 *
 * TODO: Add options to the schema and parse them in this function.
 */ 
INJECTION_OPTIONS(LIBMESH, libmesh_vnv_schema) {

}

INJECTION_SUBPACKAGE(LIBMESH,PETSC)


/** For testing purposes only -- if you are seeing this, i forgot to remove it
 *
 */

int libmesh_vnv_test_function(int x) {
   
  INJECTION_LOOP_BEGIN("LIBMESH", VWORLD, "SanityCheck", x);
  
  for (int i = 0; i < 10; i++) {
    x += i;
    INJECTION_LOOP_ITER("LIBMESH","SanityCheck", "inner");
  }

  INJECTION_LOOP_END("LIBMESH","SanityCheck");
  return x;
}

