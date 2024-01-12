#ifndef FormCTMHeatmap_H
#define FormCTMHeatmap_H

#include <QWidget>
#include <iostream>
#include <QMovie>
#include <QScreen>

#include "ThreadCTMHeatmap.hpp"
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include "popup/DialogCTMHeatmap.h"

namespace Ui {
class FormCTMHeatmap;
}

class FormCTMHeatmap : public QWidget
{
  Q_OBJECT

public:
  explicit FormCTMHeatmap(QWidget *parent = 0);
  ~FormCTMHeatmap();

  bool nodeStatus_enable = true;
  bool nodeStatus_ready = true;
  bool nodeStatus_complete = true;

  qint64 timestamp_base = 0;
  qint64 timestamp_input = -1;

  bool isHaveError = false;

  QString style_nodeDisable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_disable.png); background: none; border: none; background-repeat: none;";
  QString style_nodeEnable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_enable.png); background: none; border: none; background-repeat: none;";

  QMovie *mv_node_run;

  ThreadCTMHeatmap *workerThread;
  DialogCTMHeatmap *dialog;

public slots:
  void update_ui();

private slots:
  void stop_node_process();
  void on_pushButton_nodeEnable_clicked();
  void on_pushButton_prop_clicked();

private:
  Ui::FormCTMHeatmap *ui;
};

#endif // FormCTMHeatmap_H
