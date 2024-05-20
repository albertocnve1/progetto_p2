#include "mainwindow.h"
#include "load_sensors.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    loadSensors();
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
