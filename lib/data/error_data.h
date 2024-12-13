#ifndef __ERRORS_TO_VARIABLE_H__
#define __ERRORS_TO_VARIABLE_H__

class ErrorData {
 public:
  double data{};
  ErrorData(double data) : data{fabs(data)} {}
  virtual double getError(double measurement) = 0;
  virtual operator QString() const = 0;
};

class ErrorAbsolute : public ErrorData {
 public:
  using ErrorData::ErrorData;
  double getError(double measurement) override { return data; }
  virtual operator QString() const { return QString::number(data); }
};

class ErrorRelative : public ErrorData {
 public:
  using ErrorData::ErrorData;
  virtual double getError(double measurement) {
    return fabs(measurement * data);
  }
  virtual operator QString() const { return QString::number(data * 100) + "%"; }
};

#endif