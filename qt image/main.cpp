#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("LibraryGui"));
    app.setApplicationDisplayName(QStringLiteral("图书馆管理系统"));

    // 让 QMessageBox / QDialogButtonBox 的标准按钮显示中文
    QTranslator translator;
    const QString localTranslations =
        QCoreApplication::applicationDirPath() + QStringLiteral("/translations");
    if (!translator.load(QStringLiteral("qt_zh_CN"), localTranslations)) {
        (void)translator.load(QStringLiteral("qt_zh_CN"),
                              QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    }
    app.installTranslator(&translator);

    MainWindow w;
    w.show();

    return app.exec();
}
