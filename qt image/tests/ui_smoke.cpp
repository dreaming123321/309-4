// 在隔离的数据目录运行：检查页面导航、取消、动效开关，并保存截图。
#include "mainwindow.h"
#include <QApplication>
#include <QAbstractButton>
#include <QCheckBox>
#include <QDialog>
#include <QDir>
#include <QFontDatabase>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTimer>
#include <QLineEdit>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QFile>
#include <cstdio>

int main(int argc, char **argv)
{
    QApplication app(argc,argv);
    // offscreen 平台不枚举 Windows 字体；显式加载字体用于截图验证。
    QFontDatabase::addApplicationFont(QStringLiteral("C:/Windows/Fonts/msyh.ttc"));
    QFontDatabase::addApplicationFont(QStringLiteral("C:/Windows/Fonts/msyhbd.ttc"));
    QFontDatabase::addApplicationFont(QStringLiteral("C:/Windows/Fonts/segoeui.ttf"));
    MainWindow window;
    window.show();
    const QString output=qEnvironmentVariable("LIBRARY_UI_SHOTS");
    QDir().mkpath(output);
    auto check=[&app](bool ok,const char *message) { if(!ok) { fprintf(stderr,"FAIL: %s\n",message); app.exit(1); } return ok; };
    auto click=[&window,&check](const char *name) { auto *button=window.findChild<QAbstractButton *>(QString::fromLatin1(name)); if(check(button && button->isVisible() && button->isEnabled(),name)) button->click(); };
    auto shot=[&window,output,&check](const char *name) { check(window.grab().save(output+QStringLiteral("/")+QString::fromLatin1(name)),"save screenshot"); };
    auto page=[&window]() { return window.findChild<QStackedWidget *>(QStringLiteral("pageStack"))->currentIndex(); };
    if (argc > 1 && QString::fromLocal8Bit(argv[1]) == QStringLiteral("--submit")) {
        auto fill=[&window,&check](int quantity) {
            auto *host=window.findChild<QWidget *>(QStringLiteral("formHost"));
            auto *form=host->findChild<QFormLayout *>();
            if(!check(form && form->rowCount()==5,"add fields")) return;
            qobject_cast<QLineEdit *>(form->itemAt(1,QFormLayout::FieldRole)->widget())->setText(QStringLiteral("UI_SMOKE_BOOK"));
            qobject_cast<QLineEdit *>(form->itemAt(2,QFormLayout::FieldRole)->widget())->setText(QStringLiteral("Testing"));
            qobject_cast<QDoubleSpinBox *>(form->itemAt(3,QFormLayout::FieldRole)->widget())->setValue(25);
            qobject_cast<QSpinBox *>(form->itemAt(4,QFormLayout::FieldRole)->widget())->setValue(quantity);
            host->findChild<QDialogButtonBox *>()->button(QDialogButtonBox::Ok)->click();
        };
        auto closeMessage=[] { for(auto *w:QApplication::topLevelWidgets()) if(auto *box=qobject_cast<QMessageBox *>(w)) box->accept(); };
        QTimer::singleShot(300,&window,[&] { click("homeAction1"); });
        QTimer::singleShot(650,&window,[&] { fill(3); });
        QTimer::singleShot(1000,&window,closeMessage);
        QTimer::singleShot(1400,&window,[&] { check(page()==0,"submit returns home"); click("homeAction1"); });
        QTimer::singleShot(1800,&window,[&] { fill(99); });
        QTimer::singleShot(2200,&window,closeMessage);
        QTimer::singleShot(2600,&window,[&] {
            QFile file(qEnvironmentVariable("LIBRARY_DATA_DIR")+QStringLiteral("/book.txt"));
            if(!check(file.open(QIODevice::ReadOnly),"read saved book")) return;
            int matches=0;
            for(const QByteArray &line:file.readAll().split('\n')) {
                if(line.contains(" UI_SMOKE_BOOK ")) { matches++; if(!check(line.trimmed().endsWith(" 4 4 0"),"new total 3, duplicate adds exactly 1")) return; }
            }
            if(!check(matches==1 && page()==0,"single catalog record after submit")) return;
            fprintf(stdout,"PASS: embedded add submits 3 copies, duplicate adds exactly 1 copy\n"); app.exit(0);
        });
        QTimer::singleShot(15000,&app,[&] { fprintf(stderr,"FAIL: submit timeout\n"); app.exit(2); });
        return app.exec();
    }
    QTimer::singleShot(800,&window,[&] { check(page()==0,"home page"); shot("01-home.png"); });
    QTimer::singleShot(1000,&window,[&] { click("homeAction1"); });
    QTimer::singleShot(1400,&window,[&] { check(page()==2,"add form page"); shot("02-add-book.png"); click("pageBack"); });
    QTimer::singleShot(1700,&window,[&] { check(page()==0,"back from add"); click("homeAction3"); });
    QTimer::singleShot(2100,&window,[&] { check(page()==1,"catalog page"); shot("03-catalog.png"); click("pageBack"); });
    QTimer::singleShot(2400,&window,[&] { click("homeAction6"); });
    QTimer::singleShot(2800,&window,[&] { check(page()==2,"borrow form page"); shot("04-borrow.png"); click("pageBack"); });
    QTimer::singleShot(3100,&window,[&] { click("homeAction7"); });
    QTimer::singleShot(3400,&window,[&] { auto *table=window.findChild<QTableWidget *>(QStringLiteral("recordTable")); if(check(table && table->rowCount()>0,"return records")) { table->selectRow(0); click("returnButton"); } });
    QTimer::singleShot(3800,&window,[&] { check(page()==2,"return form page"); shot("05-return.png"); click("pageBack"); });
    QTimer::singleShot(4100,&window,[&] { click("pageBack"); click("homeAction5"); });
    QTimer::singleShot(4500,&window,[&] { check(page()==2,"export form page"); shot("06-export.png"); click("pageBack"); });
    QTimer::singleShot(4800,&window,[&] { auto *toggle=window.findChild<QCheckBox *>(QStringLiteral("motionToggle")); toggle->setChecked(false); window.resize(1000,720); click("homeAction8"); });
    QTimer::singleShot(5200,&window,[&] { check(page()==1,"reminders page"); shot("07-reminders-small.png"); click("pageBack"); });
    QTimer::singleShot(5500,&window,[&] { shot("08-home-small.png"); fprintf(stdout,"PASS: home, add, catalog, borrow, return, export, reminders, back, reduced motion\n"); app.exit(0); });
    QTimer::singleShot(15000,&app,[&] { fprintf(stderr,"FAIL: UI timeout\n"); app.exit(2); });
    return app.exec();
}
