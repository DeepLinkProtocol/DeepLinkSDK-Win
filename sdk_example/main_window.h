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

private slots:
    void on_pbtnConnect_clicked();

    void on_pbtnAuth_clicked();

    void on_pbtnGetDeviceInfo_clicked();

    void on_pbtnOpenRemote_clicked();

    void on_pbtnCloseRemote_clicked();

protected slots:
    void onIpcConnected();
    void onIpcDisconnected();
    void onErrorOccurred(QLocalSocket::LocalSocketError socketError);
    void onReadyRead();

private:
    Ui::MainWindow *ui;
    QLocalSocket *ipc;
};
#endif // MAINWINDOW_H
