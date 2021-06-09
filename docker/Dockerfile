

from vnv_petsc:latest

ENV LIBMESH_DIR=~/software/libmesh
RUN cd /vnv-mount/applications/libmesh && ./configure --with-methods="opt oprof dbg" --prefix=${LIBMESH_DIR} --enable-silent-rules --enable-unique-id --disable-warnings --with-thread-model=openmp --with-vnv=~/software/vnv/ --with-metis=PETSc --with-parmetis=PETSc --disable-maintainer-mode --enable-petsc-hypre-required && bear make && ${VNV_MATCHER} --package LIBMESH --output src/utils/vnv --extension C compile_commands.json && make && make install 

