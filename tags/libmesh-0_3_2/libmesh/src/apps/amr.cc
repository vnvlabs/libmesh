#include "libmesh.h"
#include "fe.h"
#include "quadrature_gauss.h"
#include "mesh.h"
#include "dof_map.h"
#include "general_system.h"
#include "equation_systems.h"
#include "mesh_refinement.h"


void assemble(EquationSystems& es,
	      const std::string& system_name);





int main (int argc, char** argv)
{
  libMesh::init (argc, argv);

  {
    if (argc < 4)
      std::cout << "Usage: ./prog -d DIM filename" << std::endl;
    
    // Variables to get us started
    const unsigned int dim = atoi(argv[2]);
    
    std::string meshname  (argv[3]);

    // declare a mesh...
    Mesh mesh(dim);
  
    // Read a mesh
    mesh.read(meshname);
    mesh.find_neighbors();

    mesh.elem(0)->set_refinement_flag() = Elem::REFINE;
    mesh.mesh_refinement.refine_and_coarsen_elements();    
    mesh.mesh_refinement.uniformly_refine(3);
    
    mesh.print_info();




    
    // Set up the equation system(s)
    EquationSystems es(mesh);

    es.add_system("primary");
    es("primary").add_variable("U", FIRST);
    es("primary").add_variable("V", FIRST);

    es("primary").get_dof_map().dof_coupling.resize(2);      
    es("primary").get_dof_map().dof_coupling(0,0) = 1;
    es("primary").get_dof_map().dof_coupling(1,1) = 1;
    
    es("primary").attach_assemble_function(assemble);
    
    es.init();
    
    es.print_info();
    es("primary").get_dof_map().print_dof_constraints();
    
    // call the solver.
    es("primary").solve ();
    

    
    mesh.write_gmv           ("out.gmv", es);
    mesh.write_tecplot_binary("out.plt", es);
  };
  
  return libMesh::close();
};






void assemble(EquationSystems& es,
	      const std::string& system_name)
{
  assert (system_name == "primary");
  
  const Mesh& mesh       = es.get_mesh();
  const unsigned int dim = mesh.mesh_dimension();
  const int proc_id      = mesh.processor_id();
  
  // Also use a 3x3x3 quadrature rule (3D).  Then tell the FE
  // about the geometry of the problem and the quadrature rule
  FEType fe_type (FIRST);
  
  AutoPtr<FEBase> fe(FEBase::build(dim, fe_type));
  QGauss qrule(dim, SEVENTH);
  //QTrap qrule(dim);
  
  fe->attach_quadrature_rule (&qrule);
  
  AutoPtr<FEBase> fe_face(FEBase::build(dim, fe_type));
  
  QGauss   qface(dim-1, FIFTH);
  
  fe_face->attach_quadrature_rule(&qface);
  
  
  // These are references to cell-specific data
  const std::vector<Real>& JxW_face            = fe_face->get_JxW();
  const std::vector<Real>& JxW                 = fe->get_JxW();
  const std::vector<Point>& q_point            = fe->get_xyz();
  const std::vector<std::vector<Real> >& phi   = fe->get_phi();
  const std::vector<std::vector<Point> >& dphi = fe->get_dphi();
  
  std::vector<unsigned int> dof_indices_U;
  std::vector<unsigned int> dof_indices_V;
  const DofMap& dof_map = es(system_name).get_dof_map();
  
  DenseMatrix<Number> Kuu;
  DenseMatrix<Number> Kvv;
  std::vector<Number> Fu;
  std::vector<Number> Fv;
  
  Real vol=0., area=0.;

  for (unsigned int e=0; e<mesh.n_elem(); e++)
    {
      const Elem* elem = mesh.elem(e);

      // Find the next active element on my processor
      if (elem->processor_id() != proc_id) continue;
      if (!elem->active())                 continue;

      // recompute the element-specific data for the current element
      fe->reinit (elem);

      
      //fe->print_info();

      dof_map.dof_indices(elem, dof_indices_U, 0);
      dof_map.dof_indices(elem, dof_indices_V, 1);
      
      // zero the element matrix and vector
      Kuu.resize (phi.size(),
		  phi.size());
	    
      Kvv.resize (phi.size(),
		  phi.size());
	    
      Fu.resize (phi.size());
      Fv.resize (phi.size());
	    
      for (unsigned int i=0; i<phi.size(); i++)
	Fu[i] = Fv[i] = 0.;
	    
      // standard stuff...  like in code 1.
      for (unsigned int gp=0; gp<qrule.n_points(); gp++)
	{
	  for (unsigned int i=0; i<phi.size(); ++i)
	    {
	      // this is tricky.  ig is the _global_ dof index corresponding
	      // to the _global_ vertex number elem->node(i).  Note that
	      // in general these numbers will not be the same (except for
	      // the case of one unknown per node on one subdomain) so
	      // we need to go through the dof_map
		  
	      const Real f = q_point[gp]*q_point[gp];
	      //		    const Real f = (q_point[gp](0) +
	      //				    q_point[gp](1) +
	      //				    q_point[gp](2));
		    
	      // add jac*weight*f*phi to the RHS in position ig
		    
	      Fu[i] += JxW[gp]*f*phi[i][gp];
	      Fv[i] += JxW[gp]*f*phi[i][gp];
		    
	      for (unsigned int j=0; j<phi.size(); ++j)
		{
			
		  Kuu(i,j) += JxW[gp]*((phi[i][gp])*(phi[j][gp]));
			
		  Kvv(i,j) += JxW[gp]*((phi[i][gp])*(phi[j][gp]) +
				       1.*((dphi[i][gp])*(dphi[j][gp])));
		};
	    };
	  vol += JxW[gp];
	};


      // You can't compute "area" (perimeter) if you are in 2D
      if (dim == 3)
	{
	  for (unsigned int side=0; side<elem->n_sides(); side++)
	    if (elem->neighbor(side) == NULL)
	      {
		fe_face->reinit (elem, side);
	    
		//fe_face->print_info();
	    
		for (unsigned int gp=0; gp<JxW_face.size(); gp++)
		  area += JxW_face[gp];
	      }
	}

      // Constrain the DOF indices.
      dof_map.constrain_element_matrix_and_vector(Kuu, Fu, dof_indices_U);
      dof_map.constrain_element_matrix_and_vector(Kvv, Fv, dof_indices_V);

      
      es("primary").rhs->add_vector(Fu,
				    dof_indices_U);
      es("primary").rhs->add_vector(Fv,
				    dof_indices_V);

      es("primary").matrix->add_matrix(Kuu,
				       dof_indices_U);
      es("primary").matrix->add_matrix(Kvv,
				       dof_indices_V);
    };
  
  std::cout << "Vol="  << vol << std::endl;

  if (dim == 3)
    std::cout << "Area=" << area << std::endl;
};
