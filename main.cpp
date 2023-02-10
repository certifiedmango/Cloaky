#include "mainwindow.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;


    w.show();



    QSettings settings("Cloaky", "CloakyGUI");
    settings.setValue("host_ip", w.getHostIp());
    settings.setValue("file_path", w.getFilePath());
    settings.setValue("protocol", w.getProtocol());
    settings.setValue("vpn", w.getVpn());


    return a.exec();
}
