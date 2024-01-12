#ifndef Formdrawcount_H
#define Formdrawcount_H

#include <QWidget>
#include <iostream>
#include <QMovie>
#include <QScreen>

#include "Threaddrawcount.hpp"
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include "popup/Dialogdrawcount.h"

namespace Ui {
class Formdrawcount;
}

class Formdrawcount : public QWidget
{
  Q_OBJECT

public:
  explicit Formdrawcount(QWidget *parent = 0);
  ~Formdrawcount();

  bool nodeStatus_enable = true;
  bool nodeStatus_ready = true;
  bool nodeStatus_complete = true;

  qint64 timestamp_base = 0;
  qint64 timestamp_input = -1;

  bool isHaveError = false;

  QString style_nodeDisable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_disable.png); background: none; border: none; background-repeat: none;";
  QString style_nodeEnable = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/node_enable.png); background: none; border: none; background-repeat: none;";

  QMovie *mv_node_run;

  Threaddrawcount *workerThread;
  Dialogdrawcount *dialog;

public slots:
  void update_ui();

private slots:
  void stop_node_process();
  void on_pushButton_nodeEnable_clicked();
  void on_pushButton_prop_clicked();

private:
  Ui::Formdrawcount *ui;
};

#endif // Formdrawcount_H
