#include "bridge.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QMessageBox>
#include <QUrl>
//#include <QVariant>

Bridge::Bridge(QObject *parent)
    : QObject{parent}
{

}

QString Bridge::say(const QString &message)
{
    // return QVariant(message);
    return "C++ received: " + message;
}

QString Bridge::getAppVersion()
{
    return QCoreApplication::applicationVersion();
}

QString Bridge::getSystemLanguages()
{
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    return uiLanguages.join(",");
}

int Bridge::showMessageBox(const QString &message)
{
    QMessageBox::information(nullptr, "JS Info", message);
    return 0;
}

int Bridge::showMinimized(void)
{
    emit sig_showMinimized();
    return 0;
}

int Bridge::closeWindow(void)
{
    emit sig_closeWindow();
    return 0;
}

QString Bridge::execute(const QString &command)
{
    QJsonObject result;
    result["code"] = 0;
    result["message"] = "ok";
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(command.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError && !doc.isNull()) {
        QJsonObject root_obj = doc.object();
        if (root_obj.contains("version") && root_obj["version"].isDouble()) {
            int version = root_obj["version"].toInt();
            switch (version) {
            case 1:
                result["code"] = 0;
                break;
            default:
                result["code"] = 1;
                result["message"] = "Unknown version";
                break;
            }
        } else {
            result["code"] = 1;
            result["message"] = "No version field";
        }
    } else {
        result["code"] = 1;
        result["message"] = error.errorString();
    }
    return QJsonDocument(result).toJson();
}
