#include "datastream_grpc.h"
#include "ui_datastream_grpc.h"

#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QDialog>
#include <QWebSocket>
#include <QIntValidator>
#include <QMessageBox>
#include <QNetworkDatagram>
#include <QNetworkInterface>

#include <mutex>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using rocksdb::Metric;
using rocksdb::PerfDataRequest;
using rocksdb::PerfDataResponse;
using rocksdb::PerfDataService;
using rocksdb::PerfResponseStatus;

class GrpcDataStreamerDialog : public QDialog
{
public:
  GrpcDataStreamerDialog() : QDialog(nullptr), ui(new Ui::GrpcDataStreamerDialog)
  {
    ui->setupUi(this);
    setWindowTitle("Grpc Data Streamer");

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  }
  ~GrpcDataStreamerDialog()
  {
    delete ui;
  }
  Ui::GrpcDataStreamerDialog* ui;
};

GrpcDataStreamer::GrpcDataStreamer() : _running(false)
{
}

GrpcDataStreamer::~GrpcDataStreamer()
{
  shutdown();
}

bool GrpcDataStreamer::start(QStringList*)
{
  if (_running)
  {
    return _running;
  }

  if (parserFactories() == nullptr || parserFactories()->empty())
  {
    QMessageBox::warning(nullptr, tr("Grpc Data Streamer"),
                         tr("No available MessageParsers"), QMessageBox::Ok);
    _running = false;
    return false;
  }

  bool ok = false;

  GrpcDataStreamerDialog dialog;

  // load previous values
  QSettings settings;
  QString address_str =
      settings.value("GrpcDataStreamer::address", "0.0.0.0:50051").toString();

  dialog.ui->lineEditAddress->setText(address_str);

  int res = dialog.exec();
  if (res == QDialog::Rejected)
  {
    _running = false;
    return false;
  }

  address_str = dialog.ui->lineEditAddress->text();

  // save back to service
  settings.setValue("GrpcDataStreamer::address", address_str);

  QHostAddress address(address_str);

  bool success = true;
  success &= !address.isNull();

  // start grpc service here
  ServerBuilder builder;
  builder.AddListeningPort(address_str.toStdString(), grpc::InsecureServerCredentials());
  builder.RegisterService(this);

  server_ = builder.BuildAndStart();

  _running = true;

  return _running;
}

void GrpcDataStreamer::shutdown()
{
  if (_running)
  {
    // shutdown grpc service
    server_.get()->Shutdown();
    _running = false;
  }
}

grpc::Status GrpcDataStreamer::SendPerfData(grpc::ServerContext* context,
                                            const rocksdb::PerfDataRequest* request,
                                            rocksdb::PerfDataResponse* response)
{
  qDebug() << "Received performance data at timestamp: " << request->timestamp() << "\n";
  try
  {
    for (const Metric& metric : request->metrics())
    {
      auto& series = dataMap().getOrCreateNumeric(metric.key());
      series.pushBack({ request->timestamp(), metric.value() });

      qDebug() << metric.key().c_str() << ": " << metric.value() << "\n";
    }

    emit dataReceived();
    response->set_status(PerfResponseStatus::SUCCESS);
    return Status::OK;
  }
  catch (std::exception& err)
  {
    shutdown();
    // notify the GUI
    emit closed();
    response->set_status(PerfResponseStatus::FAILURE);
    return Status::CANCELLED;
  }
}
