#ifndef __HEATMAP2D_H__
#define __HEATMAP2D_H__

#include <QList>
#include <QMap>
#include <QPair>
#include <QString>

#include "./ui_heatmap2d.h"
#include "abstract_plot.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Heatmap2d;
}  // namespace Ui
QT_END_NAMESPACE

class Heatmap2d : public AbstractPlot {
 private:
  QMap<QString, QCPColorGradient> gradient_style = {
      {"Grayscale", QCPColorGradient::gpGrayscale},
      {"Hot", QCPColorGradient::gpHot},
      {"Cold", QCPColorGradient::gpCold},
      {"Night", QCPColorGradient::gpNight},
      {"Candy", QCPColorGradient::gpCandy},
      {"Geography", QCPColorGradient::gpGeography},
      {"Ion", QCPColorGradient::gpIon},
      {"Thermal", QCPColorGradient::gpThermal},
      {"Polar", QCPColorGradient::gpPolar},
      {"Spectrum", QCPColorGradient::gpSpectrum},
      {"Jet", QCPColorGradient::gpJet},
      {"Hues", QCPColorGradient::gpHues}};

  Ui::Heatmap2d* ui;
  QVector<double> default_distribution_vector;
  QCPColorMap* color_map;

 public:
  Heatmap2d(int graph_num = 1, QWidget* parent = nullptr);
  ~Heatmap2d() { delete ui; }

  // return name's index in global Manager
  // -1 if 'None' and -2 if not found
  int get_name_index(QString name);

 public slots:
  virtual void redraw_settings(int row, int column);

  virtual void update_data(const QModelIndex& topLeft,
                           const QModelIndex& bottomRight,
                           const QList<int>& roles = QList<int>());

  virtual void update_var_names(const QModelIndex& topLeft,
                                const QModelIndex& bottomRight,
                                const QList<int>& roles = QList<int>());
};

#endif
