#include <hppmanipulationwidgetsplugin/roadmap.hh>

#include <string>
#include <sstream>

#include <hpp/gui/mainwindow.h>
#include <hpp/gui/windows-manager.h>

ManipulationRoadmap::ManipulationRoadmap(HppManipulationWidgetsPlugin *plugin):
  Roadmap (plugin),
  plugin_ (plugin),
  nodeColorMap_ (0)
{}

void ManipulationRoadmap::initRoadmap(const std::string jointName)
{
  Roadmap::initRoadmap (jointName);
  /// TODO: add a getter for the number of graph components
  nodeColorMap_ = ColorMap (50);
}

void ManipulationRoadmap::nodeColor (NodeID nodeId, Color& color)
{
  hpp::floatSeq_var q = plugin_->client()->problem()->node(nodeId);
  hpp::ID idGraph;
  plugin_->manipClient()->graph()->getNode(q.in(), idGraph);
  nodeColorMap_.getColor (idGraph, color);
}
