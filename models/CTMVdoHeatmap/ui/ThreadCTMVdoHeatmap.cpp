#include "ThreadCTMVdoHeatmap.hpp"
#include <QDebug>
#include <cira_lib_kaneki/cira_lib_kaneki.hpp>

ThreadCTMVdoHeatmap::ThreadCTMVdoHeatmap() {
}

void ThreadCTMVdoHeatmap::run() {

  mat_hm_overview.release();
  mat_hm_raw.release();

  isHaveError = false;
  output_js_data = QJsonObject();
  QJsonObject jso;

  /*if(isUseImage) {
    if(mat_im.empty()) {
      jso["error"] = "No image";
      output_js_data[name] = jso;
      isHaveError = true;
      return;
    }
  }*/

  QString vdo_path = param_js_data["vdo_path"].toString();
  qDebug() << "vdo_path : " << vdo_path;
  if(!QFile(vdo_path).exists()) {
    jso["error"] = "No VDO";
    output_js_data[name] = jso;
    isHaveError = true;
    return;
  }


  cv::VideoCapture cap;
  if(!cap.open(vdo_path.toStdString(), cv::CAP_FFMPEG)) {
    jso["error"] = "Can not Open VDO";
    output_js_data[name] = jso;
    isHaveError = true;
    return;
  }

  double fps = cap.get(cv::CAP_PROP_FPS);
  double total_frame = cap.get(cv::CAP_PROP_FRAME_COUNT);
  double dur_sec = total_frame / fps;
  double step_sec = param_js_data["step_sec"].toDouble();
  qDebug() << "fps : " << fps << " total frame : " << total_frame;


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
  std::vector<bbox_t> result_vec;

  std::vector<cv::Mat> mat_binarys;
  cv::Mat last_frame;
  int hist_size = 128;
  double alpha = param_js_data["alpha"].toDouble();
  double beta = param_js_data["beta"].toDouble();
  mon_txt_scale = param_js_data["mon_txt_scale"].toDouble();

  double global_maximum = 0;

  for(double s = 0; s < dur_sec && GlobalData::isAllSceneCanRun; s = s + step_sec) {
    double frame_id = s*fps;
    cap.set(cv::CAP_PROP_POS_FRAMES, frame_id);
    //cap.set(cv::CAP_PROP_POS_MSEC, s*1000);
    cv::Mat frame;
    cap.retrieve(frame);

    if(frame.empty()) {
        cap.set(cv::CAP_PROP_POS_FRAMES, frame_id+1);
        cv::Mat frame;
        cap.retrieve(frame);
        if(frame.empty()) continue;
    }

    //qDebug() << "sec : " << s << " -- frame_id : " << frame_id;

    std::vector<bbox_t> result_vec = det_config->infer(frame);
    //cv::imshow("frame", frame);

    cv::Mat mat_cur = cv::Mat::zeros(frame.rows, frame.cols, CV_8UC1);
    for(int i = 0; i < result_vec.size(); i++) {
      QString name = QString::fromStdString(det_config->obj_names[(int) result_vec[i].obj_id]);
      if(name != "customer") continue;
      int x = (int) (result_vec[i].x + (result_vec[i].w/2));
      int y = (int) (result_vec[i].y + (result_vec[i].h/2));
      int w = (int) result_vec[i].w;
      int h = (int) result_vec[i].h;
      //cv::rectangle(mat_cur, cv::Rect(x-w/2,y-h/2, w, h), cv::Scalar(255), -1);
      cv::rectangle(mat_cur, cv::Rect(x-w/2,y+h/4, w, h/4), cv::Scalar(1), -1);

    }
    cv::resize(mat_cur, mat_cur, cv::Size(hist_size, hist_size), cv::INTER_LINEAR);

    mat_binarys.push_back(mat_cur);

    //cv::imshow("binary", mat_cur);

    last_frame.release();
    last_frame = frame;



    //for show #############
    cv::Mat acc(hist_size, hist_size, CV_32FC1,cv::Scalar::all(0));
    for(int i = 0; i < mat_binarys.size(); i++) {
      cv::accumulate(mat_binarys[i], acc);
    }
    //acc /= mat_binarys.size();
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
      global_maximum = max;
    } else {
      acc.convertTo(mat_avg, CV_8UC1);
    }
    cv::Mat mat_clr;
    cv::applyColorMap(mat_avg, mat_clr, cv::COLORMAP_TURBO);
    cv::Mat blk(mat_clr.rows, mat_clr.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    mat_clr.copyTo(blk, mat_avg);
    cv::resize(blk, blk, cv::Size(last_frame.cols, last_frame.rows), 0, 0, cv::INTER_LANCZOS4);
    cv::addWeighted(last_frame, beta, blk, alpha, 0.0, mat_hm_overview);
    //cv::imshow("mat_hm_overview", mat_hm_overview);

    cv::resize(mat_avg, mat_avg, cv::Size(last_frame.cols, last_frame.rows), 0, 0, cv::INTER_LANCZOS4);
    mat_hm_raw = mat_avg;


    //draw percent
    int progress = (s/dur_sec)*100.0;
    QString pc_txt = QString::number(progress) + " %";\
    if(progress < 90) draw_percent(mat_hm_overview, pc_txt);

    emit monitoring();
  }

  if(!GlobalData::isAllSceneCanRun) isHaveError = true;

  det_config->mtx.unlock();
  jso["max"] = global_maximum;
  jso["step_sec"] = step_sec;
  jso["max_sec"] = global_maximum*step_sec;
  payload_js_data[name] = jso;

  msleep(500);
}

void ThreadCTMVdoHeatmap::set_up_parameter(DetConfig* det_config) {

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
