
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

