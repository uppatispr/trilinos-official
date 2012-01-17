
#include "Panzer_IntegrationRule.hpp"

#include "Teuchos_Assert.hpp"
#include "Phalanx_DataLayout_MDALayout.hpp"
#include "Intrepid_DefaultCubatureFactory.hpp"
#include "Panzer_Dimension.hpp"
#include "Panzer_CellData.hpp"

panzer::IntegrationRule::
IntegrationRule(int in_cubature_degree, const panzer::CellData& cell_data) :
  side(-1)
{
  /*
  int dim = cell_data.baseCellDimension();

  Teuchos::RCP<shards::CellTopology> baseTopo;
  if (dim == 3)
    baseTopo = Teuchos::rcp(new shards::CellTopology(shards::getCellTopologyData< shards::Hexahedron<8> >()));
  else if (dim == 2)
    baseTopo = Teuchos::rcp(new shards::CellTopology(shards::getCellTopologyData< shards::Quadrilateral<4> >()));
  else if (dim == 1)
    baseTopo = Teuchos::rcp(new shards::CellTopology(shards::getCellTopologyData< shards::Line<2> >()));
  */

  setup(in_cubature_degree,cell_data);
}

void panzer::IntegrationRule::setup(int in_cubature_degree, const panzer::CellData& cell_data)
{
  cubature_degree = in_cubature_degree ;
  spatial_dimension = cell_data.baseCellDimension();
  workset_size = cell_data.numCells();
  
  topology = cell_data.getCellTopology();
  TEUCHOS_TEST_FOR_EXCEPTION(topology==Teuchos::null,std::runtime_error,
                     "Base topology from cell_data cannot be null!");
  TEUCHOS_TEST_FOR_EXCEPTION(spatial_dimension!=(int) topology->getDimension(), std::runtime_error,
		     "Spatial dimension from cell_data does not match the cell topology.");
  
  TEUCHOS_TEST_FOR_EXCEPTION(Teuchos::is_null(topology), std::runtime_error,
		     "Failed to allocate cell topology!");
  
  Intrepid::DefaultCubatureFactory<double,Intrepid::FieldContainer<double> > 
    cubature_factory;

  // Intrepid does support a quadrature on a 0-dimensional object
  // (which doesn't make much sense anyway) to work around this we
  // will adjust the integration rule manually
  if(cell_data.isSide() && spatial_dimension==1) {
     side = cell_data.side();

     // do some specialized work for nodal case
     setup0DimIntRule(); 

     return;
  }

  if (cell_data.isSide()) {
    side = cell_data.side();

    TEUCHOS_TEST_FOR_EXCEPTION( (side >= static_cast<int>(topology->getSideCount())), 
			std::runtime_error, "Error - local side " 
			<< side << " is not in range (0->" << topology->getSideCount()-1 
   			<< ") of topologic entity!");
    
    side_topology = Teuchos::rcp(new shards::CellTopology(topology->getCellTopologyData(topology->getDimension()-1,side)));
  }

  TEUCHOS_TEST_FOR_EXCEPTION(side >= 0 && Teuchos::is_null(side_topology), 
		     std::runtime_error,
		     "Failed to allocate side topology!");
  
  Teuchos::RCP<Intrepid::Cubature<double,Intrepid::FieldContainer<double>  > > 
    intrepid_cubature;

  if (Teuchos::is_null(side_topology))
    intrepid_cubature = cubature_factory.create(*topology, cubature_degree);
  else
    intrepid_cubature = cubature_factory.create(*side_topology, cubature_degree);

  num_points = intrepid_cubature->getNumPoints();

  using Teuchos::rcp;
  using PHX::MDALayout;
  
  dl_scalar = 
    rcp(new MDALayout<Cell,IP>(workset_size,num_points));
  
  dl_vector = 
    rcp(new MDALayout<Cell,IP,Dim>(workset_size, num_points,
				   spatial_dimension));
  
  dl_tensor = 
    rcp(new MDALayout<Cell,IP,Dim,Dim>(workset_size, num_points,
				       spatial_dimension,
				       spatial_dimension));

}

bool panzer::IntegrationRule::isSide()
{
  return (!Teuchos::is_null(side_topology));
}

// setup 0-dimensional integration rule (a node)
void panzer::IntegrationRule::setup0DimIntRule()
{
  side_topology = Teuchos::rcp(new shards::CellTopology(shards::getCellTopologyData<shards::Node>()));

  num_points = 1;

  using Teuchos::rcp;
  using PHX::MDALayout;
  
  dl_scalar = 
    rcp(new MDALayout<Cell,IP>(workset_size,num_points));
  
  dl_vector = 
    rcp(new MDALayout<Cell,IP,Dim>(workset_size, num_points,
				   spatial_dimension));
  
  dl_tensor = 
    rcp(new MDALayout<Cell,IP,Dim,Dim>(workset_size, num_points,
				       spatial_dimension,
				       spatial_dimension));
}

void panzer::IntegrationRule::print(std::ostream & os)
{
   os << "Degree = " << cubature_degree 
      << ", Dimension = " << spatial_dimension 
      << ", Workset Size = " << workset_size
      << ", Num Points = " << num_points 
      << ", Side = " << side;
}
