#ifndef __HEATMAP2D_SETTINGS_MODEL_H__
#define __HEATMAP2D_SETTINGS_MODEL_H__

#include <QTableWidget>

class Heatmap2dSettingsModel : public QTableWidget {
 public:
  enum Column {
    Axis_X,         // Column for axis x values
    Axis_Y,         // Column for axis y values
    Distribution,   // Column for setting distribution
    Gradient,       // Column for gradient type
    Interpolate     // Column for switching interpolating on/off
  };
  static QList<QString> heading;

  Heatmap2dSettingsModel(QWidget* parent);
};

#endif