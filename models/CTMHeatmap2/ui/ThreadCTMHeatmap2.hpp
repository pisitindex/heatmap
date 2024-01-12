#ifndef ThreadCTMHeatmap2_HPP
#define ThreadCTMHeatmap2_HPP

#include <ros/ros.h>
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include <QThread>
#include <QJsonObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <QElapsedTimer>
#include "cira_lib_darknet_widget/widget/config/dialogListConfigs.h"

class ThreadCTMHeatmap2 : public QThread
{
  Q_OBJECT
public:

  QString name = "CTMHeatmap2";

  QJsonObject param_js_data;
  QJsonObject payload_js_data;
  QJsonObject output_js_data;

  bool isUseImage = true;
  cv::Mat mat_im, mat_hm_overview, mat_hm_raw;
  double mon_txt_scale;

  //cv::Mat mat_binary;
  cv::Mat acc;
  QMutex mtx;

  QElapsedTimer timer_el;
  qint64 time_start;

  bool isHaveError = false;

  QString tmp_config_name;
  std::deque<std::vector<bbox_t>> *prev_bbox_vec_deque;
  std::vector<unsigned int> *track_id;

  ThreadCTMHeatmap2();

  void set_up_parameter(DetConfig* det_config);

  void run();


  void draw_percent(cv::Mat &mat_result, QString txt) {

    cv::Point o;
    o.x = 10;
    o.y = 10 + 10;
    cv::Scalar color = cv::Scalar(52, 235, 143);
    double font_scale = mon_txt_scale;
    double font_thickness = mon_txt_scale*1.5;
    int rect_thickness = font_thickness*1.5;

    cv::Size const text_size = cv::getTextSize(txt.toStdString(), cv::FONT_HERSHEY_COMPLEX_SMALL, font_scale, font_thickness, 0);
    int const max_width = text_size.width;
    int const max_height = text_size.height;

    cv::Point2f p1_rect, p2_rect;
    p1_rect = cv::Point2f(std::max((int)o.x, 0), std::max((int)o.y, 0));
    p2_rect = cv::Point2f(std::min((int)o.x + max_width  + rect_thickness, mat_result.cols-1), std::min((int)o.y + max_height + rect_thickness , mat_result.rows - rect_thickness/2));

    cv::rectangle(mat_result, p1_rect,
                      p2_rect,
                      color, cv::FILLED, 8, 0);

    cv::putText(mat_result, txt.toStdString(), cv::Point2f(p1_rect.x + rect_thickness/2, p1_rect.y + max_height + rect_thickness/2), cv::FONT_HERSHEY_COMPLEX_SMALL, font_scale, cv::Scalar(0, 0, 0), font_thickness);

    o.y = p2_rect.y;
  }


signals:
  void monitoring();

public slots:
  void reset() {
    mtx.lock();
    acc.release();
    mtx.unlock();
  }

};

#endif // ThreadCTMHeatmap2_HPP
