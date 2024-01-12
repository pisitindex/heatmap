#ifndef CTMVdoHeatmapModel_HPP
#define CTMVdoHeatmapModel_HPP

#pragma once

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QShowEvent>
#include <QFuture>
#include <cira_lib_bernoulli/general/GlobalData.hpp>

#include <nodes/NodeDataModel>

#include <iostream>

#include "ui/FormCTMVdoHeatmap.h"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class FlowData;

class CTMVdoHeatmapModel : public NodeDataModel
{

  Q_OBJECT

public:
  CTMVdoHeatmapModel();

  virtual
  ~CTMVdoHeatmapModel() {}

  int portInStatus[2] = {PORTSTATUS::DISCONNECTED, PORTSTATUS::DISCONNECTED};

public:

  QString
  caption() const override
  { return QStringLiteral("CTMVdoHeatmap"); }

  bool
  captionVisible() const override
  { return true; }

  bool
  portCaptionVisible(PortType, PortIndex) const override
  { return true; }

  bool
  resizable() const override { return false; }


  QString
  name() const override
  { return QStringLiteral("CTMVdoHeatmap"); }

public:

  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &p) override;

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;

  QWidget *
  embeddedWidget() override { return form; }

private:

  bool isBusy = false;
  QMutex mutex;
  //std::shared_ptr<FlowData> _flowData;
  //FlowDataType flowdata;
  std::shared_ptr<FlowData> _flowDataOut, _flowDataOut1;
  FormCTMVdoHeatmap *form;
  QFuture<void> future;

  QTimer *timerLoadFromButton;
  bool is_updata_ui = true;

private slots:
  void runProcess(std::shared_ptr<FlowData> _flowDataIn);
  void loopTimerLoadFromButton();

  void monitoring();

};

#endif // CTMVdoHeatmapModel_HPP
