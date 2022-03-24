'''
    Quick and dirty wrapper around bear. VnV needs a compile_commands.json
    database file to extract all the vnv documentation from the libmesh
    source code. 

    We use bear to extract the compile_commands database during the build process.
    However, bear does not currently support the -fopenmp flag. When the fopenmp 
    flag is present, gcc automatically includes the directory containing the openmp
    include files. 

    Until that is supported, we need to manually go through the compile commands and 
    add the openmp include directory to the compilation arguments every time we see
    the -fopenmp flag. 

    Usage:
       python bear.py <omp-include-directory> <bear compile commands> 

    E.g
       python bear.py /path/to/omp/include make 

    The easiest way to find the omp.h include file is to type 

    locate omp.h 

    make sure to pick the correct directory for your compiler!!!!

    Note: apt-get install mlocate to install locate if you dont have it. 
'''

import json
import sys
import subprocess

subprocess.run(["bear"] + sys.argv[2:])

comps = {}
with open("compile_commands.json",'r') as w:
   comps = json.load(w)
   for i in comps:
       args = i.get("arguments",[])
       try:
            ind = args.index('-fopenmp')
            args.insert(ind,"-I"+sys.argv[1])
       except ValueError as e:
            pass

with open("compile_commands.json",'w') as w:
     json.dump(comps, w, indent=4)
