#ifndef SENSORDIALOG_H
#define SENSORDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class SensorDialog : public QDialog
{
    Q_OBJECT

public:
    SensorDialog(QWidget * = nullptr);

    QLineEdit *idEdit;
    QComboBox *typeEdit;
    QLineEdit *nameEdit;
    QLineEdit *precisionEdit;
};

#endif // SENSORDIALOG_H
