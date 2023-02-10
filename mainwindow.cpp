#include <QMessageBox>
#include <cstdio>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Windows.h>
#include <QFileDialog>
#include <QLineEdit>
#include <QSettings>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork>
#include <QString>
#include "prc.h"
#include <iostream>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <direct.h>
#include <QtGui>
#include <QtCore>
#include <QTextStream>





bool connect_b = true;



QString getOpenVPNPath()
{
    HKEY hKey;
    LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\OpenVPN"), 0, KEY_READ, &hKey);
    if (lResult != ERROR_SUCCESS) {
        return "-1";
    }

    DWORD dwType = REG_SZ;
    DWORD dwSize = MAX_PATH;
    TCHAR szValue[MAX_PATH];
    lResult = RegQueryValueEx(hKey, TEXT("exe_path"), NULL, &dwType, (LPBYTE)&szValue, &dwSize);
    RegCloseKey(hKey);

    if (lResult != ERROR_SUCCESS) {
        return "-1";
    }

    return (QString::fromWCharArray(szValue)).replace("openvpn.exe","openvpn-gui.exe");
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Cloaky");



    // Load the settings
    QSettings settings("Cloaky", "CloakyGUI");
    host_ip = settings.value("host_ip", "").toString();
    file_path = settings.value("file_path", "").toString();
    protocol = settings.value("protocol", "").toString();
    ovpn_path = settings.value("vpn", "").toString();


    ui->host->setText(host_ip);
    ui->path->setText(file_path);
    ui->vpn_file_path->setText(ovpn_path);


    ui->comboBox->setCurrentText(protocol);



    QString check_vpn_engine = getOpenVPNPath();
    if (check_vpn_engine == "-1") {

        QMessageBox::information(this, "OpenVPN Not Detected", "OpenVPN has not been detected on the system. First you need to install it. The installer will be prompted.");
//QProcess::startDetached("C:\\Users\\mega\\Documents\\build-GUICloak-Desktop_Qt_6_4_0_MinGW_64_bit-Debug\\debug\\ovpnapp.msi");

        QString fileName = "ovpnapp.msi";
        QUrl fileUrl = QUrl::fromLocalFile(fileName);
        if (!QDesktopServices::openUrl(fileUrl))
        {
            qWarning("Could not open the file.");
            return ;
        }

//        QMessageBox msgBox;
//        msgBox.setTextFormat(Qt::RichText);
//        msgBox.setText("OpenVPN could not be detected on your system, the cloaky needs it to function. You can download and install it from here:<br><br><a href='https://openvpn.net/community-downloads/'>OpenVPN Downloads</a>");

//        msgBox.exec();




    } else {
        std::cout << "OpenVPN is installed at: " << check_vpn_engine.toStdString() << std::endl;
    }



    connect(ui->enc_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_enc_comboBox_currentIndexChanged);





    if (!ui->path->text().isEmpty()) {

    QFile file(ui->path->text());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open file."));
        return;
    }

    QTextStream in(&file);
    QString data = in.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    QString encryption = jsonObj["EncryptionMethod"].toString();

    int index;

    if (encryption == "plain") {
        index = 1;
    } else if (encryption == "aes-128-gcm") {
        index = 2;
    } else if (encryption == "aes-256-gcm" || encryption == "aes-gcm") {
        index = 3;
    } else if (encryption == "chacha20-poly1305") {
        index = 4;
    } else {
        index = 3;
    }


    on_enc_comboBox_currentIndexChanged(index);
    ui->enc_comboBox->setCurrentIndex(index);
}


}

MainWindow::~MainWindow()
{
    delete ui;
}


QString getOpenVPNCFG()
{
    HKEY hKey;
    LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\OpenVPN"), 0, KEY_READ, &hKey);
    if (lResult != ERROR_SUCCESS) {
        return "-1";
    }

    DWORD dwType = REG_SZ;
    DWORD dwSize = MAX_PATH;
    TCHAR szValue[MAX_PATH];
    lResult = RegQueryValueEx(hKey, TEXT("config_dir"), NULL, &dwType, (LPBYTE)&szValue, &dwSize);
    RegCloseKey(hKey);

    if (lResult != ERROR_SUCCESS) {
        return "-1";
    }

    return (QString::fromWCharArray(szValue));
}


void MainWindow::on_connect_clicked() {

if (connect_b == true){



//    Gateway g;
//    QString gw = g.getDefaultGateway();
    host_ip = ui->host->text();
    file_path = ui->path->text();

    if(host_ip.isEmpty()) {
        QMessageBox::critical(this, "Error", "Please enter the host address");
        return;
    }

    if(file_path.isEmpty()) {
        QMessageBox::critical(this, "Error", "No config file has loaded");
        return;
    }

    QString ovpn_path   = ui->vpn_file_path->text();

    if(ovpn_path.isEmpty()) {
        QMessageBox::critical(this, "Error", "No OpenVPN config file has loaded");
        return;
    }


    QString protocol   = ui->comboBox->currentText();
    QString prt;
    if(protocol == "UDP") {
        prt = "-u";

    } else {
        prt = "";
    }




    QString combined = "ck-client.exe "+prt+" -s " + host_ip + " -c "+ file_path;

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(NULL, combined.toStdWString().data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        QMessageBox::critical(this, "Error", "Could not connect to the system shell");
        return;
    }

    // Save the settings
    QSettings settings("Cloaky", "CloakyGUI");
    settings.setValue("host_ip", host_ip);
    settings.setValue("file_path", file_path);
    settings.setValue("protocol", protocol);
    settings.setValue("vpn", ovpn_path);



    ui->connect->setText("Disconnect");


    //connecting to OpenVPN GUI

    QFileInfo fileInfo(ui->vpn_file_path->text());
    QString profile = fileInfo.completeBaseName();

//    std::cout << "profile is " << profile.toStdString() << std::endl;


    QString path = getOpenVPNPath();
    if (path == "-1") {
        QMessageBox::critical(this, "Error", "Openvpn is not installed, Cloakly uses the commiunity version of OpenVPN. Please install it from here https://openvpn.net/community-downloads/");
        return;

    } else {
        std::cout << "OpenVPN is installed at: " << path.toStdString() << std::endl;
    }


    QString ovpn_handler =  path+" --connect "+"\"" + profile + "\"";



    STARTUPINFO si3;
    memset(&si3, 0, sizeof(si3));
    si3.cb = sizeof(si3);
    si3.dwFlags = STARTF_USESHOWWINDOW;
    si3.wShowWindow = SW_HIDE;
    si3.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si3.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si3.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si3.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi3;
    memset(&pi3, 0, sizeof(pi3));

    if (!CreateProcess(NULL, ovpn_handler.toStdWString().data(), NULL, NULL, TRUE, 0, NULL, NULL, &si3, &pi3)) {
        QMessageBox::critical(this, "Error", "Could not connect to the system shell");
        return;
    }





//    WaitForSingleObject(pi.hProcess, INFINITE);

//    char buffer[1024];
//    std::string result = "";
//    DWORD bytesRead;
//    while (ReadFile(si.hStdOutput, buffer, 1024, &bytesRead, NULL) && bytesRead != 0) {
//        result += std::string(buffer, buffer + bytesRead);
//    }
//    ui->sresult->setPlainText(QString::fromStdString(result));

//    CloseHandle(pi.hProcess);
//    CloseHandle(pi.hThread);
} else if (connect_b == false){
    ui->connect->setText("Connect");
    on_disconnect_clicked();
    QThread::sleep(1);  // Sleep for 1 second (1000 milliseconds)

    }

connect_b = !connect_b;

PRC prc;
auto processList = prc.getProcessList();

QString target = "ck-client.exe";
bool found = false;
for (int i = 0; i < processList.size(); ++i) {
    if (processList.at(i) == target) {
        found = true;
        break;
    }
}

if (found) {
    ui->stat->setText("Tunnel Activated");

    } else {
    ui->stat->setText("Tunnel Deactivated");
    }


}

void MainWindow::on_disconnect_clicked()
{

    QString kill_task = "taskkill /IM ck-client.exe /F";

    STARTUPINFO si2;
    memset(&si2, 0, sizeof(si2));
    si2.cb = sizeof(si2);
    si2.dwFlags = STARTF_USESHOWWINDOW;
    si2.wShowWindow = SW_HIDE;
    si2.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si2.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si2.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si2.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(NULL, kill_task.toStdWString().data(), NULL, NULL, TRUE, 0, NULL, NULL, &si2, &pi)) {
        QMessageBox::critical(this, "Error", "Could not connect to the system shell");
        return;
    }



    //disconnecting to OpenVPN GUI

//    QFileInfo fileInfo(ui->vpn_file_path->text());
//    QString profile = fileInfo.completeBaseName();

    QString path = getOpenVPNPath();
    if (path == "-1") {
        QMessageBox::critical(this, "Error", "Openvpn is not installed, Cloakly uses the commiunity version of OpenVPN. Please install it from here https://openvpn.net/community-downloads/");
        return;

    } else {
        std::cout << "OpenVPN is installed at: " << path.toStdString() << std::endl;
    }



    QString ovpn_handler =  path+" --command disconnect_all";// "+profile;

    STARTUPINFO si4;
    memset(&si4, 0, sizeof(si4));
    si4.cb = sizeof(si4);
    si4.dwFlags = STARTF_USESHOWWINDOW;
    si4.wShowWindow = SW_HIDE;
    si4.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si4.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si4.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si4.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi4;
    memset(&pi4, 0, sizeof(pi4));

    if (!CreateProcess(NULL, ovpn_handler.toStdWString().data(), NULL, NULL, TRUE, 0, NULL, NULL, &si4, &pi4)) {
        QMessageBox::critical(this, "Error", "Could not connect to the system shell");
        return;
    }





//    ui->stat->setText("Process killed.");

  // WaitForSingleObject(pi.hProcess, INFINITE);

//    char buffer[1024];
//    std::string result = "";
//    DWORD bytesRead;
//    while (ReadFile(si2.hStdOutput, buffer, 1024, &bytesRead, NULL) && bytesRead != 0) {
//        result += std::string(buffer, buffer + bytesRead);
//    }
//    ui->sresult->setPlainText(QString::fromStdString(result));

//    CloseHandle(pi.hProcess);
//    CloseHandle(pi.hThread);

}



void MainWindow::on_browse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open the Clock client config file"), QDir::homePath(), tr("Files (*.*)"));
    if (!fileName.isEmpty()) {
        ui->path->setText(fileName);


    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not open file."));
        return;
    }

    QTextStream in(&file);
    QString data = in.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    QString serverName = jsonObj["ServerName"].toString();
    QString encryption = jsonObj["EncryptionMethod"].toString();

    int index;

    if (encryption == "plain") {
        index = 1;
    } else if (encryption == "aes-128-gcm") {
        index = 2;
    } else if (encryption == "aes-256-gcm" || encryption == "aes-gcm") {
        index = 3;
    } else if (encryption == "chacha20-poly1305") {
        index = 4;
    } else {
        index = 3;
    }


    on_enc_comboBox_currentIndexChanged(index);
    ui->enc_comboBox->setCurrentIndex(index);

    ui->host->setText(serverName);

    }

}

void MainWindow::closeEvent(QCloseEvent *event) {
    on_disconnect_clicked();
    event->accept();


    QString path = getOpenVPNPath();
    if (path == "-1") {
        QMessageBox::critical(this, "Error", "Openvpn is not installed, Cloakly uses the commiunity version of OpenVPN. Please install it from here https://openvpn.net/community-downloads/");
        return;

    } else {
        std::cout << "OpenVPN is installed at: " << path.toStdString() << std::endl;
    }


    QString combined = path+" --command exit";

//    std::cout<<combined.toStdString()<<std::endl;

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(NULL, combined.toStdWString().data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        QMessageBox::critical(this, "Error", "Could not connect to the system shell");
        return;
    }

}


void MainWindow::on_browse_vpn_clicked()
{
    QString vpnFile = QFileDialog::getOpenFileName(this, tr("Open VPN Configuration"), "", tr("OpenVPN Files (*.ovpn)"));
    if (!vpnFile.isEmpty()) {

    ui->vpn_file_path->setText(vpnFile);

     QString userHomeFolder = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
     QString config = userHomeFolder + "/OpenVPN/config/";



     QFileInfo fileInfo(vpnFile);
     QString base = fileInfo.completeBaseName();

     QString dest = QDir(config).filePath(base);
     dest = QDir(dest).filePath(fileInfo.fileName());

     QFileInfo fileInfo2(dest);
     QString v = fileInfo2.path();

//     int status = _mkdir(v.toLocal8Bit().constData());
//     if (status != 0) {
//         qDebug() << "Failed to create folder: " << strerror(errno);
//     }


//     QDir().mkpath(v); // Creates any missing folders in the destination path
     QDir().mkpath(v);

//     QString dst;
//     dst = QDir(QDir::currentPath()).filePath("cnfg");
//     dst = QDir(dst).filePath(fileInfo.fileName());
     QFile file(dest);
     if (file.exists()) {
       file.remove();
     }

     if (!QFile::copy(vpnFile, dest )) {
         qDebug() << "File copy failed!";
     }





    QString path = getOpenVPNPath();
    if (path == "-1") {
        QMessageBox::critical(this, "Error", "Openvpn is not installed, Cloakly uses the commiunity version of OpenVPN. Please install it from here https://openvpn.net/community-downloads/");
        return;

    } else {
        std::cout << "OpenVPN is installed at: " << path.toStdString() << std::endl;
    }


    QString combined = path+" --config_dir "+config;

//    std::cout<<combined.toStdString()<<std::endl;

    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    if (!CreateProcess(NULL, combined.toStdWString().data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        QMessageBox::critical(this, "Error", "Could not connect to the system shell");
        return;
    }


    }
}


//void MainWindow::on_actionAbout_triggered()
//{
//    QMessageBox aboutBox;
//    aboutBox.setText("By High resolution Mango, based on cloak open source project");
//    aboutBox.setWindowTitle("About");
//    aboutBox.setStandardButtons(QMessageBox::Close);
//    aboutBox.setDefaultButton(QMessageBox::Close);
//    aboutBox.exec();
//}

void MainWindow::on_enc_comboBox_currentIndexChanged(int index)
{

    QString j_name = ui->path->text();

    if (!j_name.isEmpty() && index > 0) {


    QString encryptionMethod;
    if (index == 1) {
        encryptionMethod = "plain";
    } else if (index == 2) {
        encryptionMethod = "aes-128-gcm";
    } else if (index == 3) {
        encryptionMethod = "aes-256-gcm";
    } else if (index == 4) {
        encryptionMethod = "chacha20-poly1305";
    }

    // Read the existing JSON file
    QFile file(j_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QTextStream in(&file);
    QString jsonString = in.readAll();
    file.close();

    // Parse the JSON content
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();

    // Update the "EncryptionMethod" value
    jsonObj["EncryptionMethod"] = encryptionMethod;

    // Write the updated JSON content back to the file
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    out << QJsonDocument(jsonObj).toJson(QJsonDocument::Indented);
    file.close();

    }
}




