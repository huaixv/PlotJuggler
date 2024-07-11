#include "log_transform.h"
#include "ui_log_transform.h"
#include <QFormLayout>
#include <QDoubleValidator>

LogTransform::LogTransform() : ui(new Ui::LogTransform), _widget(new QWidget())
{
  ui->setupUi(_widget);

  connect(ui->doubleSpinBoxLogBase, &QDoubleSpinBox::editingFinished, this, [=]() {
    base_ = ui->doubleSpinBoxLogBase->value();
    emit parametersChanged();
  });
}

LogTransform::~LogTransform()
{
  delete ui;
  delete _widget;
}

QWidget* LogTransform::optionsWidget()
{
  return _widget;
}

bool LogTransform::xmlSaveState(QDomDocument& doc, QDomElement& parent_element) const
{
  QDomElement widget_el = doc.createElement("options");
  widget_el.setAttribute("base", ui->doubleSpinBoxLogBase->value());
  parent_element.appendChild(widget_el);
  return true;
}

bool LogTransform::xmlLoadState(const QDomElement& parent_element)
{
  QDomElement widget_el = parent_element.firstChildElement("options");
  if (widget_el.isNull())
  {
    return false;
  }
  base_ = widget_el.attribute("base", "2.0").toDouble();
  ui->doubleSpinBoxLogBase->setValue(base_);
  return true;
}

std::optional<PlotData::Point> LogTransform::calculateNextPoint(size_t index)
{
  const auto& p = dataSource()->at(index);
  PlotData::Point out = { p.x, std::log(p.y) / std::log(base_) };
  return out;
}
