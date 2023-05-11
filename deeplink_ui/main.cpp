#include "main_window.h"

#include <QApplication>
#include <QCommandLineParser>

#define DEFAULT_WEBPAGE_URL "http://192.168.1.104:8080/"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("DeepLink");
    QApplication::setApplicationVersion("1.0.0.1");

    QString url = DEFAULT_WEBPAGE_URL;
    bool devTools = false;

    QCommandLineParser parser;
    parser.setApplicationDescription("DeepLink Command Helper");
    // parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption url_option("url",
        "The url of the front-end page of DeepLink [default: " DEFAULT_WEBPAGE_URL "].", "url");
    const QCommandLineOption dev_option("dev", "Developer mode.");
    parser.addOption(url_option);
    parser.addOption(dev_option);
    // Process the actual command line arguments given by the user
    parser.process(app);
    if (parser.isSet(url_option)) url = parser.value(url_option);
    if (parser.isSet(dev_option)) devTools = true;

    int width = 1160, height = 765;
    if (devTools) height += 36;

    MainWindow w(devTools);
    w.setUrl(url);
    w.resize(width, height);
    w.show();
    return app.exec();
}
