#ifndef Formcombine_heatmap_H
#define Formcombine_heatmap_H

#include <QWidget>
#include <iostream>
#include <QMovie>
#include <QScreen>

#include "Threadcombine_heatmap.hpp"
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include "popup/Dialogcombine_heatmap.h"
#include <opencv2/opencv.hpp>

namespace Ui {
class Formcombine_heatmap;
}


class Formcombine_heatmap : public QWidget
{
  Q_OBJECT

public:
  explicit Formcombine_heatmap(QWidget *parent = 0);
  ~Formcombine_heatmap();

  bool nodeStatus_enable = true;
  bool nodeStatus_ready = true;
  bool nodeStatus_complete = true;

  qint64 timestamp_base = 0;
  qint64 timestamp_input = -1;

  bool isHaveError = false;

  QString style_nodeDisable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_disable.png); background: none; border: none; background-repeat: none;";
  QString style_nodeEnable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_enable.png); background: none; border: none; background-repeat: none;";
  QString tmp_folder;
  bool isPin = false;


  QMovie *mv_node_run;

  Threadcombine_heatmap *workerThread;
  Dialogcombine_heatmap *dialog;

public slots:
  void update_ui();

private slots:
  void stop_node_process();
  void on_pushButton_nodeEnable_clicked();
  void on_pushButton_prop_clicked();
  void test();

  int64 return_max_sec(QString filename);

  QString open_image_dialog();
  QString open_video_diaog();
  QString open_folder_dialog();

  void testImage(QString img_path);

  void combine_heatmap_from_folder(QString folder_path);

  void video_heatmap(QString video_path, QString img_path);

  void compare_heatmap(QString video_path, QString img_path1, QString img_path2);

  void combine_two_heatmap(QString img_path1, QString img_path2);

  void on_pushButton_video_heatmap_select_video_clicked();

  void on_pushButton_combine_heatmap_select_folder_clicked();

  void on_pushButton_video_heatmap_select_image_clicked();

  void on_pushButton_combine_heatmap_start_clicked();

  void on_pushButton_video_heatmap_start_clicked();

  void on_pushButton_compare_heatmap_select_video_clicked();

  void on_pushButton_compare_heatmap_select_image_1_clicked();

  void on_pushButton_compare_heatmap_select_image_2_clicked();

  void on_pushButton_compare_heatmap_start_clicked();

private:
  Ui::Formcombine_heatmap *ui;
};

#endif // Formcombine_heatmap_H
