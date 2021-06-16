

from vnv_petsc:latest

ENV LIBMESH_DIR=/root/software/libmesh
COPY . ./libmesh 
RUN  cd libmesh && ./configure --with-methods="opt oprof dbg" --prefix=${LIBMESH_DIR} --enable-silent-rules --enable-unique-id --disable-warnings --with-thread-model=openmp --with-vnv=${VNV_DIR} --with-metis=PETSc --with-parmetis=PETSc --disable-maintainer-mode --enable-petsc-hypre-required && bear make && ${VNV_MATCHER} --package LIBMESH --output src/utils/vnv --extension C compile_commands.json && make && make install 


