#ifndef DEVTOOLSWINDOW_H
#define DEVTOOLSWINDOW_H

#include <QMainWindow>

class QWebEngineProfile;
class WebView;

class DevToolsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DevToolsWindow(QWebEngineProfile *profile, QWidget *parent = nullptr);
    QSize sizeHint() const override;

    WebView *webView() const;

signals:

private slots:
    void handleWebViewTitleChanged(const QString &title);

private:
    WebView *view_;

};

#endif // DEVTOOLSWINDOW_H
