#include "sensordialog.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

SensorDialog::SensorDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("ID:"));
    idEdit = new QLineEdit(this);
    layout->addWidget(idEdit);

    layout->addWidget(new QLabel("Tipo:"));
    typeEdit = new QComboBox(this);
    typeEdit->addItem("Humidity Sensor");
    typeEdit->addItem("Dust Sensor");
    typeEdit->addItem("Temperature Sensor");
    layout->addWidget(typeEdit);

    layout->addWidget(new QLabel("Nome:"));
    nameEdit = new QLineEdit(this);
    layout->addWidget(nameEdit);

    layout->addWidget(new QLabel("Precisione:"));
    precisionEdit = new QLineEdit(this);
    layout->addWidget(precisionEdit);

    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Annulla", this);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
}
