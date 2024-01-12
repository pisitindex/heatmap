#include "ThreadCTMHeatmap2.hpp"
#include <QDebug>

#include <cira_lib_kaneki/cira_lib_kaneki.hpp>

ThreadCTMHeatmap2::ThreadCTMHeatmap2() {
  timer_el.start();
}

void ThreadCTMHeatmap2::run() {

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

  QString config_name = param_js_data["this_config_name"].toString();
  if(config_name.isEmpty()) {
    jso["error"] = "Error loading config";
    output_js_data[name] = jso;
    isHaveError = true;
    return;
  }


  DetConfig* det_config = DialogListConfigs::map_det_cfgs[config_name];
  if(!det_config || !det_config->is_initialized) {
    jso["error"] = "Error loading config";
    output_js_data[name] = jso;
    isHaveError = true;
    return;
  }

  if(config_name != tmp_config_name) { det_config->reset_track_id(track_id, prev_bbox_vec_deque); }
  tmp_config_name = config_name;
  det_config->mtx.lock();

  if(!det_config->is_initialized) {
    jso["error"] = "Error loading config";
    output_js_data[name] = jso;
    isHaveError = true;
    det_config->mtx.unlock();
    return;
  }

  set_up_parameter(det_config);

  det_config->prev_bbox_vec_deque = prev_bbox_vec_deque;
  det_config->track_id = track_id;


  //#######################################################################3


  cv::Mat last_frame;
  //int hist_size = 128;
  double alpha = param_js_data["alpha"].toDouble();
  double beta = param_js_data["beta"].toDouble();
  mon_txt_scale = param_js_data["mon_txt_scale"].toDouble();


    //qDebug() << "sec : " << s << " -- frame_id : " << frame_id;

    std::vector<bbox_t> result_vec = det_config->infer(mat_im);

    //cv::imshow("frame", frame);

    cv::Mat mat_cur = cv::Mat::zeros(mat_im.rows, mat_im.cols, CV_8UC1);
    for(int i = 0; i < result_vec.size(); i++) {
      QString name = QString::fromStdString(det_config->obj_names[(int) result_vec[i].obj_id]);
      if(name != "customer") continue;
      int x = (int) (result_vec[i].x + (result_vec[i].w/2));
      int y = (int) (result_vec[i].y + (result_vec[i].h/2));
      int w = (int) result_vec[i].w;
      int h = (int) result_vec[i].h;
      cv::rectangle(mat_cur, cv::Rect(x-w/2,y-h/2, w, h), cv::Scalar(255), -1);

    }
    //cv::resize(mat_cur, mat_cur, cv::Size(hist_size, hist_size), cv::INTER_LINEAR);

    last_frame.release();
    last_frame = mat_im;



    //for show #############
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


  if(!GlobalData::isAllSceneCanRun) isHaveError = true;

  det_config->mtx.unlock();
  payload_js_data[name] = jso;

}

void ThreadCTMHeatmap2::set_up_parameter(DetConfig* det_config) {

  det_config->thresh = param_js_data["thresh"].toDouble() / 100.0;
  det_config->nms = param_js_data["nms"].toDouble();

  det_config->roi_x = param_js_data["roi_x"].toInt();
  det_config->roi_y = param_js_data["roi_y"].toInt();
  det_config->roi_width = param_js_data["roi_w"].toInt();
  det_config->roi_height = param_js_data["roi_h"].toInt();

  det_config->is_tracking = param_js_data["enable_tracking"].toBool();

  det_config->frame_story = param_js_data["frame_buffer"].toInt();
  det_config->max_dist = param_js_data["max_dist"].toInt();

  det_config->is_draw_result = param_js_data["result_drawing"].toBool();
  det_config->is_draw_roi = param_js_data["draw_roi"].toBool();
  det_config->is_draw_text = param_js_data["text_drawing"].toBool();
  det_config->show_confidence = param_js_data["show_confidence"].toBool();
  det_config->rect_thickness = param_js_data["rect_thickness"].toInt();
  det_config->font_scale = param_js_data["font_scale"].toDouble();
  det_config->font_thickness = param_js_data["font_thickness"].toInt();
  det_config->txt_offset = param_js_data["txt_offset"].toInt();


}
