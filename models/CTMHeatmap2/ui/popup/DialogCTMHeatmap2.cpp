#include "DialogCTMHeatmap2.h"
#include "ui_DialogCTMHeatmap2.h"

#include <cira_lib_yolo/cira_lib_yolo.hpp>
#include <QTimer>

#include <QFileDialog>

DialogCTMHeatmap2::DialogCTMHeatmap2(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogCTMHeatmap2)
{
  ui->setupUi(this);

  ui->tabWidget->setCurrentIndex(0);
  ui->tabWidget_2->setCurrentIndex(0);

  ui->checkBox_enable_tracking->setChecked(false);

  if(cira_lib_darknet_widget::dlg_detect_lst_config == NULL) {
    cira_lib_darknet_widget::dlg_detect_lst_config = new DialogListConfigs();
  }

  connect(cira_lib_darknet_widget::dlg_detect_lst_config, SIGNAL(config_removed(QString)), this, SLOT(config_removed(QString)));
  ui->label_config_name->setText("");
  ui->label_config_icon->setPixmap(QPixmap());


}

DialogCTMHeatmap2::~DialogCTMHeatmap2()
{
  delete ui;
}

QJsonObject DialogCTMHeatmap2::saveState() {

  QJsonObject jso;

  jso["thresh"] = ui->spinBox_threshold->value();
  jso["nms"] = ui->doubleSpinBox_nms->value();
  jso["enable_tracking"] = ui->checkBox_enable_tracking->isChecked();
  jso["roi_x"] = ui->spinBox_roi_x->value();
  jso["roi_y"] = ui->spinBox_roi_y->value();
  jso["roi_w"] = ui->spinBox_roi_w->value();
  jso["roi_h"] = ui->spinBox_roi_h->value();

  jso["frame_buffer"] = ui->spinBox_frame_buffer->value();
  jso["max_dist"] = ui->spinBox_max_distance->value();

  jso["result_drawing"] = ui->groupBox_result_drawing->isChecked();

  jso["show_confidence"] = ui->checkBox_show_percent->isChecked();
  jso["text_drawing"] = ui->groupBox_text->isChecked();
  jso["rect_thickness"] = ui->spinBox_rect_thickness->value();
  jso["font_scale"] = ui->spinBox_font_scale->value();
  jso["font_thickness"] = ui->spinBox_font_thickness->value();
  jso["txt_offset"] = ui->spinBox_font_offset->value();

  jso["draw_roi"] = ui->checkBox_is_draw_roi->isChecked();

  //optional
  jso["curr_tap"] = ui->tabWidget->currentIndex();

  //save all map_det_cfgs
  QJsonArray array;
  for(int i = 0; i < cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs.keys().size(); i++) {
      QJsonObject jso_cfg;
      QString name = cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs.keys()[i];
      if(name.isEmpty()) continue;
      QString config_path = cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]->config_path;
      if(!cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]->is_initialized) continue;
      jso_cfg["config_name"] = name;
      jso_cfg["config_path"] = config_path;
      jso_cfg["backend_gpu"] = cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]->is_backend_gpu;
      array.append(jso_cfg);
  }
  jso["map_det_cfgs"] = array;

  QString this_name = ui->label_config_name->text();
  jso["this_config_name"] = this_name;

  //#############################
  jso["alpha"] = ui->doubleSpinBox_alpha->value();
  jso["beta"] = ui->doubleSpinBox_beta->value();
  jso["mon_txt_scale"] = ui->doubleSpinBox_monitor_txt_scale->value();


  return jso;
}

void DialogCTMHeatmap2::restoreState(QJsonObject jso) {
  is_restoring = true;

  ui->spinBox_threshold->setValue(jso["thresh"].toDouble());
  ui->doubleSpinBox_nms->setValue(jso["nms"].toDouble());
  ui->checkBox_enable_tracking->setChecked(jso["enable_tracking"].toBool());
  ui->spinBox_roi_x->setValue(jso["roi_x"].toInt());
  ui->spinBox_roi_y->setValue(jso["roi_y"].toInt());
  ui->spinBox_roi_w->setValue(jso["roi_w"].toInt());
  ui->spinBox_roi_h->setValue(jso["roi_h"].toInt());
  ui->spinBox_frame_buffer->setValue(jso["frame_buffer"].toInt());
  ui->spinBox_max_distance->setValue(jso["max_dist"].toInt());

  ui->groupBox_result_drawing->setChecked(jso["result_drawing"].toBool());
  ui->checkBox_show_percent->setChecked(jso["show_confidence"].toBool());
  ui->spinBox_rect_thickness->setValue(jso["rect_thickness"].toInt());
  ui->spinBox_font_scale->setValue(jso["font_scale"].toDouble());
  ui->spinBox_font_thickness->setValue(jso["font_thickness"].toInt());
  ui->spinBox_font_offset->setValue(jso["txt_offset"].toInt());
  ui->groupBox_text->setChecked(jso["text_drawing"].toBool());

  ui->checkBox_is_draw_roi->setChecked(jso["draw_roi"].toBool());
  ui->checkBox_is_draw_roi->setChecked(true);

  QString this_name = jso["this_config_name"].toString();
  //QString config_path = jso["config_path"].toString();
  //bool is_backend_gpu = jso["backend_gpu"].toBool();

  //restore all map_det_cfgs
  QJsonArray array = jso["map_det_cfgs"].toArray();
  for(int i = 0; i < array.size(); i++) {
      QJsonObject jso_cfg = array[i].toObject();
      QString name = jso_cfg["config_name"].toString();
      QString config_path = jso_cfg["config_path"].toString();
      bool is_backend_gpu = jso_cfg["backend_gpu"].toBool();

      if(cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs.contains(name)) continue;

      if(!GlobalCiRALibYolo::GlobalCiRALibYoloObject->is_gpu_ok()) is_backend_gpu = false;
      QString backend = "CPU";
      if(is_backend_gpu) backend = "CUDA";

      QMessageBox msgBox;
      msgBox.setWindowTitle("Load config : DeepDetect");
      msgBox.setText("name : " + name + " backend : " + backend + "\n" + config_path);
      msgBox.setStandardButtons(QMessageBox::Yes);
      msgBox.addButton(QMessageBox::No);
      msgBox.setDefaultButton(QMessageBox::Yes);
      msgBox.button(QMessageBox::Yes)->animateClick(5000);

      if(msgBox.exec() == QMessageBox::Yes && cira_lib_darknet_widget::dlg_detect_lst_config->addConfigPath(name, config_path, is_backend_gpu)) {}
      else {
        cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]= new DetConfig(this, config_path, is_backend_gpu);
      }
  }

  if(cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs.contains(this_name) && cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[this_name]->is_initialized) {
      ui->label_config_name->setText(this_name);
      label_form_cfg_name->setText(this_name);
      ui->label_config_icon->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect_cpu.png"));
      label_form->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect_cpu.png"));
      if(cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[this_name]->is_backend_gpu) {
          ui->label_config_icon->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect.png"));
          label_form->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect.png"));
      }
  }

  //#############################
  ui->doubleSpinBox_alpha->setValue(jso["alpha"].toDouble());
  ui->doubleSpinBox_beta->setValue(jso["beta"].toDouble());
  ui->doubleSpinBox_monitor_txt_scale->setValue(jso["mon_txt_scale"].toDouble());


  is_restoring = false;
}

void DialogCTMHeatmap2::on_pushButton_reset_count_clicked()
{
   QString name = ui->label_config_name->text();
   if(!name.isEmpty() && cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name] && cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]->is_initialized) {
       mtx.lock();
       cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]->reset_track_id(&track_id, &prev_bbox_vec_deque);
       mtx.unlock();
   }
}

void DialogCTMHeatmap2::on_checkBox_enable_tracking_clicked(bool checked)
{
  if(checked) {
      QString name = ui->label_config_name->text();
      if(!name.isEmpty() && cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name] && cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]->is_initialized) {
          mtx.lock();
          cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]->reset_track_id(&track_id, &prev_bbox_vec_deque);
          mtx.unlock();
      }
  }
}

void DialogCTMHeatmap2::on_pushButton_config_clicked()
{
    bool pin_tmp = isPin;
    isPin = true;

    ui->pushButton_config->setEnabled(false);
    cira_lib_darknet_widget::dlg_detect_lst_config->close();

    QPoint p = this->mapToGlobal(ui->pushButton_config->pos());
    p.setY(p.y()+ui->pushButton_config->height());
    cira_lib_darknet_widget::dlg_detect_lst_config->move(p);

    cira_lib_darknet_widget::dlg_detect_lst_config->setSelectedConfig(ui->label_config_name->text());

    QMetaObject::Connection cn = connect(cira_lib_darknet_widget::dlg_detect_lst_config->lst_widget, SIGNAL(currentTextChanged(QString)), this, SLOT(current_config_changed(QString)));

    cira_lib_darknet_widget::dlg_detect_lst_config->exec();

    QObject::disconnect(cn);
    isPin = pin_tmp;

    ui->pushButton_config->setEnabled(true);
}

void DialogCTMHeatmap2::current_config_changed(QString name) {

    mtx.lock();
    if(name.isEmpty()) {
        ui->label_config_name->setText(name);
        ui->label_config_icon->setPixmap(QPixmap());
        label_form_cfg_name->setText(name);
        label_form->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect.png"));
        mtx.unlock();
        return;
    }

    ui->label_config_name->setText(name);
    ui->label_config_icon->setPixmap(QPixmap());
    label_form_cfg_name->setText(name);

    DetConfig *det_cfg = cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name];
    if(det_cfg->is_initialized) {
        if(det_cfg->is_backend_gpu) {
            ui->label_config_icon->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect.png"));
            label_form->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect.png"));
        }
        else {
            ui->label_config_icon->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect_cpu.png"));
            label_form->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect_cpu.png"));
        }
        det_cfg->reset_track_id(&track_id, &prev_bbox_vec_deque);
    }
    mtx.unlock();
}

void DialogCTMHeatmap2::config_removed(QString name) {
    if(ui->label_config_name->text() != name) return;
    mtx.lock();
    ui->label_config_name->setText("");
    label_form_cfg_name->setText("");
    ui->label_config_icon->setPixmap(QPixmap());
    label_form->setPixmap(QPixmap(":/cira_index_heatmap/model/cnn_detect.png"));
    mtx.unlock();
}

void DialogCTMHeatmap2::cmdSetParameters(QJsonObject jso_cmd) {

  if(jso_cmd.contains("thresh") && jso_cmd["thresh"].isDouble()) { ui->spinBox_threshold->setValue(jso_cmd["thresh"].toDouble()); }
  if(jso_cmd.contains("nms") && jso_cmd["nms"].isDouble()) { ui->doubleSpinBox_nms->setValue(jso_cmd["nms"].toDouble()); }

  if(jso_cmd.contains("reset") && jso_cmd["reset"].isBool() && jso_cmd["reset"].toBool()) { emit reset(); }


  if(jso_cmd.contains("reset_count") && jso_cmd["reset_count"].isBool()) {
    if(ui->checkBox_enable_tracking->isChecked() && jso_cmd["reset_count"].toBool()) {
      QString name = ui->label_config_name->text();
      if(!name.isEmpty() && cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name] && cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]->is_initialized) {
          mtx.lock();
          cira_lib_darknet_widget::dlg_detect_lst_config->map_det_cfgs[name]->reset_track_id(&track_id, &prev_bbox_vec_deque);
          mtx.unlock();
      }

    }
  }

}

void DialogCTMHeatmap2::on_doubleSpinBox_monitor_txt_scale_valueChanged(double arg1)
{
    (*mon_txt_scale) = arg1;
}

void DialogCTMHeatmap2::on_pushButton_reset_clicked()
{
    emit reset();
}
