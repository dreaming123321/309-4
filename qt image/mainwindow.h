#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 与 book.txt 的 7 个字段对应：
// 编号 书名 类别 价格 总数 可借 借阅次数
struct Book
{
    QString id;
    QString name;
    QString category;
    double price = 0.0;
    int total = 0;
    int available = 0;
    int borrowed = 0;
};

// 与 borrow.txt 的 9 个字段对应：
// 借阅编号 书名 学号 姓名 借书日期 应还日期 未还数量 天数 状态
struct BorrowRecord
{
    QString id;
    QString bookName;
    QString bookId; // 旧记录可能没有图书编号
    QString studentId;
    QString personName;
    QString borrowDate;
    QString dueDate;
    int count = 0;
    int days = 0;
    QString status;
};

class QStackedWidget;
class QLabel;
class QDialog;
class QPushButton;
class QPropertyAnimation;
class QCloseEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void closeEvent(QCloseEvent *event) override;

    // 分层界面；数据处理仍使用原来的功能函数。
    void setupShell();
    void switchPage(QWidget *page);
    void showHome();
    void showWorkspace(int tab, const QString &title = QString());
    void refreshDashboard();
    int execForm(QDialog &dialog);
    void exportBooks();

    QStackedWidget *m_pages = nullptr;
    QWidget *m_homePage = nullptr;
    QWidget *m_workspacePage = nullptr;
    QWidget *m_formPage = nullptr;
    QWidget *m_formHost = nullptr;
    QWidget *m_navigation = nullptr;
    QLabel *m_pageTitle = nullptr;
    QLabel *m_pageDescription = nullptr;
    QLabel *m_bookStat = nullptr;
    QLabel *m_availableStat = nullptr;
    QLabel *m_dueStat = nullptr;
    QLabel *m_dataStatus = nullptr;
    QPushButton *m_backButton = nullptr;
    QDialog *m_activeForm = nullptr;
    QPropertyAnimation *m_pageAnimation = nullptr;
    bool m_motionEnabled = true;

    // 数据层
    bool loadBooks(QVector<Book> &books, QString *error = nullptr) const;
    bool saveBooks(const QVector<Book> &books, QString *error = nullptr) const;
    bool loadRecords(QVector<BorrowRecord> &records, QString *error = nullptr) const;
    bool saveRecords(const QVector<BorrowRecord> &records, QString *error = nullptr) const;
    bool appendRecord(const BorrowRecord &record, QString *error = nullptr) const;

    QString dataPath(const QString &fileName) const;
    static QString campusName(const QString &id);

    // 界面
    void setupTables();
    void refreshBooks();
    void refreshRecords();
    void refreshRemind();
    void filterBooks(const QString &keyword);

    // 操作
    void addBook();
    void deleteBook();
    void sortBooks();
    void borrowBook();
    void returnBook();

    Ui::MainWindow *ui;
    QString m_dataDir;
    bool m_sorted = false;
};

#endif // MAINWINDOW_H
