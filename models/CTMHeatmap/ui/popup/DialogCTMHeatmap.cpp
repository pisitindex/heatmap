#include "DialogCTMHeatmap.h"
#include "ui_DialogCTMHeatmap.h"

#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include <cira_lib_bernoulli/datatype/FlowData.hpp>
#include <cira_lib_bernoulli/asm_opencv/asmOpenCV.h>

DialogCTMHeatmap::DialogCTMHeatmap(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogCTMHeatmap)
{
  ui->setupUi(this);
  
  //cira_std::connectFlowSlot("<flowName>", this, SLOT(getFlow(std::shared_ptr<FlowData>)));
  
}

DialogCTMHeatmap::~DialogCTMHeatmap()
{
  delete ui;
}



QJsonObject DialogCTMHeatmap::saveState() {
  QJsonObject param_js_data;

  param_js_data["frame_history"] = ui->spinBox_history_frame->value();
  param_js_data["alpha"] = ui->doubleSpinBox_alpha->value();
  param_js_data["beta"] = ui->doubleSpinBox_beta->value();

  return param_js_data;
}

void DialogCTMHeatmap::restoreState(QJsonObject param_js_data) {
  ui->spinBox_history_frame->setValue(param_js_data["frame_history"].toInt());
  ui->doubleSpinBox_alpha->setValue(param_js_data["alpha"].toDouble());
  ui->doubleSpinBox_beta->setValue(param_js_data["beta"].toDouble());
}
