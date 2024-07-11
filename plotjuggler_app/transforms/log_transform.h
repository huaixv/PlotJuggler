#ifndef LOG_TRANSFORM_H
#define LOG_TRANSFORM_H

#include "PlotJuggler/transform_function.h"

using namespace PJ;

namespace Ui
{
class LogTransform;
}

class LogTransform : public TransformFunction_SISO
{
public:
  explicit LogTransform();

  ~LogTransform() override;

  static const char* transformName()
  {
    return "Log Transform";
  }

  const char* name() const override
  {
    return transformName();
  }

  QWidget* optionsWidget() override;

  bool xmlSaveState(QDomDocument& doc, QDomElement& parent_element) const override;

  bool xmlLoadState(const QDomElement& parent_element) override;

private:
  Ui::LogTransform* ui;
  QWidget* _widget;

  double base_ = 2.0;

  std::optional<PlotData::Point> calculateNextPoint(size_t index) override;
};

#endif  // LOG_TRANSFORM_H
