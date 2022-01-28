///LIBMESH:6029191005160149874
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
	 return "{\"InjectionPoints\":{\"SanityCheck\":{\"docs\":{\"description\":\"\",\"instructions\":\"\",\"params\":{},\"template\":\"\",\"title\":\"\"},\"loop\":true,\"name\":\"SanityCheck\",\"packageName\":\"LIBMESH\",\"parameters\":{\"int libmesh_vnv_test_function(int)\":{\"x\":\"int\"}},\"stages\":{\"Begin\":{\"docs\":{\"description\":\"\",\"instructions\":\"\",\"params\":{},\"template\":\"\",\"title\":\"\"},\"info\":{\"Calling Function\":\"libmesh_vnv_test_function\",\"Calling Function Column\":1,\"Calling Function Line\":26,\"filename\":\"src/utils/vnv.C\",\"lineColumn\":5,\"lineNumber\":86}},\"End\":{\"docs\":{\"description\":\"\",\"instructions\":\"\",\"params\":{},\"template\":\"\",\"title\":\"\"},\"info\":{\"Calling Function\":\"libmesh_vnv_test_function\",\"Calling Function Column\":1,\"Calling Function Line\":26,\"filename\":\"src/utils/vnv.C\",\"lineColumn\":43,\"lineNumber\":101}}}}},\"Options\":{\"description\":\"\",\"instructions\":\"\",\"params\":{},\"template\":\"\\n VnV allows users to set options in hypre using the input file. This callback\\n registers the json schema for the available options with the toolkit and defines\\n\\n TODO: Add options to the schema and parse them in this function.\\n \\n\",\"title\":\"\"},\"SubPackages\":{\"PETSC\":{\"docs\":{\"description\":\"\",\"instructions\":\"\",\"params\":{},\"template\":\"\",\"title\":\"\"},\"name\":\"PETSC\",\"packageName\":\"LIBMESH\"}}}";}

INJECTION_REGISTRATION(LIBMESH){
	REGISTERSUBPACKAGE(PETSC);
	REGISTEROPTIONS(LIBMESH)
	Register_Injection_Point("LIBMESH","SanityCheck","{\"int libmesh_vnv_test_function(int)\":{\"x\":\"int\"}}");
	REGISTER_FULL_JSON(LIBMESH, getFullRegistrationJson_LIBMESH);
}



