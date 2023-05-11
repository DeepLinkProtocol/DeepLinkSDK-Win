#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>

class Bridge : public QObject
{
    Q_OBJECT
public:
    explicit Bridge(QObject *parent = nullptr);

signals:
    // QString CallByCPP(const QString &input1, const QString &input2);
    void sig_showMinimized();
    void sig_closeWindow();

public slots:
    // call by JS
    QString say(const QString &message);
    QString getAppVersion();
    QString getSystemLanguages();
    int showMessageBox(const QString &message);
    // minimize window
    int showMinimized(void);
    // close window
    int closeWindow(void);
    // common command
    QString execute(const QString &command);

};

#endif // BRIDGE_H
