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
    void deleteSensor();  // Dichiarazione dello slot per eliminare un sensore
    void showContextMenu(const QPoint &);  // Dichiarazione dello slot per mostrare il menu contestuale
    void editSensor();  // Dichiarazione dello slot per modificare un sensore
    void exportSensor(); // Dichiarazione dello slot per esportare un sensore

private:
    QHBoxLayout *layout;   // Layout principale orizzontale
    QLineEdit searchBox;   // Casella di ricerca
    QListWidget listWidget;  // Lista di sensori
    QLabel detailsLabel;   // Etichetta per i dettagli del sensore
    QPushButton *addButton; // Pulsante per aggiungere un nuovo sensore
    QPushButton *removeButton; // Pulsante per rimuovere un sensore
    
};

#endif // MAINWINDOW_H
