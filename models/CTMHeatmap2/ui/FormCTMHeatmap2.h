#ifndef FormCTMHeatmap2_H
#define FormCTMHeatmap2_H

#include <QWidget>
#include <iostream>
#include <QMovie>
#include <QScreen>

#include "ThreadCTMHeatmap2.hpp"
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include "popup/DialogCTMHeatmap2.h"

namespace Ui {
class FormCTMHeatmap2;
}

class FormCTMHeatmap2 : public QWidget
{
  Q_OBJECT

public:
  explicit FormCTMHeatmap2(QWidget *parent = 0);
  ~FormCTMHeatmap2();

  bool nodeStatus_enable = true;
  bool nodeStatus_ready = true;
  bool nodeStatus_complete = true;

  qint64 timestamp_base = 0;
  qint64 timestamp_input = -1;

  bool isHaveError = false;

  QString style_nodeDisable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_disable.png); background: none; border: none; background-repeat: none;";
  QString style_nodeEnable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_enable.png); background: none; border: none; background-repeat: none;";

  QMovie *mv_node_run;

  ThreadCTMHeatmap2 *workerThread;
  DialogCTMHeatmap2 *dialog = NULL;

  void init();

public slots:
  void update_ui();

private slots:
  void stop_node_process();
  void on_pushButton_nodeEnable_clicked();
  void on_pushButton_prop_clicked();

private:
  Ui::FormCTMHeatmap2 *ui;
};

#endif // FormCTMHeatmap2_H
