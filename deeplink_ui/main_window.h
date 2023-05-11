#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Bridge;
class WebView;
class QLineEdit;
class QProgressBar;
class DevToolsWindow;
class QWebEngineProfile;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(bool for_dev_tools = false);
    ~MainWindow();

    void setUrl(const QString &url);

private slots:
    void webViewLoadStarted();
    void webViewLoadFinished(bool ok);
    void webViewUrlChanged(const QUrl &url);
    void webViewLoadProgress(int progress);
    void pbtnDevToolsClicked();
    void pbtnCallJsFuncClicked();

    // function called by JS
    void bridgeShowMinisized();
    void bridgeCloseWindow();

private:
    Bridge *bridge_;
    WebView *view_;
    QWebEngineProfile *profile_;
    QLineEdit *url_edit_;
    QProgressBar *progress_bar_;
    DevToolsWindow *dev_tools_;

};
#endif // MAINWINDOW_H
