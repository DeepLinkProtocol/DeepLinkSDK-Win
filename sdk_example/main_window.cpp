#include "main_window.h"
#include "./ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ipc(new QLocalSocket(this))
{
    ui->setupUi(this);
    connect(ipc, &QLocalSocket::connected, this, &MainWindow::onIpcConnected);
    connect(ipc, &QLocalSocket::disconnected, this, &MainWindow::onIpcDisconnected);
    connect(ipc, &QLocalSocket::errorOccurred, this, &MainWindow::onErrorOccurred);
    connect(ipc, &QLocalSocket::readyRead, this, &MainWindow::onReadyRead);
}

MainWindow::~MainWindow()
{
    ipc->abort();
    ipc->close();
    delete ui;
}

void MainWindow::showMessage(const QString &message)
{
    QTextEdit *edit = findChild<QTextEdit*>("teMessage");
    edit->append(message);
}

void MainWindow::writeMessage(const QString &message)
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        showMessage("write failed because of ipc is not connected");
        return;
    }
    // Serialize the message and send
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_5_10);
    out << quint32(message.size());
    out << message;
    // qDebug() << "ipc send:" << block;
    ipc->write(block);
    ipc->flush();
}

void MainWindow::on_pbtnConnect_clicked()
{
    QLineEdit *edit = findChild<QLineEdit*>("leIpcName");
    QString ipc_name = edit->text();
    if (ipc_name.isEmpty()) {
        QMessageBox::information(this, "info", "Ipc name can not be empty");
    } else {
        showMessage("Connecting to ipc " + ipc_name);
        ipc->connectToServer(ipc_name);
    }
}

void MainWindow::on_pbtnAuth_clicked()
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        QMessageBox::information(this, "info", "Please connect ipc first");
        return;
    }
    QLineEdit *editId = findChild<QLineEdit*>("leAuthId");
    QLineEdit *editToken = findChild<QLineEdit*>("leAuthToken");
    QString id = editId->text();
    QString token = editToken->text();
    if (id.isEmpty() || token.isEmpty()) {
        QMessageBox::information(this, "info", "Auth id or token is empty");
    } else {
        // JSON message
        QJsonObject root;
        root["method"] = "auth";
        QJsonObject data_obj;
        data_obj["id"] = id;
        data_obj["token"] = token;
        root["data"] = data_obj;
        QString message = QJsonDocument(root).toJson();
        // qDebug() << message;
        writeMessage(message);
    }
}

void MainWindow::on_pbtnGetDeviceInfo_clicked()
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        QMessageBox::information(this, "info", "Please connect ipc first");
        return;
    }
    // JSON message
    QJsonObject root;
    root["method"] = "getDeviceInfo";
    QString message = QJsonDocument(root).toJson();
    // qDebug() << message;
    writeMessage(message);
}

void MainWindow::on_pbtnOpenRemote_clicked()
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        QMessageBox::information(this, "info", "Please connect ipc first");
        return;
    }
    QLineEdit *editId = findChild<QLineEdit*>("leDeviceId");
    QLineEdit *editPassword = findChild<QLineEdit*>("leDevicePassword");
    QString id = editId->text();
    QString password = editPassword->text();
    if (id.isEmpty() || password.isEmpty()) {
        QMessageBox::information(this, "info", "Device id or password is empty");
    } else {
        // JSON message
        QJsonObject root;
        root["method"] = "openConnect";
        QJsonObject data_obj;
        data_obj["device"] = id;
        data_obj["password"] = password;
        root["data"] = data_obj;
        QString message = QJsonDocument(root).toJson();
        // qDebug() << message;
        writeMessage(message);
    }
}

void MainWindow::on_pbtnCloseRemote_clicked()
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        QMessageBox::information(this, "info", "Please connect ipc first");
        return;
    }
    QLineEdit *editId = findChild<QLineEdit*>("leDeviceId");
    QString id = editId->text();
    if (id.isEmpty()) {
        QMessageBox::information(this, "info", "Device id is empty");
    } else {
        // JSON message
        QJsonObject root;
        root["method"] = "closeConnect";
        QJsonObject data_obj;
        data_obj["device"] = id;
        root["data"] = data_obj;
        QString message = QJsonDocument(root).toJson();
        // qDebug() << message;
        writeMessage(message);
    }
}

void MainWindow::onIpcConnected()
{
    showMessage("Ipc connected");
    QPushButton *button = findChild<QPushButton*>("pbtnConnect");
    button->setEnabled(false);
}

void MainWindow::onIpcDisconnected()
{
    showMessage("Ipc disconnected");
    QPushButton *button = findChild<QPushButton*>("pbtnConnect");
    button->setEnabled(true);
}

void MainWindow::onErrorOccurred(QLocalSocket::LocalSocketError socketError)
{
    switch(socketError) {
    case QLocalSocket::ServerNotFoundError:
        showMessage("Ipc server not found");
        break;
    case QLocalSocket::ConnectionRefusedError:
        showMessage("Ipc connection refused");
        break;
    case QLocalSocket::PeerClosedError:
        showMessage("Ipc peer closed");
        break;
    default:
        showMessage(ipc->errorString());
        break;
    }
}

void MainWindow::onReadyRead()
{
    QDataStream in(ipc);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_10);
    quint32 block_size = 0;
    if (ipc->bytesAvailable() < (int)sizeof(quint32)) {
        showMessage("read message length failed");
        return;
    }
    in >> block_size;
    if (ipc->bytesAvailable() < block_size || in.atEnd()) {
        showMessage("read message body failed");
        return;
    }
    QString message;
    in >> message;
    showMessage("ipc received: " + message);
}
