#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLocalSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showMessage(const QString &message);
    void writeMessage(const QString &message);
    void createRemoteIpc(const QString &remote_ipc);

private slots:
    void on_pbtnConnect_clicked();

    void on_pbtnAuth_clicked();

    void on_pbtnGetDeviceInfo_clicked();

    void on_pbtnOpenRemote_clicked();

    void on_pbtnCloseRemote_clicked();

    void on_pbtnConnectRemoteIpc_clicked();

protected slots:
    // ipc with deeplink service program
    void onIpcConnected();
    void onIpcDisconnected();
    void onErrorOccurred(QLocalSocket::LocalSocketError socketError);
    void onReadyRead();
    // ipc with remote video process
    void onRemoteIpcConnected();
    void onRemoteIpcDisconnected();
    void onRemoteIpcErrorOccurred(QLocalSocket::LocalSocketError socketError);
    void onRemoteIpcReadyRead();

private:
    Ui::MainWindow *ui;
    QLocalSocket *ipc;
};
#endif // MAINWINDOW_H
