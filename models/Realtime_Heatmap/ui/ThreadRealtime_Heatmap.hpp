#ifndef ThreadRealtime_Heatmap_HPP
#define ThreadRealtime_Heatmap_HPP

#include <ros/ros.h>
#include <cira_lib_bernoulli/general/GlobalData.hpp>
#include <QThread>
#include <QJsonObject>
#include <QElapsedTimer>
#include <opencv2/opencv.hpp>
#include <QMutex>

class ThreadRealtime_Heatmap : public QThread
{
  Q_OBJECT
public:

  QString name = "Realtime_Heatmap";

  QJsonObject payload_js_data;
  QJsonObject output_js_data;
  QJsonObject param_js_data;
  QMutex mtx;

  bool isUseImage = true;
  cv::Mat mat_im, acc, mat_hm_overview, mat_hm_raw;
  qint64 time_start;
  QElapsedTimer timer_el;
  double mon_txt_scale;


  bool isHaveError = false;

  ThreadRealtime_Heatmap() { }

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



    cv::Mat last_frame;
    //int hist_size = 128;
    double alpha = param_js_data["alpha"].toDouble();
    double beta = param_js_data["beta"].toDouble();
    mon_txt_scale = param_js_data["mon_txt_scale"].toDouble();


      //qDebug() << "sec : " << s << " -- frame_id : " << frame_id;
      //cv::imshow("frame", frame);

      cv::Mat mat_cur = cv::Mat::zeros(mat_im.rows, mat_im.cols, CV_8UC1);

      QJsonArray objs = payload_js_data["DeepDetect"].toObject()["objects"].toArray();


      for(int i = 0; i < objs.size(); i++) {
        QString name = objs[i].toObject()["name"].toString();
        if(name != "customer") continue;
        int x = objs[i].toObject()["x"].toInt();
        int y = objs[i].toObject()["y"].toInt();
        int w = objs[i].toObject()["w"].toInt();
        int h = objs[i].toObject()["h"].toInt();
        cv::rectangle(mat_cur, cv::Rect(x-w/2,y-h/2, w, h), cv::Scalar(255), -1);

      }
      //cv::resize(mat_cur, mat_cur, cv::Size(hist_size, hist_size), cv::INTER_LINEAR);

      last_frame.release();
      last_frame = mat_im;



      //for show #############

      mtx.lock();
      if(acc.empty()) {
        acc =  cv::Mat(mat_cur.rows, mat_cur.cols, CV_32FC1,cv::Scalar::all(0));
        time_start = timer_el.elapsed() / 1000;
      }
      cv::accumulate(mat_cur, acc);

      cv::Mat mat_avg;
      bool is_normalize = true;
      if(is_normalize) {
        cv::Mat m;
         double min, max;
         cv::minMaxLoc(acc, &min, &max);
         if(min != max) {
           acc.convertTo(m,CV_8UC1,255.0/(max-min),-255.0*min/(max-min));
         } else {
           acc.convertTo(m, CV_8UC1, 255.0/max);
         }
        mat_avg = m;
      } else {
        acc.convertTo(mat_avg, CV_8UC1);
      }

      mat_hm_overview.release();
      mat_hm_raw.release();

      cv::Mat mat_clr;
      cv::applyColorMap(mat_avg, mat_clr, cv::COLORMAP_TURBO);
      cv::Mat blk(mat_clr.rows, mat_clr.cols, CV_8UC3, cv::Scalar(0, 0, 0));
      mat_clr.copyTo(blk, mat_avg);
      cv::resize(blk, blk, cv::Size(last_frame.cols, last_frame.rows), 0, 0, cv::INTER_LANCZOS4);
      cv::addWeighted(last_frame, beta, blk, alpha, 0.0, mat_hm_overview);
      //cv::imshow("mat_hm_overview", mat_hm_overview);

      cv::resize(mat_avg, mat_avg, cv::Size(last_frame.cols, last_frame.rows), 0, 0, cv::INTER_LANCZOS4);
      mat_hm_raw = mat_avg;


      qint64 dur_sec = (timer_el.elapsed() / 1000) - time_start;
      QString time_str = QDateTime::fromTime_t(dur_sec).toUTC().toString("hh:mm:ss");
      draw_percent(mat_hm_overview, time_str);

      //emit monitoring();

    mtx.unlock();
    if(!GlobalData::isAllSceneCanRun) isHaveError = true;







    jso["msg"] = "Hello World";

    payload_js_data[name] = jso;
  }

public slots:
  void reset() {
    mtx.lock();
    acc.release();
    mtx.unlock();
  }

};

#endif // ThreadRealtime_Heatmap_HPP
