#ifndef ThreadCTMHeatmap_HPP
#define ThreadCTMHeatmap_HPP

#include <ros/ros.h>
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include <QThread>
#include <QJsonObject>

#include <opencv2/opencv.hpp>

class ThreadCTMHeatmap : public QThread
{
  Q_OBJECT
public:

  QString name = "CTMHeatmap";

  QJsonObject payload_js_data;
  QJsonObject output_js_data;
  QJsonObject param_js_data;

  bool isUseImage = true;
  cv::Mat mat_im, mat_result;

  bool isHaveError = false;

  int hist_size = 64;
  std::vector<cv::Mat> mat_historys;

  ThreadCTMHeatmap() { }

  void run() {

    mat_result.release();

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

    if(!payload_js_data.contains("DeepDetect")) {
      jso["error"] = "No DeepDetect";
      output_js_data[name] = jso;
      isHaveError = true;
      return;
    }

    int frame_history = param_js_data["frame_history"].toInt();
    while(mat_historys.size() < frame_history) {
      mat_historys.push_back(cv::Mat::zeros(hist_size, hist_size, CV_8UC1));
    }
    while(mat_historys.size() > frame_history-1) {
      mat_historys[0].release();
      mat_historys.erase(mat_historys.begin());
    }


    QJsonArray objs = payload_js_data["DeepDetect"].toObject()["objects"].toArray();

    cv::Mat mat_cur = cv::Mat::zeros(mat_im.rows, mat_im.cols, CV_8UC1);
    for(int i = 0; i < objs.size(); i++) {
      QString name = objs[i].toObject()["name"].toString();
      if(name != "customer") continue;
      int x = objs[i].toObject()["x"].toInt();
      int y = objs[i].toObject()["y"].toInt();
      int w = objs[i].toObject()["w"].toInt();
      int h = objs[i].toObject()["h"].toInt();

      int r  = std::sqrt(w*h) / 2;
      if(r <= 0) r = 1;

      //cv::circle(mat_cur, cv::Point(x,y), r, cv::Scalar(255), cv::FILLED, 8,0);
      cv::rectangle(mat_cur, cv::Rect(x-w/2,y-h/2, w, h), cv::Scalar(255), -1);

    }

    cv::resize(mat_cur, mat_cur, cv::Size(hist_size, hist_size), cv::INTER_LINEAR);

    mat_historys.push_back(mat_cur);

    cv::Mat acc(hist_size, hist_size, CV_32FC1,cv::Scalar::all(0));
    for(int i = 0; i < mat_historys.size(); i++) {
      cv::accumulate(mat_historys[i], acc);
    }
    acc /= mat_historys.size();

    cv::Mat mat_avg;
    acc.convertTo(mat_avg, CV_8UC1);

    cv::Mat mat_clr;
    cv::applyColorMap(mat_avg, mat_clr, cv::COLORMAP_JET);

    cv::Mat blk(mat_clr.rows, mat_clr.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    mat_clr.copyTo(blk, mat_avg);

    cv::resize(blk, blk, cv::Size(mat_im.cols, mat_im.rows), 0, 0, cv::INTER_LANCZOS4);

    double alpha = param_js_data["alpha"].toDouble();
    double beta = param_js_data["beta"].toDouble();

    cv::addWeighted(mat_im, beta, blk, alpha, 0.0, mat_result);

    payload_js_data[name] = jso;

  }


};

#endif // ThreadCTMHeatmap_HPP
