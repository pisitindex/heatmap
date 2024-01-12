#ifndef DialogRealtime_Heatmap_H
#define DialogRealtime_Heatmap_H

#include <QDialog>
#include <QJsonObject>
#include <QEvent>

#include <iostream>

namespace Ui {
class DialogRealtime_Heatmap;
}

class DialogRealtime_Heatmap : public QDialog
{
  Q_OBJECT

public:
  explicit DialogRealtime_Heatmap(QWidget *parent = 0);
  ~DialogRealtime_Heatmap();

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

private slots:
  void on_pushButton_reset_clicked();

private:
  Ui::DialogRealtime_Heatmap *ui;

signals:
  void reset();
};

#endif // DialogRealtime_Heatmap_H
