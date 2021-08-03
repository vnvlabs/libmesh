///18213640431074515396
/// This file was automatically generated using the VnV-Matcher executable. 
/// The matcher allows for automatic registration of all VnV plugins and injection 
/// points. Building the matcher requires Clang. If Clang is not available on this machine,
/// Registration code should be written manually. 
/// 

//PACKAGENAME: LIBMESH

#include "VnV.h" 

DECLARESUBPACKAGE(PETSC)
DECLAREOPTIONS(LIBMESH)
const char* getFullRegistrationJson_LIBMESH(){
	 return "{\"Communicator\":{\"docs\":\"\",\"name\":\"mpi\",\"package\":\"VNV\"},\"InjectionPoints\":{\"SanityCheck\":{\"docs\":\"\",\"name\":\"SanityCheck\",\"packageName\":\"LIBMESH\",\"parameters\":[{\"x\":\"int\"},{\"x\":\"int\"},{\"x\":\"int\"}],\"stages\":{\"Begin\":{\"docs\":\"\",\"info\":{\"Calling Function\":\"libmesh_vnv_test_function\",\"Calling Function Column\":1,\"Calling Function Line\":28,\"filename\":\"src/utils/vnv.C\",\"lineColumn\":5,\"lineNumber\":136}},\"End\":{\"docs\":\"\",\"info\":{\"Calling Function\":\"libmesh_vnv_test_function\",\"Calling Function Column\":1,\"Calling Function Line\":28,\"filename\":\"src/utils/vnv.C\",\"lineColumn\":5,\"lineNumber\":145}},\"inner\":{\"docs\":\"\",\"info\":{\"Calling Function\":\"libmesh_vnv_test_function\",\"Calling Function Column\":1,\"Calling Function Line\":28,\"filename\":\"src/utils/vnv.C\",\"lineColumn\":5,\"lineNumber\":149}}}}},\"Options\":\"\\n VnV allows users to set options in hypre using the input file. This callback\\n registers the json schema for the available options with the toolkit and defines\\n the options callback. Because we are in C, we are using the C interface. \\n \\n Life would be way easier if we can just compile this file with c++ :)\\n\\n TODO: Add options to the schema and parse them in this function.\\n \",\"SubPackages\":{\"PETSC\":{\"docs\":\"\",\"name\":\"PETSC\",\"packageName\":\"LIBMESH\"}}}";}

INJECTION_REGISTRATION(LIBMESH){
	REGISTERSUBPACKAGE(LIBMESH,PETSC);
	REGISTEROPTIONS(LIBMESH)
	Register_Injection_Point("LIBMESH","SanityCheck",0,"{\"x\":\"int\"}");
	REGISTER_FULL_JSON(LIBMESH, getFullRegistrationJson_LIBMESH);
};



