#include <QMainWindow>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString getHostIp() const { return host_ip; }
    void setHostIp(const QString &value) { host_ip = value; }

    QString getFilePath() const { return file_path; }
    void setFilePath(const QString &value) { file_path = value; }

    QString getProtocol() const { return protocol; }
    void setProtocol(const QString &value) { protocol = value; }

    QString getVpn() const { return ovpn_path; }
    void setVpn(const QString &value) { ovpn_path = value; }


private slots:
    void on_connect_clicked();
    void on_browse_clicked();
    void on_disconnect_clicked();


    void on_browse_vpn_clicked();




    void on_enc_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QString host_ip;
    QString file_path;
    QString protocol;
    QString ovpn_path;



protected:
    void closeEvent(QCloseEvent *event) override;
};
