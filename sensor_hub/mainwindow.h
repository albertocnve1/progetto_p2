#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPushButton>  

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void filterSensors(const QString &text);
    void addSensor();  // Dichiarazione dello slot per aggiungere un nuovo sensore

private:
    QHBoxLayout *layout;   // Layout principale orizzontale
    QLineEdit searchBox;   // Casella di ricerca
    QListWidget listWidget;  // Lista di sensori
    QLabel detailsLabel;   // Etichetta per i dettagli del sensore
    QPushButton *addButton; // Pulsante per aggiungere un nuovo sensore
};

#endif // MAINWINDOW_H
