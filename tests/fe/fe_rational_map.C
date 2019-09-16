#include "test_comm.h"

#include <libmesh/dof_map.h>
#include <libmesh/elem.h>
#include <libmesh/equation_systems.h>
#include <libmesh/fe_base.h>
#include <libmesh/fe_interface.h>
#include <libmesh/mesh.h>
#include <libmesh/mesh_generation.h>
#include <libmesh/numeric_vector.h>
#include <libmesh/system.h>

#include <vector>

#include "libmesh_cppunit.h"


using namespace libMesh;


template <ElemType elem_type>
class RationalMapTest : public CppUnit::TestCase {

private:
  unsigned int _dim, _nx, _ny, _nz;
  Elem *_elem;
  std::vector<dof_id_type> _dof_indices;
  FEBase * _fe;
  Mesh * _mesh;
  System * _sys;
  EquationSystems * _es;

public:
  void setUp()
  {
    _mesh = new Mesh(*TestCommWorld);
    const std::unique_ptr<Elem> test_elem = Elem::build(elem_type);
    _dim = test_elem->dim();
    const unsigned int ny = _dim > 1;
    const unsigned int nz = _dim > 2;

    // Make sure we can handle non-zero weight indices
    _mesh->add_node_integer("buffer integer1");
    _mesh->add_node_integer("buffer integer2");
    unsigned char weight_index = cast_int<unsigned char>
      (_mesh->add_node_datum<Real>("rational_weight"));
    libmesh_assert_not_equal_to(weight_index, 0);

    _mesh->set_default_mapping_type(RATIONAL_BERNSTEIN_MAP);
    _mesh->set_default_mapping_data(weight_index);

    MeshTools::Generation::build_cube (*_mesh,
                                       1, ny, nz,
                                       0., 1., 0., ny, 0., nz,
                                       elem_type);

    for (auto elem : _mesh->element_ptr_range())
      {
        CPPUNIT_ASSERT_EQUAL(elem->mapping_type(), RATIONAL_BERNSTEIN_MAP);
        CPPUNIT_ASSERT_EQUAL(elem->mapping_data(), weight_index);
      }

    // Transform the cube / square into a rotated quarter-annulus,
    // with central axis at (-.5, 0) and radii ranging from .5 to 1.5

    for (auto node : _mesh->node_ptr_range())
      {
        Real & x = (*node)(0);
        Real & y = (*node)(1);
        node->set_extra_datum<Real>(weight_index, 1);
        if (y > .6)
          {
            y = .5 + x;
            x = -.5;
          }
        else if (y > .4)
          {
            y = .5 + x;
            x = y - .5;
            node->set_extra_datum<Real>(weight_index, sqrt(Real(2))/2);
          }
      }

    _es = new EquationSystems(*_mesh);
    _sys = &(_es->add_system<System> ("SimpleSystem"));
    _sys->add_variable("u", FIRST);
    _es->init();

    _fe = FEBase::build(_dim, _sys->variable_type("u")).release();
    _fe->get_xyz();
    _fe->get_phi();
    _fe->get_dphi();
    _fe->get_dphidx();
#if LIBMESH_DIM > 1
    _fe->get_dphidy();
#endif
#if LIBMESH_DIM > 2
    _fe->get_dphidz();
#endif

    auto rng = _mesh->active_local_element_ptr_range();
    _elem = rng.begin() == rng.end() ? nullptr : *(rng.begin());

    _nx = 10;
    _ny = (_dim > 1) ? _nx : 0;
    _nz = (_dim > 2) ? _nx : 0;
  }

  void tearDown()
  {
    delete _fe;
    delete _es;
    delete _mesh;
  }

  void testMap()
  {
    // Handle the "more processors than elements" case
    if (!_elem)
      return;

    // These tests require exceptions to be enabled because a
    // TypeTensor::solve() call down in Elem::contains_point()
    // actually throws a non-fatal exception for a certain Point which
    // is not in the Elem. When exceptions are not enabled, this test
    // simply aborts.
#ifdef LIBMESH_ENABLE_EXCEPTIONS
    for (unsigned int i=0; i != _nx+1; ++i)
      for (unsigned int j=0; j != _ny+1; ++j)
        for (unsigned int k=0; k != _nz+1; ++k)
          {
            Real r = (Real(i)/_nx) + 0.5,
                 theta = (Real(j)/_nx)*pi/2,
                 z = (Real(k)/_nx);
            Real x = -.5 + r * std::cos(theta),
                 y = r * std::sin(theta);
            Point p(x,y,z);
            CPPUNIT_ASSERT(_elem->contains_point(p));
          }
#endif
  }
};


#define INSTANTIATE_RATIONALMAP_TEST(elemtype)                          \
  class RationalMapTest_##elemtype : public RationalMapTest<elemtype> { \
  public:                                                               \
  CPPUNIT_TEST_SUITE( RationalMapTest_##elemtype );                     \
  CPPUNIT_TEST( testMap );                                              \
  CPPUNIT_TEST_SUITE_END();                                             \
  };                                                                    \
                                                                        \
  CPPUNIT_TEST_SUITE_REGISTRATION( RationalMapTest_##elemtype );

INSTANTIATE_RATIONALMAP_TEST(EDGE3);
INSTANTIATE_RATIONALMAP_TEST(QUAD8);
INSTANTIATE_RATIONALMAP_TEST(QUAD9);
INSTANTIATE_RATIONALMAP_TEST(HEX20);
INSTANTIATE_RATIONALMAP_TEST(HEX27);