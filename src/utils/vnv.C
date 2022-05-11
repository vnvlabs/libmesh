
#include <stddef.h>
#include "VnV.h"

INJECTION_LIBRARY(LIBMESH) 

static const char* libmesh_vnv_schema = "{\"type\": \"object\", \"required\":[]}";

/**
 * Welcome to libmesh. 
 *
 *
 */ 
INJECTION_OPTIONS(LIBMESH, libmesh_vnv_schema) {
    return nullptr;
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

