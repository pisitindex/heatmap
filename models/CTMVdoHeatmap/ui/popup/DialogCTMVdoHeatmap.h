#ifndef DialogCTMVdoHeatmap_H
#define DialogCTMVdoHeatmap_H

#include <QDialog>
#include <QJsonObject>
#include <QEvent>
#include <iostream>

#include <ros/ros.h>
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include <QThread>
#include <QJsonObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QMutex>
#include <QLabel>

#include <opencv2/opencv.hpp>
#include <cira_lib_darknet_widget/cira_lib_darknet_widget.hpp>

namespace Ui {
class DialogCTMVdoHeatmap;
}

class DialogCTMVdoHeatmap : public QDialog
{
  Q_OBJECT

public:
  explicit DialogCTMVdoHeatmap(QWidget *parent = 0);
  ~DialogCTMVdoHeatmap();

  void changeEvent(QEvent *ev) {
    if (ev->type() == QEvent::ActivationChange)
    {
        if(this->isActiveWindow())
        {

        }
        else
        {
          if(!isPin) {
            this->close();
          }
        }
    }
  }

  bool isPin = false;

  QString style_unpin = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/unpin.png); background: none; border: none; background-repeat: none;";
  QString style_pin = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/pin.png); background: none; border: none; background-repeat: none;";

  bool is_restoring = false;
  QJsonObject saveState();
  void restoreState(QJsonObject jso);

  QLabel *label_form;
  QLabel *label_form_cfg_name;

  QMutex mtx;

  std::deque<std::vector<bbox_t>> prev_bbox_vec_deque;
  std::vector<unsigned int> track_id;

  void cmdSetParameters(QJsonObject jso_cmd);

  QString tmp_video;
  double *mon_txt_scale;

private slots:
  void on_doubleSpinBox_monitor_txt_scale_valueChanged(double arg1);

private slots:
  void on_pushButton_brows_vdo_clicked();

private slots:

  void current_config_changed(QString name);
  void config_removed(QString name);

  void on_pushButton_reset_count_clicked();

  void on_checkBox_enable_tracking_clicked(bool checked);

  void on_pushButton_config_clicked();

private:
  Ui::DialogCTMVdoHeatmap *ui;
};

#endif // DialogCTMVdoHeatmap_H
