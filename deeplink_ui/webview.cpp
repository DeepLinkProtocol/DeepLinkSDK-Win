#include "webview.h"

#include <QMessageBox>
#include <QTimer>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent)
{
    setMouseTracking(true);
    connect(this, &QWebEngineView::renderProcessTerminated,
            [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
        QString status;
        switch (termStatus) {
        case QWebEnginePage::NormalTerminationStatus:
            status = tr("Render process normal exit");
            break;
        case QWebEnginePage::AbnormalTerminationStatus:
            status = tr("Render process abnormal exit");
            break;
        case QWebEnginePage::CrashedTerminationStatus:
            status = tr("Render process crashed");
            break;
        case QWebEnginePage::KilledTerminationStatus:
            status = tr("Render process killed");
            break;
        }
        QMessageBox::StandardButton btn = QMessageBox::question(window(), status,
                                                   tr("Render process exited with code: %1\n"
                                                      "Do you want to reload the page ?").arg(statusCode));
        if (btn == QMessageBox::Yes)
            QTimer::singleShot(0, [this] { reload(); });
    });
}

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    // TODO:
}

QWebEngineView *WebView::createWindow(QWebEnginePage::WebWindowType type)
{
//    BrowserWindow *mainWindow = qobject_cast<BrowserWindow*>(window());
//    if (!mainWindow)
//        return nullptr;

//    switch (type) {
//    case QWebEnginePage::WebBrowserTab: {
//        return mainWindow->tabWidget()->createTab();
//    }
//    case QWebEnginePage::WebBrowserBackgroundTab: {
//        return mainWindow->tabWidget()->createBackgroundTab();
//    }
//    case QWebEnginePage::WebBrowserWindow: {
//        return mainWindow->browser()->createWindow()->currentTab();
//    }
//    case QWebEnginePage::WebDialog: {
//        WebPopupWindow *popup = new WebPopupWindow(page()->profile());
//        connect(popup->view(), &WebView::devToolsRequested, this, &WebView::devToolsRequested);
//        return popup->view();
//    }
//    }
    return nullptr;
}
