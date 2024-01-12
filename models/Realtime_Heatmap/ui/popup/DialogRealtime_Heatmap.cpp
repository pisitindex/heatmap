#include "DialogRealtime_Heatmap.h"
#include "ui_DialogRealtime_Heatmap.h"

DialogRealtime_Heatmap::DialogRealtime_Heatmap(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogRealtime_Heatmap)
{
  ui->setupUi(this);
}

DialogRealtime_Heatmap::~DialogRealtime_Heatmap()
{
  delete ui;
}

QJsonObject DialogRealtime_Heatmap::saveState() {
  QJsonObject param_js_data;

  param_js_data["alpha"] = ui->doubleSpinBox_alpha->value();
  param_js_data["beta"] = ui->doubleSpinBox_beta->value();
  param_js_data["mon_txt_scale"] = ui->doubleSpinBox_monitor_txt_scale->value();

  return param_js_data;
}

void DialogRealtime_Heatmap::restoreState(QJsonObject param_js_data) {
  ui->doubleSpinBox_alpha->setValue(param_js_data["alpha"].toDouble());
  ui->doubleSpinBox_beta->setValue(param_js_data["beta"].toDouble());
  ui->doubleSpinBox_monitor_txt_scale->setValue(param_js_data["mon_txt_scale"].toDouble());

}

void DialogRealtime_Heatmap::on_pushButton_reset_clicked()
{
    emit reset();
}
