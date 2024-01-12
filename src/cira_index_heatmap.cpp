#include <cira_index_heatmap/cira_index_heatmap.hpp>
#include <cira_lib_bernoulli/CiraBernoulliWidget.h>

#include <models/CTMHeatmap/CTMHeatmapModel.hpp>
#include <models/CTMVdoHeatmap/CTMVdoHeatmapModel.hpp>
#include <models/CTMHeatmap2/CTMHeatmap2Model.hpp>
#include <models/Realtime_Heatmap/Realtime_HeatmapModel.hpp>
#include <models/drawcount/drawcountModel.hpp>
#include <models/combine_heatmap/combine_heatmapModel.hpp>

cira_index_heatmap::cira_index_heatmap() {

}

std::shared_ptr<DataModelRegistry> cira_index_heatmap::registerDataModels(std::shared_ptr<DataModelRegistry> ret) {


  ret->registerModel<CTMHeatmapModel>();
  ret->registerModel<CTMVdoHeatmapModel>();
  ret->registerModel<CTMHeatmap2Model>();
  ret->registerModel<Realtime_HeatmapModel>();
  ret->registerModel<drawcountModel>();
  ret->registerModel<combine_heatmapModel>();

  setTreeWidget();
  return ret;

}

void cira_index_heatmap::setTreeWidget() {

  QStringList strListNodeDataModels;
  QString category;

  category = "Index CTM Heatmap";
  strListNodeDataModels << category + ",CTMHeatmap"      + ",null";
  strListNodeDataModels << category + ",CTMVdoHeatmap"      + ",null";
  strListNodeDataModels << category + ",CTMHeatmap2"      + ",null";
  strListNodeDataModels << category + ",Realtime_Heatmap"      + ",null";
  strListNodeDataModels << category + ",drawcount"      + ",null";
  strListNodeDataModels << category + ",combine_heatmap"      + ",null";
  CiraBernoulliWidget::pluginTreeWidget->addNodeDataModels(strListNodeDataModels);

}
