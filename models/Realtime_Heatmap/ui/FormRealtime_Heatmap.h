#ifndef FormRealtime_Heatmap_H
#define FormRealtime_Heatmap_H

#include <QWidget>
#include <iostream>
#include <QMovie>
#include <QScreen>

#include "ThreadRealtime_Heatmap.hpp"
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include "popup/DialogRealtime_Heatmap.h"

namespace Ui {
class FormRealtime_Heatmap;
}

class FormRealtime_Heatmap : public QWidget
{
  Q_OBJECT

public:
  explicit FormRealtime_Heatmap(QWidget *parent = 0);
  ~FormRealtime_Heatmap();

  bool nodeStatus_enable = true;
  bool nodeStatus_ready = true;
  bool nodeStatus_complete = true;

  qint64 timestamp_base = 0;
  qint64 timestamp_input = -1;

  bool isHaveError = false;

  QString style_nodeDisable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_disable.png); background: none; border: none; background-repeat: none;";
  QString style_nodeEnable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_enable.png); background: none; border: none; background-repeat: none;";

  QMovie *mv_node_run;

  ThreadRealtime_Heatmap *workerThread;
  DialogRealtime_Heatmap *dialog;

public slots:
  void update_ui();

private slots:
  void stop_node_process();
  void on_pushButton_nodeEnable_clicked();
  void on_pushButton_prop_clicked();

private:
  Ui::FormRealtime_Heatmap *ui;
};

#endif // FormRealtime_Heatmap_H
