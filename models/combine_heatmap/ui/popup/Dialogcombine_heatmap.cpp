#include "Dialogcombine_heatmap.h"
#include "ui_Dialogcombine_heatmap.h"

Dialogcombine_heatmap::Dialogcombine_heatmap(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialogcombine_heatmap)
{
  ui->setupUi(this);
}

Dialogcombine_heatmap::~Dialogcombine_heatmap()
{
  delete ui;
}

QJsonObject Dialogcombine_heatmap::saveState() {
  QJsonObject param_js_data;

  param_js_data["label_combine_heatmap_select_folder_path"] = label_combine_heatmap_select_folder_path->text();
  param_js_data["label_video_heatmap_select_video_path"] = label_video_heatmap_select_video_path->text();
  param_js_data["label_video_heatmap_select_image_path"] = label_video_heatmap_select_image_path->text();
  param_js_data["label_compare_heatmap_select_video_patah"] = label_compare_heatmap_select_video_patah->text();
  param_js_data["label_compare_heatmap_select_image_path_1"] = label_compare_heatmap_select_image_path_1->text();
  param_js_data["label_compare_heatmap_select_image_path_2"] = label_compare_heatmap_select_image_path_2->text();


  return param_js_data;
}

void Dialogcombine_heatmap::restoreState(QJsonObject param_js_data) {
    label_combine_heatmap_select_folder_path->setText(param_js_data["label_combine_heatmap_select_folder_path"].toString());
    label_video_heatmap_select_video_path->setText(param_js_data["label_video_heatmap_select_video_path"].toString());
    label_video_heatmap_select_image_path->setText(param_js_data["label_video_heatmap_select_image_path"].toString());
    label_compare_heatmap_select_video_patah->setText(param_js_data["label_compare_heatmap_select_video_patah"].toString());
    label_compare_heatmap_select_image_path_1->setText(param_js_data["label_compare_heatmap_select_image_path_1"].toString());
    label_compare_heatmap_select_image_path_2->setText(param_js_data["label_compare_heatmap_select_image_path_2"].toString());
}
