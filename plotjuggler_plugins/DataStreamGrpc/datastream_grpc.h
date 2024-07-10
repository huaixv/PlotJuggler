#pragma once

#include "perfdata.grpc.pb.h"
#include "PlotJuggler/datastreamer_base.h"

#include <grpcpp/grpcpp.h>

#include <QtPlugin>
#include <thread>

class GrpcDataStreamer : public PJ::DataStreamer, public rocksdb::PerfDataService::Service
{
Q_OBJECT
  Q_PLUGIN_METADATA(IID "facontidavide.PlotJuggler3.DataStreamer")
  Q_INTERFACES(PJ::DataStreamer)

public:
  GrpcDataStreamer();

  virtual bool start(QStringList*) override;

  virtual void shutdown() override;

  virtual bool isRunning() const override
  {
    return _running;
  }

  virtual ~GrpcDataStreamer() override;

  virtual const char* name() const override
  {
    return "GrpcDataStreamer";
  }

  virtual bool isDebugPlugin() override
  {
    return false;
  }

  grpc::Status SendPerfData(grpc::ServerContext* context, const rocksdb::PerfDataRequest* request, rocksdb::PerfDataResponse* response) override;

private:
  bool _running;
  std::unique_ptr<grpc::Server> server_;
};
