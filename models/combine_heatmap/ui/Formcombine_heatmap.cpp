#include "Formcombine_heatmap.h"
#include "ui_Formcombine_heatmap.h"
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <typeinfo>

Formcombine_heatmap::Formcombine_heatmap(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Formcombine_heatmap)
{
  ui->setupUi(this);

  timestamp_base = GlobalData::timer.nsecsElapsed();
  nodeStatus_ready = true;

  workerThread = new Threadcombine_heatmap();
  dialog = new Dialogcombine_heatmap();

  mv_node_run = new QMovie(":/cira_index_heatmap/icon/run_led.gif");
  update_ui();
  connect(GlobalData::GlobalDataObject, SIGNAL(stopAllScene()), this, SLOT(stop_node_process()));

  dialog->label_combine_heatmap_select_folder_path = ui->label_combine_heatmap_select_folder_path;
  dialog->label_video_heatmap_select_video_path = ui->label_video_heatmap_select_video_path;
  dialog->label_video_heatmap_select_image_path = ui->label_video_heatmap_select_image_path;
  dialog->label_compare_heatmap_select_video_patah = ui->label_compare_heatmap_select_video_patah;
  dialog->label_compare_heatmap_select_image_path_1 = ui->label_compare_heatmap_select_image_path_1;
  dialog->label_compare_heatmap_select_image_path_2 = ui->label_compare_heatmap_select_image_path_2;

}

Formcombine_heatmap::~Formcombine_heatmap()
{
  delete ui;
}

void Formcombine_heatmap::on_pushButton_nodeEnable_clicked()
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

void Formcombine_heatmap::on_pushButton_prop_clicked()
{
  dialog->setWindowModality(Qt::NonModal); dialog->setParent(GlobalData::parent);
  dialog->setWindowFlags( /*Qt::WindowTitleHint | Qt::FramelessWindowHint |*/
                                      Qt::Window |  Qt::WindowCloseButtonHint /* | Qt::Popup*/);

  if(ui->label->pixmap()) dialog->setWindowIcon(QIcon(   *ui->label->pixmap()   ));

  QSize size = qApp->screens()[0]->size();
  QPoint p = QCursor::pos();
  int w = size.width();
  int h = size.height();
  int x = p.x();
  int y = p.y();
  if(x + dialog->width() > w) {
    x -= x + dialog->width() - w;
  }
  if(y + dialog->height() > h) {
    y -= y + dialog->height() - h;
  }

  dialog->move(x, y);
  dialog->setVisible(true);
}

//---------------------- test function ---------------------------//

void Formcombine_heatmap::test(){
    qDebug() << "Hello world";
}

void Formcombine_heatmap::testImage(QString img_path){
    cv::Mat img;
    img = cv::imread(img_path.toStdString());
    cv::imshow("before",img);
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);// เปลี่ยนให้เป็น 1 channel 0 -> 255
    cv::imshow("after",img);
}

void Formcombine_heatmap::combine_two_heatmap(QString img_path1, QString img_path2){
    cv::Mat img1, img2, sum_img;
    img1 = cv::imread(img_path1.toStdString());
    img2 = cv::imread(img_path2.toStdString());

    double max_sec_img1, max_sec_img2;
    max_sec_img1 = return_max_sec(img_path1);
    max_sec_img2 = return_max_sec(img_path2);

    if(max_sec_img1 > max_sec_img2){
        sum_img = ((img1/255) + ((img2/255)*max_sec_img2/max_sec_img1)) * 255;
        //sum_img.convertTo(sum_img, CV_8SC3);
        cv::imshow("combine result", sum_img);
    }
    else {
        sum_img = (((img1/255)*max_sec_img1/max_sec_img2) + (img2/255)) * 255;
        //sum_img.convertTo(sum_img, CV_8SC3);
        cv::imshow("combine result", sum_img);
    }
}



//----------------------------------------------------------------//

int64 Formcombine_heatmap::return_max_sec(QString filename){
    // เข้าถึง index สุดท้ายของ qtstringlist ด้วยฟังก์ชั่น takeLast()
    if(filename.contains("/")){
        // ตัด path เอาแค่ชื่อรูป
        QStringList split_foder;
        split_foder = filename.split("/");
        filename = split_foder.takeLast();
    }

    //ตัด "-" เอาแค่ตัวสุดท้าย
    QStringList split_name;
    split_name = filename.split("-");
    QString last_index;
    last_index = split_name.takeLast();
    if(last_index.contains(".png")){
        return last_index.remove(".png").toInt();
    }
    if(last_index.contains(".jpg")){
        return last_index.remove(".jpg").toInt();
    }
    if(last_index.contains(".JPG")){
        return last_index.remove(".JPG").toInt();
    }

}

QString Formcombine_heatmap::open_video_diaog(){
    // tmp มีไว้เพื่อ เมื่อเวลาโหลดวิดิโอไปครั้งนึงแล้ว เวลากดโหดใหม่จะไม่ต้องเริ่มจากหน้า Home
    // tr ครอบเพื่อให้รันได้หลาย os
    QString tmp_video;
    QString select_video = QFileDialog::getOpenFileName(
                    nullptr,
                    tr("Open Video"),
                    tmp_video,
                    tr("Video Files (*.dat *.wmv *.3gp  *.amv "
                                               "  *.avi .flv *.m1v *.mkv *.mov *.mp4 "
                                               "*.mp4v *.mpeg *.tod *.ts *.tts  *.webm"),0,QFileDialog::DontUseNativeDialog);
    if(select_video.isEmpty()) return "";

    tmp_video = select_video;
    qDebug() << tmp_video;
    return tmp_video;
}
QString Formcombine_heatmap::open_folder_dialog(){
    QString select_folder = QFileDialog::getExistingDirectory(this, "Choose Folder", get_current_dir_name());
    if(!select_folder.isEmpty()) {
      tmp_folder = select_folder;
      qDebug() << tmp_folder;
      return tmp_folder;
    }
    if(select_folder.isEmpty()) return "";
}
QString Formcombine_heatmap::open_image_dialog(){
    QString tmp_image;
    QString select_image = QFileDialog::getOpenFileName(
                    nullptr,
                    tr("Open Image"),
                    tmp_image,
                    tr("Image Files (*.jpg *.png"),0,QFileDialog::DontUseNativeDialog);

    if(!select_image.isEmpty()) {
      tmp_image = select_image;
      qDebug() << tmp_image;
      return select_image;
    }
    if(select_image.isEmpty()) return "";
}

void Formcombine_heatmap::combine_heatmap_from_folder(QString folder_path){
    qDebug() << folder_path;
    QDir directory(folder_path);
    QStringList images = directory.entryList(QStringList() << "*.png" << "*.jpg" << "*.JPG",QDir::Files);

    cv::Mat acc, m;

    for(int i = 0; i < images.size(); i++) {
        QString img_path = folder_path + "/" + images[i];
        cv::Mat tmp_img = cv::imread(img_path.toStdString());// อ่านภาพมาเป็น 3 channel RGB
        cv::cvtColor(tmp_img, tmp_img, cv::COLOR_BGR2GRAY);// เปลี่ยนให้เป็น 1 channel 0 -> 255
        tmp_img.convertTo(tmp_img, CV_32FC1, 1.0/255.0); //0-255 -> 0-1

        QStringList tmp_strlst = images[i].split(".")[0].split("-");
        double tmp_max_sec = tmp_strlst.takeLast().toDouble();

        tmp_img = tmp_img*tmp_max_sec;  //convert to second unit

        if(i==0) acc = cv::Mat(tmp_img.rows, tmp_img.cols, CV_32FC1,cv::Scalar::all(0));

        cv::accumulate(tmp_img, acc);
    }

    double min, max;
    cv::minMaxLoc(acc, &min, &max);
    if(min != max) {
      // เอา acc มาดัดให้ min = 0 max = 255 ปลายทางที่ m 
      acc.convertTo(m,CV_8UC1,255.0/(max-min),-255.0*min/(max-min)); // UC Unsigned char อัคระ 8 bits 0 - 255
    } else {
      acc.convertTo(m, CV_8UC1, 255.0/max);
    }
   cv::Mat mat_avg = m;
   int global_maximum = max;
   QString export_max_sec = QString::number(global_maximum);

   qDebug() << "global_max_sec " << global_maximum;
   cv::imshow("test", mat_avg);

   QString fp = QFileDialog::getSaveFileName(nullptr, "save combine heatmap", folder_path+"/output-"+export_max_sec+".png");
   if(!fp.isEmpty()) {
       cv::imwrite(fp.toStdString(), mat_avg);
   }

   /*

    double tmp_max_sec;
    bool first_img = true;
    int i = 0;
    foreach(QString filename, images) {
    qDebug() << i << filename;
    i++;
    if(first_img){
        // ครั้งแรกเก็บไว้ใน tmp
        tmp_img = cv::imread(folder_path.toStdString()+"/"+filename.toStdString());
        tmp_max_sec = return_max_sec(filename);
        sum_img = tmp_img;
        first_img = false;
        qDebug() << "End 1 round";

    }
    else{
        // ถ้าไม่ใช่ครั้งแรก
        qDebug() << "Next";
        double new_max_sec;
        new_max_sec = return_max_sec(filename);
        tmp_img = cv::imread(folder_path.toStdString()+"/"+filename.toStdString());

        if(new_max_sec > tmp_max_sec){
            sum_img = ((sum_img/255)*(tmp_max_sec/new_max_sec)) + (tmp_img/255);
            tmp_max_sec = new_max_sec;
            sum_img = sum_img *255;
            qDebug() << "new >";
        }
        else {
           sum_img = (sum_img/255) + ((tmp_img/255)*(new_max_sec/tmp_max_sec));
           sum_img = sum_img *255;
           qDebug() << "new <";
        }
        qDebug() << "End 1 round";

    }
    }
    cv::imshow("test", sum_img);
    */

}

void Formcombine_heatmap::video_heatmap(QString video_path, QString img_path){
    cv::Mat frame;
    cv::Mat heatmap;
    cv::Mat color_heatmap;
    cv::VideoCapture cap;

    cap.open(video_path.toStdString(), cv::CAP_FFMPEG);
    cap.read(frame);

    heatmap = cv::imread(img_path.toStdString()); // 3 ch
    cv::cvtColor(heatmap, heatmap, cv::COLOR_BGR2GRAY); // 1 ch 0-255 พร้อมทำเป็น heatmap

    //cv::applyColorMap(heatmap, heatmap, cv::COLORMAP_TURBO);
    //cv::addWeighted(frame, 0.7, heatmap, 0.6,0, color_heatmap);

    cv::Mat mat_clr;
    cv::applyColorMap(heatmap, mat_clr, cv::COLORMAP_TURBO);
    
    cv::Mat blk(mat_clr.rows, mat_clr.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    mat_clr.copyTo(blk, heatmap);
    //cv::imshow("mat_clr", mat_clr);
    //cv::imshow("blk", blk);
    
    cv::addWeighted(frame, 0.7, blk, 0.9, 0.0, color_heatmap);

    cv::imshow("video heatmap", color_heatmap);
}

void Formcombine_heatmap::compare_heatmap(QString video_path, QString img_path1, QString img_path2){

    cv::Mat frame;
    cv::Mat heatmap;
    cv::VideoCapture cap;

    cap.open(video_path.toStdString(), cv::CAP_FFMPEG);
    cap.read(frame);

    cv::Mat heatmap1, heatmap2;

    heatmap1 = cv::imread(img_path1.toStdString());
    heatmap2 = cv::imread(img_path2.toStdString());
    cv::cvtColor(heatmap1, heatmap1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(heatmap2, heatmap2, cv::COLOR_BGR2GRAY);

    QString img1 = img_path1.split("/").takeLast();
    QString img2 = img_path2.split("/").takeLast();

    QStringList tmp_qstrl_img1, tmp_qstrl_img2;
    tmp_qstrl_img1 = img1.split(".")[0].split("-");
    tmp_qstrl_img2 = img2.split(".")[0].split("-");

    double max_sec_img1, max_sec_img2;
    max_sec_img1 = tmp_qstrl_img1.takeLast().toDouble();
    max_sec_img2 = tmp_qstrl_img2.takeLast().toDouble();


    qDebug() << typeid(img1).name();

    qDebug() << typeid(max_sec_img1).name();
    qDebug() << typeid(max_sec_img2).name();

    if(max_sec_img1 > max_sec_img2){
        heatmap2 = ((heatmap2/255)*max_sec_img2/max_sec_img1)*255;
        //heatmap2.convertTo(heatmap2, CV_8UC3);
//        qDebug() << "max "+QString::number(max_sec_img1);
//        qDebug() << "min "+QString::number(max_sec_img2);

        cv::Mat mat_clr1, mat_clr2, color_heatmap1, color_heatmap2;
        cv::applyColorMap(heatmap1, mat_clr1, cv::COLORMAP_TURBO);
        cv::applyColorMap(heatmap2, mat_clr2, cv::COLORMAP_TURBO);
        //cv::imshow("heatmap1", mat_clr1);
        //cv::imshow("heatmap2", mat_clr2);

        cv::Mat blk1(mat_clr1.rows, mat_clr1.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::Mat blk2(mat_clr1.rows, mat_clr1.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        mat_clr1.copyTo(blk1, heatmap1);
        mat_clr2.copyTo(blk2, heatmap2);

        double min1, min2, max1, max2;
        cv::minMaxLoc(heatmap1, &min1, &max1);
        cv::minMaxLoc(heatmap2, &min2, &max2);
        qDebug() << "max loc heatmap1 "+img1+QString::number(max1);
        qDebug() << "min loc heatmap1 "+img1+QString::number(min1);
        qDebug() << "max loc heatmap2 "+img2+QString::number(max2);
        qDebug() << "min loc heatmap2 "+img2+QString::number(min2);

        cv::addWeighted(frame, 0.5, blk1, 0.9, 0.0, color_heatmap1);
        cv::addWeighted(frame, 0.5, blk2, 0.9, 0.0, color_heatmap2);

        cv::imshow("heatmap min "+img2.toStdString(), color_heatmap2);
        cv::imshow("heatmap max "+img1.toStdString(), color_heatmap1);

    }
    else {
        heatmap1 = ((heatmap1/255)*max_sec_img1/max_sec_img2)*255;
        //heatmap1.convertTo(heatmap1, CV_8SC3);
//        cv::imshow("heatmap min", heatmap1);
//        cv::imshow("heatmap max", heatmap2);
//        qDebug() << "max "+QString::number(max_sec_img2);
//        qDebug() << "min "+QString::number(max_sec_img1);

        cv::Mat mat_clr1, mat_clr2, color_heatmap1, color_heatmap2;
        cv::applyColorMap(heatmap1, mat_clr1, cv::COLORMAP_TURBO);
        cv::applyColorMap(heatmap2, mat_clr2, cv::COLORMAP_TURBO);
        //cv::imshow("heatmap1", mat_clr1);
        //cv::imshow("heatmap2", mat_clr2);

        cv::Mat blk1(mat_clr1.rows, mat_clr1.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::Mat blk2(mat_clr1.rows, mat_clr1.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        mat_clr1.copyTo(blk1, heatmap1);
        mat_clr2.copyTo(blk2, heatmap2);

        double min1, min2, max1, max2;
        cv::minMaxLoc(heatmap1, &min1, &max1);
        cv::minMaxLoc(heatmap2, &min2, &max2);
        qDebug() << "max loc heatmap1 "+img1+QString::number(max1);
        qDebug() << "min loc heatmap1 "+img1+QString::number(min1);
        qDebug() << "max loc heatmap2 "+img2+QString::number(max2);
        qDebug() << "min loc heatmap2 "+img2+QString::number(min2);


        cv::addWeighted(frame, 0.5, blk1, 0.9, 0.0, color_heatmap1);
        cv::addWeighted(frame, 0.5, blk2, 0.9, 0.0, color_heatmap2);

        cv::imshow("heatmap min " +img1.toStdString(), color_heatmap1);
        cv::imshow("heatmap max " +img2.toStdString(), color_heatmap2);
    }

}



//---------------------- combine heatmap ---------------------------//


void Formcombine_heatmap::on_pushButton_combine_heatmap_select_folder_clicked()
{
    ui->label_combine_heatmap_select_folder_path->setText(Formcombine_heatmap::open_folder_dialog());
}

void Formcombine_heatmap::on_pushButton_combine_heatmap_start_clicked()
{
    QString combine_heatmap_folder_path = ui->label_combine_heatmap_select_folder_path->text();
    Formcombine_heatmap::combine_heatmap_from_folder(combine_heatmap_folder_path);
}


//---------------------- video heatmap ---------------------------//

void Formcombine_heatmap::on_pushButton_video_heatmap_select_video_clicked()
{
    ui->label_video_heatmap_select_video_path->setText(Formcombine_heatmap::open_video_diaog());
}

void Formcombine_heatmap::on_pushButton_video_heatmap_select_image_clicked()
{
    ui->label_video_heatmap_select_image_path->setText(Formcombine_heatmap::open_image_dialog());
}

void Formcombine_heatmap::on_pushButton_video_heatmap_start_clicked()
{
    QString video_path = ui->label_video_heatmap_select_video_path->text();
    QString img_path = ui->label_video_heatmap_select_image_path->text();
    Formcombine_heatmap::video_heatmap(video_path, img_path);
}


//---------------------- compare heatmap ---------------------------//

void Formcombine_heatmap::on_pushButton_compare_heatmap_select_video_clicked()
{
    ui->label_compare_heatmap_select_video_patah->setText(Formcombine_heatmap::open_video_diaog());
    Formcombine_heatmap::test();
}

void Formcombine_heatmap::on_pushButton_compare_heatmap_select_image_1_clicked()
{
    ui->label_compare_heatmap_select_image_path_1->setText(Formcombine_heatmap::open_image_dialog());
    Formcombine_heatmap::test();
}

void Formcombine_heatmap::on_pushButton_compare_heatmap_select_image_2_clicked()
{
    ui->label_compare_heatmap_select_image_path_2->setText(Formcombine_heatmap::open_image_dialog());
    Formcombine_heatmap::test();
}

void Formcombine_heatmap::on_pushButton_compare_heatmap_start_clicked()
{
    QString video_path = ui->label_compare_heatmap_select_video_patah->text();
    QString img_path1 = ui->label_compare_heatmap_select_image_path_1->text();
    QString img_path2 = ui->label_compare_heatmap_select_image_path_2->text();
    Formcombine_heatmap::compare_heatmap(video_path, img_path1, img_path2);
    //Formcombine_heatmap::combine_two_heatmap(img_path1,img_path2);
    //Formcombine_heatmap::testImage(img_path1);
}
//----------------------------------------------------------------//
