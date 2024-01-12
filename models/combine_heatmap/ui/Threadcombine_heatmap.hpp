#ifndef Threadcombine_heatmap_HPP
#define Threadcombine_heatmap_HPP

#include <ros/ros.h>
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include <QThread>
#include <QJsonObject>

#include <opencv2/opencv.hpp>

class Threadcombine_heatmap : public QThread
{
  Q_OBJECT
public:

  QString name = "combine_heatmap";

  QJsonObject payload_js_data;
  QJsonObject output_js_data;
  QJsonObject param_js_data;

  bool isUseImage = true;
  cv::Mat mat_im;

  bool isHaveError = false;

  Threadcombine_heatmap() { }

  void run() {

    isHaveError = false;
    output_js_data = QJsonObject();
    QJsonObject jso;
    if(isUseImage) {
      if(mat_im.empty()) {
        jso["error"] = "No image";
        output_js_data[name] = jso;
        isHaveError = true;
        return;
      }
    }

    //********* run here **************

    jso["msg"] = "Hello World";

    payload_js_data[name] = jso;
  }


};

#endif // Threadcombine_heatmap_HPP
