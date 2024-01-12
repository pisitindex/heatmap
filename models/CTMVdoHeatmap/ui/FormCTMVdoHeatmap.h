#ifndef FormCTMVdoHeatmap_H
#define FormCTMVdoHeatmap_H

#include <QWidget>
#include <iostream>
#include <QMovie>
#include <QScreen>

#include "ThreadCTMVdoHeatmap.hpp"
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include "popup/DialogCTMVdoHeatmap.h"

namespace Ui {
class FormCTMVdoHeatmap;
}

class FormCTMVdoHeatmap : public QWidget
{
  Q_OBJECT

public:
  explicit FormCTMVdoHeatmap(QWidget *parent = 0);
  ~FormCTMVdoHeatmap();

  bool nodeStatus_enable = true;
  bool nodeStatus_ready = true;
  bool nodeStatus_complete = true;

  qint64 timestamp_base = 0;
  qint64 timestamp_input = -1;

  bool isHaveError = false;

  QString style_nodeDisable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_disable.png); background: none; border: none; background-repeat: none;";
  QString style_nodeEnable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_enable.png); background: none; border: none; background-repeat: none;";

  QMovie *mv_node_run;

  ThreadCTMVdoHeatmap *workerThread;
  DialogCTMVdoHeatmap *dialog = NULL;

  void init();

public slots:
  void update_ui();

private slots:
  void stop_node_process();
  void on_pushButton_nodeEnable_clicked();
  void on_pushButton_prop_clicked();

private:
  Ui::FormCTMVdoHeatmap *ui;
};

#endif // FormCTMVdoHeatmap_H
