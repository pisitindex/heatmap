#include "FormCTMHeatmap2.h"
#include "ui_FormCTMHeatmap2.h"

#include <cira_lib_yolo/yolo_share.hpp>

FormCTMHeatmap2::FormCTMHeatmap2(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::FormCTMHeatmap2)
{
  ui->setupUi(this);

  timestamp_base = GlobalData::timer.nsecsElapsed();
  nodeStatus_ready = true;

  ui->label_config_name->setText("");

  workerThread = new ThreadCTMHeatmap2();



  mv_node_run = new QMovie(":/cira_index_heatmap/icon/run_led.gif");
  update_ui();
  connect(GlobalData::GlobalDataObject, SIGNAL(stopAllScene()), this, SLOT(stop_node_process()));

  if(nodeStatus_enable) {
    ui->pushButton_nodeEnable->setStyleSheet(style_nodeEnable);
  } else {
    ui->pushButton_nodeEnable->setStyleSheet(style_nodeDisable);
  }


  dialog = new DialogCTMHeatmap2();
  dialog->label_form = ui->label;
  dialog->label_form_cfg_name = ui->label_config_name;

  workerThread->prev_bbox_vec_deque = &dialog->prev_bbox_vec_deque;
  workerThread->track_id = &dialog->track_id;

  dialog->mon_txt_scale = &workerThread->mon_txt_scale;

#ifdef WIN32
   dialog->setWindowModality(Qt::NonModal); dialog->setParent(GlobalData::parent);
   if(ui->label->pixmap()) dialog->setWindowIcon(QIcon(   *ui->label->pixmap()   ));
   dialog->setWindowFlags( Qt::Window |  Qt::WindowCloseButtonHint );
#elif RASP
  dialog->setWindowModality(Qt::WindowModal);
  dialog->setWindowFlags( Qt::Tool | Qt::Dialog | Qt::WindowCloseButtonHint );
#elif __linux__
  dialog->setWindowModality(Qt::WindowModal);
  dialog->setWindowFlags( Qt::WindowCloseButtonHint );
#elif __APPLE__
  dialog->setWindowModality(Qt::WindowModal);
  dialog->setWindowFlags( Qt::Tool | Qt::Dialog | Qt::WindowCloseButtonHint );
#endif

  connect(dialog, SIGNAL(reset()), workerThread, SLOT(reset()));

}

FormCTMHeatmap2::~FormCTMHeatmap2()
{
  while (workerThread->isRunning()) { workerThread->msleep(10); }
  delete workerThread;
  delete mv_node_run;
  delete ui;
}

void FormCTMHeatmap2::init() {

  /*QString config_path = (*workerThread->jso_parameters)["config_path"].toString();
  if(!config_path.isEmpty() && workerThread->cira_yolo_->detector == NULL) {
    DialogAutoLoadConfig dlg_ld_cfg(this->parentWidget());
    dlg_ld_cfg.setWindowTitle(" CiRA AI : CTMHeatmap2");
    dlg_ld_cfg.config_dir = config_path;
    YoloShare::tmp_object_wg_path = config_path;
    if(dlg_ld_cfg.validateConfigFiles()) {
      dlg_ld_cfg.exec();
      if(dlg_ld_cfg.is_enable_auto_load_config()) {
        bool ok = workerThread->cira_yolo_->init(dlg_ld_cfg.cfg_file.toStdString(), dlg_ld_cfg.names_file.toStdString(), dlg_ld_cfg.weights_file.toStdString(), &darknet::CiRAYolo::object_detector);
        if(ok) {
          darknet::CiRAYolo::object_detector->config_path = config_path.toStdString();
          emit GlobalData::GlobalDataObject->load_yolo_weight_finished(0);
        }
      } else {
        (*workerThread->jso_parameters)["config_path"] = "";
      }
    }
  }*/

  update_ui();
}

void FormCTMHeatmap2::on_pushButton_nodeEnable_clicked()
{
  if(nodeStatus_enable) {
    ui->pushButton_nodeEnable->setStyleSheet(style_nodeDisable);
    nodeStatus_enable = false;
  } else {
    ui->pushButton_nodeEnable->setStyleSheet(style_nodeEnable);
    timestamp_base = GlobalData::timer.nsecsElapsed();
    nodeStatus_enable = true;
  }
}

void FormCTMHeatmap2::on_pushButton_prop_clicked()
{
  QSize size = qApp->screens()[0]->size();
  QPoint p = QCursor::pos();
  int w = size.width(); int h = size.height();
  int x = p.x(); int y = p.y();

  if(dialog->isVisible()) {
    if(x + dialog->width() > w) x -= x + dialog->width() - w;
    if(y + dialog->height() > h) y -= y + dialog->height() - h;
    //dialog->updateParameters();
    dialog->move(x, y);
    return;
  }

  if(x + dialog->width() > w) x -= x + dialog->width() - w;
  if(y + dialog->height() > h) y -= y + dialog->height() - h;
  dialog->move(x, y);
  //dialog->exec();
  dialog->show();
  //dialog->updateParameters();

  //delete dialog;
  //dialog = NULL;
}
