
ARG FROM_IMAGE=vnv_petsc
FROM ${FROM_IMAGE}:latest

ARG COMMIT_HASH=unknown
LABEL vnv.version.libmesh=${COMMIT_HASH}

ENV VNV_CONFIG=${VNV_CONFIG}:${SOURCE_DIR}/libmesh/.vnv-config.json \
    LIBMESH_DIR=${SOFTWARE_DIR}/libmesh \
    OMPI_ALLOW_RUN_AS_ROOT=1 \
    OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1 

COPY ./ ${SOURCE_DIR}/libmesh 

#Install LIBMESH VNV
RUN  cd ${SOURCE_DIR}/libmesh \
     && ./configure --with-methods="opt"\
                    --with-mpi-lib=/usr/lib/x86_64-linux-gnu/\
                    --with-mpi-include=/usr/include/x86_64-linux-gnu/mpich\
                    --prefix=${LIBMESH_DIR} \
                    --enable-silent-rules \
                    --enable-unique-id \
                    --disable-warnings \
                    --with-thread-model=openmp\
                    --with-vnv=${VNV_DIR}\
                    --with-metis=PETSc\
                    --with-parmetis=PETSc\
                    --disable-maintainer-mode\
                    --enable-petsc-hypre-required \
    && bear make \
    && ${VNV_MATCHER} --package LIBMESH \
                      --output src/utils/vnv_LIBMESH.C \
                      --fix-omp \
		      --ignore-dir ./contrib \
                      --cache src/utils/vnv.__cache__\
                      compile_commands.json\
    && make \
    && make install








