#pragma once

#include <QtPlugin>
#include <thread>
#include "PlotJuggler/datastreamer_base.h"

class DataStreamGrpc : public PJ::DataStreamer
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "facontidavide.PlotJuggler3.DataStreamer")
  Q_INTERFACES(PJ::DataStreamer)

public:
  DataStreamGrpc();

  virtual bool start(QStringList*) override;

  virtual void shutdown() override;

  virtual bool isRunning() const override;

  virtual ~DataStreamGrpc() override;

  virtual const char* name() const override
  {
    return "Grpc Streamer";
  }

  virtual bool isDebugPlugin() override
  {
    return false;
  }

  virtual bool xmlSaveState(QDomDocument& doc,
                            QDomElement& parent_element) const override;

  virtual bool xmlLoadState(const QDomElement& parent_element) override;

  std::pair<QAction*, int> notificationAction() override
  {
    return { _dummy_notification, _notifications_count };
  }

private:
  struct Parameters
  {
    double A, B, C, D;
  };

  void loop();

  std::thread _thread;

  bool _running;

  std::map<std::string, Parameters> _parameters;

  void pushSingleCycle();

  QAction* _dummy_notification;

  int _notifications_count;
};
