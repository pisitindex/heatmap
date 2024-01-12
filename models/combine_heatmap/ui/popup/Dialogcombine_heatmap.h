#ifndef Dialogcombine_heatmap_H
#define Dialogcombine_heatmap_H

#include <QDialog>
#include <QJsonObject>
#include <QEvent>

#include <iostream>

#include <QLabel>

namespace Ui {
class Dialogcombine_heatmap;
}

class Dialogcombine_heatmap : public QDialog
{
  Q_OBJECT

public:
  explicit Dialogcombine_heatmap(QWidget *parent = 0);
  ~Dialogcombine_heatmap();

  void changeEvent(QEvent *ev) {
    if (ev->type() == QEvent::ActivationChange)
    {
        if(this->isActiveWindow())
        {

        }
        else
        {
          if(!isPin) {
            this->close();
          }
        }
    }
  }

  bool isPin = true;

  QString style_unpin = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/unpin.png); background: none; border: none; background-repeat: none;";
  QString style_pin = "background-color: transparent; border-image: url(:/cira_index_heatmap/icon/pin.png); background: none; border: none; background-repeat: none;";

  QJsonObject saveState();
  void restoreState(QJsonObject param_js_data);

  QLabel *label_combine_heatmap_select_folder_path;
  QLabel *label_video_heatmap_select_video_path;
  QLabel *label_video_heatmap_select_image_path;
  QLabel *label_compare_heatmap_select_video_patah;
  QLabel *label_compare_heatmap_select_image_path_1;
  QLabel *label_compare_heatmap_select_image_path_2;

private:
  Ui::Dialogcombine_heatmap *ui;
};

#endif // Dialogcombine_heatmap_H
