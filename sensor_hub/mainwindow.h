#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include "sensor.h"


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void filterSensors(const QString &text);
    void addSensor();  // Dichiarazione dello slot per aggiungere un nuovo sensore
    void newSensor();  // Dichiarazione dello slot per creare un nuovo sensore
    void createSensorFile(sensor*);  // Dichiarazione dello slot per creare un sensore da un file

private:
    QHBoxLayout *layout;   // Layout principale orizzontale
    QLineEdit searchBox;   // Casella di ricerca
    QListWidget listWidget;  // Lista di sensori
    QLabel detailsLabel;   // Etichetta per i dettagli del sensore
    QPushButton *addButton; // Pulsante per aggiungere un nuovo sensore
};

#endif // MAINWINDOW_H
