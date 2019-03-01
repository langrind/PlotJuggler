#ifndef ULOG_PARAMETERS_DIALOG_H
#define ULOG_PARAMETERS_DIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class ULogParametersDialog;
}

class ULogParametersDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ULogParametersDialog(
            const std::vector<std::pair<QString, QString>>& info,
            const std::vector<std::pair<QString, QString>>& params,
            QWidget *parent = nullptr);

  void restoreSettings();

  ~ULogParametersDialog();

private:
  Ui::ULogParametersDialog *ui;
};

#endif // ULOG_PARAMETERS_DIALOG_H
