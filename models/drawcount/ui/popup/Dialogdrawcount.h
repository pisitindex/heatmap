#ifndef Dialogdrawcount_H
#define Dialogdrawcount_H

#include <QDialog>
#include <QJsonObject>
#include <QEvent>

#include <iostream>

namespace Ui {
class Dialogdrawcount;
}

class Dialogdrawcount : public QDialog
{
  Q_OBJECT

public:
  explicit Dialogdrawcount(QWidget *parent = 0);
  ~Dialogdrawcount();

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

private:
  Ui::Dialogdrawcount *ui;
};

#endif // Dialogdrawcount_H
