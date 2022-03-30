
ARG FROM_IMAGE=vnv_petsc
FROM ${FROM_IMAGE}:latest


#Install LIBMESH VNV
ENV LIBMESH_DIR=/root/software/libmesh
ENV OMPI_ALLOW_RUN_AS_ROOT=1
ENV OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
COPY ./ ./libmesh 
RUN  cd libmesh && ./configure --with-methods="opt" --with-mpi-lib=/usr/lib/x86_64-linux-gnu/ --with-mpi-include=/usr/include/x86_64-linux-gnu/mpich --prefix=${LIBMESH_DIR} --enable-silent-rules --enable-unique-id --disable-warnings --with-thread-model=openmp --with-vnv=${VNV_DIR} --with-metis=PETSc --with-parmetis=PETSc --disable-maintainer-mode --enable-petsc-hypre-required 

RUN cd libmesh && python3 bear.py /usr/lib/gcc/x86_64-linux-gnu/9/include make 
RUN cd libmesh && ${VNV_MATCHER} --package LIBMESH --output src/utils/vnv_LIBMESH.C --ignore-dir ./contrib --cache src/utils/vnv.__cache__  compile_commands.json
RUN cd libmesh && make && make install

ARG COMMIT_HASH=unknown
LABEL vnv.version.libmesh=${COMMIT_HASH}
ENV VNV_CONFIG=${VNV_CONFIG}:/libmesh/.vnv-config.json







