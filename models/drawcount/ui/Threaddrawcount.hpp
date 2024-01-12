#ifndef Threaddrawcount_HPP
#define Threaddrawcount_HPP

#include <ros/ros.h>
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include <QThread>
#include <QJsonObject>

#include <opencv2/opencv.hpp>

class Threaddrawcount : public QThread
{
  Q_OBJECT
public:

  QString name = "drawcount";

  QJsonObject payload_js_data;
  QJsonObject output_js_data;
  QJsonObject param_js_data;

  bool isUseImage = true;
  cv::Mat mat_im;

  bool isHaveError = false;

  Threaddrawcount() { }

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

  int64 customer_in = payload_js_data["InOut"].toObject()["customer_in"].toInt();
  int64 customer_out = payload_js_data["InOut"].toObject()["customer_out"].toInt();

  double x = 0.5 * mat_im.cols;
  double y = 0.95 *mat_im.rows;

  double fontScale = 1;

  int64 thickness = 2;

  std::string word = "Customer in:" +QString::number(customer_in).toStdString() + "  " + "Customer out:" +QString::number(customer_out).toStdString();
  cv::putText(mat_im, word, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0,0,0), thickness, cv::LINE_AA);


    jso["msg"] = "Hello World";

    payload_js_data[name] = jso;
  }


};

#endif // Threaddrawcount_HPP
