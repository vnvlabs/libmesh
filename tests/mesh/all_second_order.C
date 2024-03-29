// libMesh includes
#include <libmesh/libmesh.h>
#include <libmesh/distributed_mesh.h>
#include <libmesh/replicated_mesh.h>
#include <libmesh/mesh_generation.h>
#include <libmesh/edge_edge2.h>

// cppunit includes
#include "test_comm.h"
#include "libmesh_cppunit.h"

using namespace libMesh;

class AllSecondOrderTest : public CppUnit::TestCase
{
public:
  LIBMESH_CPPUNIT_TEST_SUITE( AllSecondOrderTest );

  CPPUNIT_TEST( allSecondOrder );
  CPPUNIT_TEST( allSecondOrderRange );
  CPPUNIT_TEST( allSecondOrderDoNothing );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {}

  void tearDown() {}

  void allSecondOrder()
  {
    LOG_UNIT_TEST;

    DistributedMesh mesh(*TestCommWorld, /*dim=*/2);

    mesh.allow_remote_element_removal(false);

    MeshTools::Generation::build_square(mesh, 2, 2);

    mesh.all_second_order();
  }

  void allSecondOrderRange()
  {
    ReplicatedMesh mesh(*TestCommWorld);

    // Construct a single-element Quad4 mesh
    MeshTools::Generation::build_square(mesh, /*nx=*/1, /*ny=*/1);

    // Add extra nodes above the four original nodes
    Point z_trans(0,0,1);
    for (dof_id_type n=0; n<4; ++n)
      mesh.add_point(mesh.point(n) + z_trans, /*id=*/4+n, /*proc_id=*/0);

    // Construct Edge2 elements attached to Quad4 at individual points
    // and in subdomain 1.
    for (dof_id_type n=0; n<4; ++n)
      {
        Elem * elem = mesh.add_elem(new Edge2);
        elem->set_node(0) = mesh.node_ptr(n);
        elem->set_node(1) = mesh.node_ptr(n+4);
        elem->subdomain_id() = 1;
      }

    // Convert only Edge2 elements (all elements in subdomain 1) to
    // SECOND-order.
    mesh.all_second_order_range(mesh.active_subdomain_elements_ptr_range(1),
                                /*full_ordered=*/true);

    // Make sure we still have the expected total number of elements
    CPPUNIT_ASSERT_EQUAL(static_cast<dof_id_type>(5), mesh.n_elem());

    // Make sure we added the right number of nodes
    CPPUNIT_ASSERT_EQUAL(static_cast<dof_id_type>(12), mesh.n_nodes());

    // Make sure that the type of the Quad4 is unchanged
    CPPUNIT_ASSERT_EQUAL(QUAD4, mesh.elem_ptr(0)->type());

    // Make sure that the other elements are now Edge3s
    for (dof_id_type e=1; e<5; ++e)
      CPPUNIT_ASSERT_EQUAL(EDGE3, mesh.elem_ptr(e)->type());
  }

  void allSecondOrderDoNothing()
  {
    DistributedMesh mesh(*TestCommWorld, /*dim=*/2);

    mesh.allow_remote_element_removal(false);

    MeshTools::Generation::build_square(mesh,
                                        /*nx=*/2, /*ny=*/2,
                                        /*xmin=*/0., /*xmax=*/1.,
                                        /*ymin=*/0., /*ymax=*/1.,
                                        QUAD9);

    // Test that all_second_order_range() correctly "does nothing" in
    // parallel when passed ranges of local elements. Here we should
    // hit one of the "early return" cases for this function.
    mesh.all_second_order_range(mesh.active_local_element_ptr_range(),
                                /*full_ordered=*/true);

    // Make sure we still have the same number of elements
    CPPUNIT_ASSERT_EQUAL(static_cast<dof_id_type>(4), mesh.n_elem());

    // Make sure we still have the same number of nodes
    CPPUNIT_ASSERT_EQUAL(static_cast<dof_id_type>(25), mesh.n_nodes());
  }
};


CPPUNIT_TEST_SUITE_REGISTRATION( AllSecondOrderTest );
