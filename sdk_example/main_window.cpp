#include "main_window.h"
#include "./ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

#include "../include/deeplink_sdk.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ipc(new QLocalSocket(this))
    , remote_ipc(nullptr)
{
    ui->setupUi(this);
    connect(ipc, &QLocalSocket::connected, this, &MainWindow::onIpcConnected);
    connect(ipc, &QLocalSocket::disconnected, this, &MainWindow::onIpcDisconnected);
    connect(ipc, &QLocalSocket::errorOccurred, this, &MainWindow::onErrorOccurred);
    connect(ipc, &QLocalSocket::readyRead, this, &MainWindow::onReadyRead);
    connect(this, &MainWindow::ipcMessageReceived, this, &MainWindow::onIpcMessageReceived);
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

void MainWindow::writeMessage(const QString &message, QLocalSocket *client)
{
    if (client->state() != QLocalSocket::ConnectedState) {
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
    client->write(block);
    client->flush();
}

void MainWindow::createRemoteIpc(const QString &ipc_name)
{
    remote_ipc = new QLocalSocket(this);
    connect(remote_ipc, &QLocalSocket::connected, this, &MainWindow::onRemoteIpcConnected);
    connect(remote_ipc, &QLocalSocket::disconnected, this, &MainWindow::onRemoteIpcDisconnected);
    connect(remote_ipc, &QLocalSocket::errorOccurred, this, &MainWindow::onRemoteIpcErrorOccurred);
    connect(remote_ipc, &QLocalSocket::readyRead, this, &MainWindow::onRemoteIpcReadyRead);
    showMessage("Begin to connecting with remote video process");
    remote_ipc->connectToServer(ipc_name);
}

void MainWindow::getRemoteControlledPrompt()
{
    // JSON message
    QJsonObject root;
    root["method"] = "getRemoteControlledPrompt";
    QJsonObject data_obj;
    root["data"] = data_obj;
    QString message = QJsonDocument(root).toJson();
    // qDebug() << message;
    writeMessage(message, ipc);
}

void MainWindow::getAppVersion()
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        return;
    }
    // JSON message
    QJsonObject root;
    root["method"] = "getAppVersion";
    QJsonObject data_obj;
    root["data"] = data_obj;
    QString message = QJsonDocument(root).toJson();
    // qDebug() << message;
    writeMessage(message, ipc);
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
        writeMessage(message, ipc);
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
    writeMessage(message, ipc);
}

void MainWindow::on_pbtnOpenRemote_clicked()
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        QMessageBox::information(this, "info", "Please connect ipc first");
        return;
    }
    QLineEdit *editId = findChild<QLineEdit*>("leDeviceId");
    QLineEdit *editPassword = findChild<QLineEdit*>("leDevicePassword");
    QCheckBox *chbFullScreen = findChild<QCheckBox*>("chbFullScreen");
    QCheckBox *chbMicrophone = findChild<QCheckBox*>("chbMicrophone");
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
        if (chbFullScreen)
            data_obj["fullscreen"] = chbFullScreen->isChecked();
        if (chbMicrophone)
            data_obj["microphone"] = chbMicrophone->isChecked();
        root["data"] = data_obj;
        QString message = QJsonDocument(root).toJson();
        // qDebug() << message;
        writeMessage(message, ipc);
    }
}

void MainWindow::on_pbtnCloseRemote_clicked()
{
    if (remote_ipc) {
        // JSON message
        QJsonObject root;
        root["method"] = "quit";
        QJsonObject data_obj;
        root["data"] = data_obj;
        QString message = QJsonDocument(root).toJson();
        // qDebug() << message;
        writeMessage(message, remote_ipc);
        return;
    }
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
        writeMessage(message, ipc);
    }
}

void MainWindow::on_pbtnConnectRemoteIpc_clicked()
{
    QLineEdit *edit = findChild<QLineEdit*>("leRemoteIpcName");
    QString ipc_name = edit->text();
    if (ipc_name.isEmpty()) {
        QMessageBox::information(this, "info", "Ipc name of remote video process can not be empty");
    } else {
        createRemoteIpc(ipc_name);
    }
}

void MainWindow::onIpcConnected()
{
    showMessage("Ipc connected");
    QPushButton *button = findChild<QPushButton*>("pbtnConnect");
    button->setEnabled(false);
    getAppVersion();
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
    while (!in.atEnd()) {
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

        emit ipcMessageReceived(message);
    }
}

void MainWindow::onIpcMessageReceived(const QString &message)
{
    showMessage("ipc received: " + message);

    // parse received message
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError && !doc.isNull()) {
        QJsonObject root_obj = doc.object();
        if (root_obj.contains("method") && root_obj["method"].isString()) {
            QString method = root_obj["method"].toString();
            if (method.compare("openConnect") == 0) {
                if (root_obj.contains("data") && root_obj["data"].isObject()) {
                    QJsonObject data_obj = root_obj["data"].toObject();
                    if (data_obj.contains("remote_ipc") && data_obj["remote_ipc"].isString()) {
                        QString remote_ipc = data_obj["remote_ipc"].toString();
                        // createRemoteIpc(remote_ipc);
                        QLineEdit *edit = findChild<QLineEdit*>("leRemoteIpcName");
                        if (edit) edit->setText(remote_ipc);
                    }
                }
            } else if (method.compare("auth") == 0) {
                if (root_obj.contains("code") && root_obj["code"].isDouble()) {
                    int error_code = root_obj["code"].toInt();
                    if (error_code == 0)
                        getRemoteControlledPrompt();
                }
            } else if (method.compare("getRemoteControlledPrompt") == 0) {
                if (root_obj.contains("data") && root_obj["data"].isObject()) {
                    QJsonObject data_obj = root_obj["data"].toObject();
                    if (data_obj.contains("value") && data_obj["value"].isString()) {
                        QString value = data_obj["value"].toString();
                        QCheckBox *chb = findChild<QCheckBox*>("chbRemoteControlledSwitch");
                        disconnect(chb, &QCheckBox::stateChanged,
                                   this, &MainWindow::onRemoteControlledSwitchStateChanged);
                        // chb->setChecked(value.compare("on") == 0);
                        chb->setCheckState(value.compare("on") == 0 ? Qt::Checked : Qt::Unchecked);
                        connect(chb, &QCheckBox::stateChanged,
                                this, &MainWindow::onRemoteControlledSwitchStateChanged);
                    }
                }
            } else if (method.compare("remoteControlledStarted") == 0) {
                if (root_obj.contains("data") && root_obj["data"].isObject()) {
                    QJsonObject data_obj = root_obj["data"].toObject();
                    QString device;
                    if (data_obj.contains("device") && data_obj["device"].isString())
                        device = data_obj["device"].toString();
                    if (!device.isEmpty()) {
                        QLineEdit *edit = findChild<QLineEdit*>("leRemoteControlledById");
                        if (edit) edit->setText(device);
                    }
                }
            } else if (method.compare("remoteControlledStopped") == 0) {
                QLineEdit *edit = findChild<QLineEdit*>("leRemoteControlledById");
                if (edit) edit->clear();
            }
        } else {
            showMessage("received message has no method field");
        }
    } else {
        showMessage("parse message error: " + error.errorString());
    }
}

void MainWindow::onRemoteIpcConnected()
{
    showMessage("Ipc of remote video process connected");
    QPushButton *button = findChild<QPushButton*>("pbtnConnectRemoteIpc");
    button->setEnabled(false);
}

void MainWindow::onRemoteIpcDisconnected()
{
    showMessage("Ipc of remote video process disconnected");
    QPushButton *button = findChild<QPushButton*>("pbtnConnectRemoteIpc");
    button->setEnabled(true);
    QLocalSocket *client = qobject_cast<QLocalSocket *>(sender());
    client->deleteLater();
    remote_ipc = nullptr;
    QLineEdit *edit = findChild<QLineEdit*>("leRemoteIpcName");
    if (edit) edit->clear();
}

void MainWindow::onRemoteIpcErrorOccurred(QLocalSocket::LocalSocketError socketError)
{
    QLocalSocket *client = qobject_cast<QLocalSocket *>(sender());
    switch(socketError) {
    case QLocalSocket::ServerNotFoundError:
        showMessage("Ipc of remote video process not found");
        break;
    case QLocalSocket::ConnectionRefusedError:
        showMessage("Ipc of remote video process connection refused");
        break;
    case QLocalSocket::PeerClosedError:
        showMessage("Ipc of remote video process peer closed");
        break;
    default:
        showMessage("Ipc of remote video process error: " + client->errorString());
        break;
    }
}

void MainWindow::onRemoteIpcReadyRead()
{
    QLocalSocket *client = qobject_cast<QLocalSocket *>(sender());
    QDataStream in(client);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_5_10);
    while (!in.atEnd()) {
        quint32 block_size = 0;
        if (client->bytesAvailable() < (int)sizeof(quint32)) {
            showMessage("read message length failed in ipc of remote video process");
            return;
        }
        in >> block_size;
        if (client->bytesAvailable() < block_size || in.atEnd()) {
            showMessage("read message body failed in ipc of remote video process");
            return;
        }
        QString message;
        in >> message;
        showMessage("ipc of remote video process received: " + message);
    }
}

void MainWindow::on_pbtnSetConnectOption_clicked()
{
    if (!remote_ipc) return;
    static int click_cout = 0;
    // JSON message
    QJsonObject root;
    root["method"] = "setConnectOption";
    QJsonObject data_obj;
    data_obj["option"] = kOptionVideoBitrate;
    data_obj["value"] = (click_cout % 2 == 0 ? "0" : "40");
    root["data"] = data_obj;
    QString message = QJsonDocument(root).toJson();
    // qDebug() << message;
    writeMessage(message, remote_ipc);
    ++click_cout;
}

void MainWindow::on_pbtnSendData_clicked()
{
    if (!remote_ipc) return;
    if (remote_ipc->state() != QLocalSocket::ConnectedState) return;
    static int int_value = 100;
    static double double_value = 3.1415926f;
    static bool bool_value = true;
    // JSON message
    QJsonObject root;
    root["method"] = "sendData";
    QJsonObject data_obj;
    data_obj["int"] = int_value;
    data_obj["double"] = double_value;
    data_obj["bool"] = bool_value;
    data_obj["string"] = "this is a test string";
    root["data"] = data_obj;
    QString message = QJsonDocument(root).toJson();
    // qDebug() << message;
    writeMessage(message, remote_ipc);
    ++int_value;
    double_value = double_value * 2;
    bool_value = !bool_value;
}

void MainWindow::on_pbtnCloseRemoteControlled_clicked()
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        QMessageBox::information(this, "info", "Please connect ipc first");
        return;
    }
    QLineEdit *editId = findChild<QLineEdit*>("leRemoteControlledById");
    QString id = editId->text();
    if (id.isEmpty()) {
        QMessageBox::information(this, "info", "Device id is empty");
    } else {
        // JSON message
        QJsonObject root;
        root["method"] = "closeRemoteControlled";
        QJsonObject data_obj;
        data_obj["device"] = id;
        root["data"] = data_obj;
        QString message = QJsonDocument(root).toJson();
        // qDebug() << message;
        writeMessage(message, ipc);
    }
}

void MainWindow::onRemoteControlledSwitchStateChanged(int arg1)
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        QMessageBox::information(this, "info", "Please connect ipc first");
        return;
    }
    QCheckBox *chb = findChild<QCheckBox*>("chbRemoteControlledSwitch");
    if (!chb) return;
    // JSON message
    QJsonObject root;
    root["method"] = "setRemoteControlledPrompt";
    QJsonObject data_obj;
    data_obj["value"] = chb->isChecked() ? "on" : "off";
    root["data"] = data_obj;
    QString message = QJsonDocument(root).toJson();
    // qDebug() << message;
    writeMessage(message, ipc);
}

void MainWindow::on_pbtnSendData2_clicked()
{
    if (ipc->state() != QLocalSocket::ConnectedState) {
        QMessageBox::information(this, "info", "Please connect ipc first");
        return;
    }
    QLineEdit *edit = findChild<QLineEdit*>("leRemoteControlledById");
    if (!edit) return;
    QString to_device = edit->text();
    if (to_device.isEmpty()) return;
    static int int_value = 100;
    static double double_value = 3.1415926f;
    static bool bool_value = true;
    // JSON message
    QJsonObject root;
    root["method"] = "sendData";
    root["to_device"] = to_device;
    QJsonObject data_obj;
    data_obj["int"] = int_value;
    data_obj["double"] = double_value;
    data_obj["bool"] = bool_value;
    data_obj["string"] = "this is a test string";
    root["data"] = data_obj;
    QString message = QJsonDocument(root).toJson();
    // qDebug() << message;
    writeMessage(message, ipc);
    ++int_value;
    double_value = double_value * 2;
    bool_value = !bool_value;
}
