#include <functions/load_sensors.h>
#include <view/mainwindow.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    loadSensors();
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
