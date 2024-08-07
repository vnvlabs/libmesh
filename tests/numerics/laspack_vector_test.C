#include <libmesh/laspack_vector.h>

#ifdef LIBMESH_HAVE_LASPACK

#include "numeric_vector_test.h"


using namespace libMesh;

class LaspackVectorTest : public NumericVectorTest<LaspackVector<Number>>
{
public:
  void setUp()
  {
    // Laspack doesn't support distributed parallel vectors, but we
    // can build a serial vector on each processor
    my_comm = new Parallel::Communicator();
  }

  void tearDown()
  {
    delete my_comm;
  }

  LaspackVectorTest() :
    NumericVectorTest<LaspackVector<Number>>() {
    if (unitlog->summarized_logs_enabled())
      this->libmesh_suite_name = "NumericVectorTest";
    else
      this->libmesh_suite_name = "LaspackVectorTest";
  }

  CPPUNIT_TEST_SUITE( LaspackVectorTest );

  NUMERICVECTORTEST

  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION( LaspackVectorTest );

#endif // #ifdef LIBMESH_HAVE_LASPACK

