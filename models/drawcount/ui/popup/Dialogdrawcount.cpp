#include "Dialogdrawcount.h"
#include "ui_Dialogdrawcount.h"

Dialogdrawcount::Dialogdrawcount(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialogdrawcount)
{
  ui->setupUi(this);
}

Dialogdrawcount::~Dialogdrawcount()
{
  delete ui;
}

QJsonObject Dialogdrawcount::saveState() {
  QJsonObject param_js_data;

  return param_js_data;
}

void Dialogdrawcount::restoreState(QJsonObject param_js_data) {

}
