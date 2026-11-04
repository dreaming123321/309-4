/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *mainTabs;
    QWidget *bookTab;
    QVBoxLayout *bookLayout;
    QHBoxLayout *bookToolbar;
    QLineEdit *searchEdit;
    QPushButton *refreshButton;
    QPushButton *sortButton;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QTableWidget *bookTable;
    QWidget *borrowTab;
    QVBoxLayout *borrowLayout;
    QHBoxLayout *recordToolbar;
    QPushButton *borrowButton;
    QPushButton *returnButton;
    QPushButton *refreshRecordsButton;
    QSpacerItem *recordSpacer;
    QTableWidget *recordTable;
    QWidget *remindTab;
    QVBoxLayout *remindLayout;
    QHBoxLayout *remindToolbar;
    QPushButton *refreshRemindButton;
    QSpacerItem *remindSpacer;
    QTableWidget *remindTable;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1100, 700);
        MainWindow->setMinimumSize(QSize(1000, 650));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        mainTabs = new QTabWidget(centralwidget);
        mainTabs->setObjectName("mainTabs");
        bookTab = new QWidget();
        bookTab->setObjectName("bookTab");
        bookLayout = new QVBoxLayout(bookTab);
        bookLayout->setObjectName("bookLayout");
        bookToolbar = new QHBoxLayout();
        bookToolbar->setObjectName("bookToolbar");
        searchEdit = new QLineEdit(bookTab);
        searchEdit->setObjectName("searchEdit");

        bookToolbar->addWidget(searchEdit);

        refreshButton = new QPushButton(bookTab);
        refreshButton->setObjectName("refreshButton");

        bookToolbar->addWidget(refreshButton);

        sortButton = new QPushButton(bookTab);
        sortButton->setObjectName("sortButton");

        bookToolbar->addWidget(sortButton);

        addButton = new QPushButton(bookTab);
        addButton->setObjectName("addButton");

        bookToolbar->addWidget(addButton);

        deleteButton = new QPushButton(bookTab);
        deleteButton->setObjectName("deleteButton");

        bookToolbar->addWidget(deleteButton);


        bookLayout->addLayout(bookToolbar);

        bookTable = new QTableWidget(bookTab);
        bookTable->setObjectName("bookTable");

        bookLayout->addWidget(bookTable);

        mainTabs->addTab(bookTab, QString());
        borrowTab = new QWidget();
        borrowTab->setObjectName("borrowTab");
        borrowLayout = new QVBoxLayout(borrowTab);
        borrowLayout->setObjectName("borrowLayout");
        recordToolbar = new QHBoxLayout();
        recordToolbar->setObjectName("recordToolbar");
        borrowButton = new QPushButton(borrowTab);
        borrowButton->setObjectName("borrowButton");

        recordToolbar->addWidget(borrowButton);

        returnButton = new QPushButton(borrowTab);
        returnButton->setObjectName("returnButton");

        recordToolbar->addWidget(returnButton);

        refreshRecordsButton = new QPushButton(borrowTab);
        refreshRecordsButton->setObjectName("refreshRecordsButton");

        recordToolbar->addWidget(refreshRecordsButton);

        recordSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        recordToolbar->addItem(recordSpacer);


        borrowLayout->addLayout(recordToolbar);

        recordTable = new QTableWidget(borrowTab);
        recordTable->setObjectName("recordTable");

        borrowLayout->addWidget(recordTable);

        mainTabs->addTab(borrowTab, QString());
        remindTab = new QWidget();
        remindTab->setObjectName("remindTab");
        remindLayout = new QVBoxLayout(remindTab);
        remindLayout->setObjectName("remindLayout");
        remindToolbar = new QHBoxLayout();
        remindToolbar->setObjectName("remindToolbar");
        refreshRemindButton = new QPushButton(remindTab);
        refreshRemindButton->setObjectName("refreshRemindButton");

        remindToolbar->addWidget(refreshRemindButton);

        remindSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        remindToolbar->addItem(remindSpacer);


        remindLayout->addLayout(remindToolbar);

        remindTable = new QTableWidget(remindTab);
        remindTable->setObjectName("remindTable");

        remindLayout->addWidget(remindTable);

        mainTabs->addTab(remindTab, QString());

        verticalLayout->addWidget(mainTabs);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        mainTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\345\233\276\344\271\246\351\246\206\347\256\241\347\220\206\347\263\273\347\273\237", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\350\276\223\345\205\245\344\271\246\345\220\215 / \347\274\226\345\217\267 / \347\261\273\345\210\253\347\255\233\351\200\211", nullptr));
        refreshButton->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        sortButton->setText(QCoreApplication::translate("MainWindow", "\346\214\211\345\200\237\351\230\205\346\254\241\346\225\260\346\216\222\345\272\217", nullptr));
        addButton->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\345\233\276\344\271\246...", nullptr));
        deleteButton->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\351\200\211\344\270\255", nullptr));
        mainTabs->setTabText(mainTabs->indexOf(bookTab), QCoreApplication::translate("MainWindow", "\345\233\276\344\271\246\347\256\241\347\220\206", nullptr));
        borrowButton->setText(QCoreApplication::translate("MainWindow", "\345\200\237\351\230\205\345\233\276\344\271\246...", nullptr));
        returnButton->setText(QCoreApplication::translate("MainWindow", "\345\275\222\350\277\230\351\200\211\344\270\255", nullptr));
        refreshRecordsButton->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260", nullptr));
        mainTabs->setTabText(mainTabs->indexOf(borrowTab), QCoreApplication::translate("MainWindow", "\345\200\237\351\230\205\347\256\241\347\220\206", nullptr));
        refreshRemindButton->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260\346\217\220\351\206\222", nullptr));
        mainTabs->setTabText(mainTabs->indexOf(remindTab), QCoreApplication::translate("MainWindow", "\345\210\260\346\234\237\346\217\220\351\206\222", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
