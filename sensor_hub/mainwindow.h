#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QWidget>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void filterSensors(const QString &text);

private:
    QHBoxLayout *layout;
    QListWidget listWidget;
    QLabel detailsLabel;
    QLineEdit searchBox;
};

#endif // MAINWINDOW_H
