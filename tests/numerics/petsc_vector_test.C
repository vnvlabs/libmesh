#include <libmesh/petsc_vector.h>

#ifdef LIBMESH_HAVE_PETSC

#include "numeric_vector_test.h"


using namespace libMesh;

class PetscVectorTest : public NumericVectorTest<PetscVector<Number>> {
public:
  PetscVectorTest() :
    NumericVectorTest<PetscVector<Number>>() {
    if (unitlog->summarized_logs_enabled())
      this->libmesh_suite_name = "NumericVectorTest";
    else
      this->libmesh_suite_name = "PetscVectorTest";
  }

  CPPUNIT_TEST_SUITE( PetscVectorTest );

  NUMERICVECTORTEST

  CPPUNIT_TEST( testGetArray );

  CPPUNIT_TEST_SUITE_END();

  void testGetArray()
  {
    LOG_UNIT_TEST;

    unsigned int min_block_size  = 2;

    // a different size on each processor.
    unsigned int my_p = my_comm->rank();
    unsigned int local_size  = (min_block_size + my_p);
    unsigned int global_size = 0;
    unsigned int my_offset = 0;

    for (libMesh::processor_id_type p=0; p<my_comm->size(); p++)
      {
        const unsigned int p_size =
          (min_block_size + static_cast<unsigned int>(p));
        global_size += p_size;
        if (p < my_p)
          my_offset += p_size;
      }

    PetscVector<Number> v(*my_comm, global_size, local_size);

    PetscScalar * values = v.get_array();

    for (unsigned int i=0; i<local_size; i++)
      values[i] = i;

    v.restore_array();

    v.close();

    // Check the values through the interface
    for (unsigned int i=0; i<local_size; i++)
      LIBMESH_ASSERT_FP_EQUAL(i, std::abs(v(my_offset + i)), TOLERANCE*TOLERANCE);

    // Check that we can see the same thing with get_array_read
    const PetscScalar * read_only_values = v.get_array_read();

    for (unsigned int i=0; i<local_size; i++)
      LIBMESH_ASSERT_FP_EQUAL(i, std::abs(read_only_values[i]), TOLERANCE*TOLERANCE);

    v.restore_array();

    // Test getting a read only array after getting a writable array
    values = v.get_array();
    read_only_values = v.get_array_read();
    CPPUNIT_ASSERT_EQUAL((intptr_t)read_only_values, (intptr_t)values);

    v.restore_array();
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( PetscVectorTest );

#endif // #ifdef LIBMESH_HAVE_PETSC
