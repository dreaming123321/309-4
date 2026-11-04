#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDate>
#include <QDir>
#include <QCoreApplication>
#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSet>
#include <QSpinBox>
#include <QStatusBar>
#include <QStringConverter>
#include <QTableWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <algorithm>
#include <limits>

namespace {

QTableWidgetItem *makeItem(const QString &text, bool center = false)
{
    auto *it = new QTableWidgetItem(text);
    if (center)
        it->setTextAlignment(Qt::AlignCenter);
    return it;
}

void useChineseButtons(QDialogButtonBox *buttons)
{
    if (auto *ok = buttons->button(QDialogButtonBox::Ok))
        ok->setText(QStringLiteral("确定"));
    if (auto *cancel = buttons->button(QDialogButtonBox::Cancel))
        cancel->setText(QStringLiteral("取消"));
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QStringLiteral("图书馆管理系统"));
    resize(1100, 700);

    // 数据目录：默认使用 309-4，可用环境变量 LIBRARY_DATA_DIR 覆盖
    m_dataDir = qEnvironmentVariable(
        "LIBRARY_DATA_DIR",
        QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("../../309-4")));

    setupTables();
    setupShell();
    ui->searchEdit->setPlaceholderText(QStringLiteral("按书名、编号、类别或校区搜索"));

    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::addBook);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::deleteBook);
    connect(ui->sortButton, &QPushButton::clicked, this, &MainWindow::sortBooks);
    connect(ui->borrowButton, &QPushButton::clicked, this, &MainWindow::borrowBook);
    connect(ui->returnButton, &QPushButton::clicked, this, &MainWindow::returnBook);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &MainWindow::filterBooks);

    connect(ui->refreshButton, &QPushButton::clicked, this, [this] {
        m_sorted = false;
        refreshBooks();
        refreshRecords();
        refreshRemind();
    });
    connect(ui->refreshRecordsButton, &QPushButton::clicked, this, [this] {
        refreshRecords();
        refreshRemind();
    });
    connect(ui->refreshRemindButton, &QPushButton::clicked,
            this, &MainWindow::refreshRemind);

    refreshBooks();
    refreshRecords();
    refreshRemind();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTables()
{
    ui->bookTable->setColumnCount(8);
    ui->bookTable->setHorizontalHeaderLabels({
        QStringLiteral("编号"),
        QStringLiteral("书名"),
        QStringLiteral("类别"),
        QStringLiteral("价格"),
        QStringLiteral("总数"),
        QStringLiteral("可借"),
        QStringLiteral("借阅次数"),
        QStringLiteral("校区")
    });

    ui->recordTable->setColumnCount(8);
    ui->recordTable->setHorizontalHeaderLabels({
        QStringLiteral("借阅编号"),
        QStringLiteral("书名"),
        QStringLiteral("学号"),
        QStringLiteral("姓名"),
        QStringLiteral("借书日期"),
        QStringLiteral("应还日期"),
        QStringLiteral("未还数量"),
        QStringLiteral("状态")
    });

    ui->remindTable->setColumnCount(7);
    ui->remindTable->setHorizontalHeaderLabels({
        QStringLiteral("借阅编号"),
        QStringLiteral("书名"),
        QStringLiteral("学号"),
        QStringLiteral("姓名"),
        QStringLiteral("应还日期"),
        QStringLiteral("剩余天数"),
        QStringLiteral("状态")
    });

    for (QTableWidget *table : {ui->bookTable, ui->recordTable, ui->remindTable}) {
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setAlternatingRowColors(true);
    }
}

QString MainWindow::dataPath(const QString &fileName) const
{
    return m_dataDir + QLatin1Char('/') + fileName;
}

QString MainWindow::campusName(const QString &id)
{
    if (id.startsWith(QStringLiteral("XL")))
        return QStringLiteral("仙林");
    if (id.startsWith(QStringLiteral("PL")))
        return QStringLiteral("三牌楼");
    return QStringLiteral("未知");
}

bool MainWindow::loadBooks(QVector<Book> &books, QString *error) const
{
    books.clear();

    QFile file(dataPath(QStringLiteral("book.txt")));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error)
            *error = QStringLiteral("打不开 %1").arg(file.fileName());
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    QSet<QString> ids;
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        const QStringList parts = line.split(
            QRegularExpression(QStringLiteral("\\s+")),
            Qt::SkipEmptyParts);

        if (parts.size() != 7) {
            if (error) *error = QStringLiteral("图书记录必须为 7 个字段，读取已停止。");
            return false;
        }

        Book b;
        b.id        = parts[0];
        b.name      = parts[1];
        b.category  = parts[2];
        bool priceOk, totalOk, availableOk, borrowedOk;
        b.price     = parts[3].toDouble(&priceOk);
        b.total     = parts[4].toInt(&totalOk);
        b.available = parts[5].toInt(&availableOk);
        b.borrowed  = parts[6].toInt(&borrowedOk);
        if (!priceOk || !totalOk || !availableOk || !borrowedOk
            || !(b.price >= 0 && b.price <= 500) || b.total < 1 || b.total > 200
            || b.available < 0 || b.available > b.total || b.borrowed < 0
            || b.id.toUtf8().size() > 19 || b.name.toUtf8().size() > 99
            || b.category.toUtf8().size() > 49 || ids.contains(b.id) || books.size() >= 300) {
            if (error) *error = QStringLiteral("图书编号重复、字段过长或数量格式异常，读取已停止。");
            return false;
        }
        ids.insert(b.id);
        books.append(b);
    }
    return true;
}

bool MainWindow::saveBooks(const QVector<Book> &books, QString *error) const
{
    QSaveFile file(dataPath(QStringLiteral("book.txt")));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (error)
            *error = QStringLiteral("写不了 %1").arg(file.fileName());
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    for (const Book &b : books) {
        out << b.id << ' ' << b.name << ' ' << b.category << ' '
            << QString::number(b.price, 'f', 2) << ' '
            << b.total << ' ' << b.available << ' ' << b.borrowed << '\n';
    }

    if (!file.commit()) {
        if (error)
            *error = file.errorString();
        return false;
    }
    return true;
}

bool MainWindow::loadRecords(QVector<BorrowRecord> &records, QString *error) const
{
    records.clear();

    QFile file(dataPath(QStringLiteral("borrow.txt")));
    if (!file.exists())
        return true;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (error)
            *error = QStringLiteral("打不开 %1").arg(file.fileName());
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        const QStringList parts = line.split(
            QRegularExpression(QStringLiteral("\\s+")),
            Qt::SkipEmptyParts);

        if (parts.size() != 9 && parts.size() != 10) {
            if (error) *error = QStringLiteral("借阅记录格式异常，读取已停止。");
            return false;
        }

        BorrowRecord r;
        r.id         = parts[0];
        r.bookName   = parts[1];
        r.studentId  = parts[2];
        r.personName = parts[3];
        r.borrowDate = parts[4];
        r.dueDate    = parts[5];
        bool countOk;
        r.count      = parts[6].toInt(&countOk);
        if (!countOk || r.count < 0) {
            if (error) *error = QStringLiteral("借阅数量格式异常，读取已停止。");
            return false;
        }
        r.days       = parts[7].toInt();
        r.status     = parts[8];
        if (parts.size() >= 10) r.bookId = parts[9];
        records.append(r);
    }
    return true;
}

bool MainWindow::saveRecords(const QVector<BorrowRecord> &records, QString *error) const
{
    QSaveFile file(dataPath(QStringLiteral("borrow.txt")));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (error)
            *error = QStringLiteral("写不了 %1").arg(file.fileName());
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    for (const BorrowRecord &r : records) {
        out << r.id << ' ' << r.bookName << ' ' << r.studentId << ' '
            << r.personName << ' ' << r.borrowDate << ' ' << r.dueDate << ' '
            << r.count << ' ' << r.days << ' ' << r.status;
        if (!r.bookId.isEmpty()) out << ' ' << r.bookId;
        out << '\n';
    }

    if (!file.commit()) {
        if (error)
            *error = file.errorString();
        return false;
    }
    return true;
}

bool MainWindow::appendRecord(const BorrowRecord &r, QString *error) const
{
    QFile file(dataPath(QStringLiteral("borrow.txt")));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        if (error)
            *error = QStringLiteral("写不了 %1").arg(file.fileName());
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << r.id << ' ' << r.bookName << ' ' << r.studentId << ' '
        << r.personName << ' ' << r.borrowDate << ' ' << r.dueDate << ' '
        << r.count << ' ' << r.days << ' ' << r.status;
    if (!r.bookId.isEmpty()) out << ' ' << r.bookId;
    out << '\n';
    return true;
}

void MainWindow::refreshBooks()
{
    QVector<Book> books;
    QString error;
    if (!loadBooks(books, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }

    if (m_sorted) {
        std::sort(books.begin(), books.end(),
                  [](const Book &a, const Book &b) {
                      if (a.borrowed != b.borrowed)
                          return a.borrowed > b.borrowed;
                      return a.name < b.name;
                  });
    }

    ui->bookTable->setRowCount(static_cast<int>(books.size()));
    int availableSum = 0;

    for (int row = 0; row < books.size(); ++row) {
        const Book &b = books.at(row);
        ui->bookTable->setItem(row, 0, makeItem(b.id));
        ui->bookTable->setItem(row, 1, makeItem(b.name));
        ui->bookTable->setItem(row, 2, makeItem(b.category));
        ui->bookTable->setItem(row, 3,
            makeItem(QString::number(b.price, 'f', 2), true));
        ui->bookTable->setItem(row, 4,
            makeItem(QString::number(b.total), true));
        ui->bookTable->setItem(row, 5,
            makeItem(QString::number(b.available), true));
        ui->bookTable->setItem(row, 6,
            makeItem(QString::number(b.borrowed), true));
        ui->bookTable->setItem(row, 7,
            makeItem(campusName(b.id), true));

        availableSum += b.available;
    }

    filterBooks(ui->searchEdit->text());

    statusBar()->showMessage(
        QStringLiteral("共 %1 种图书，可借 %2 本")
            .arg(static_cast<int>(books.size()))
            .arg(availableSum));
}

void MainWindow::refreshRecords()
{
    QVector<BorrowRecord> records;
    QString error;
    if (!loadRecords(records, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }

    ui->recordTable->setRowCount(static_cast<int>(records.size()));
    for (int row = 0; row < records.size(); ++row) {
        const BorrowRecord &r = records.at(row);
        ui->recordTable->setItem(row, 0, makeItem(r.id));
        ui->recordTable->setItem(row, 1, makeItem(r.bookName));
        ui->recordTable->setItem(row, 2, makeItem(r.studentId));
        ui->recordTable->setItem(row, 3, makeItem(r.personName));
        ui->recordTable->setItem(row, 4, makeItem(r.borrowDate, true));
        ui->recordTable->setItem(row, 5, makeItem(r.dueDate, true));
        ui->recordTable->setItem(row, 6,
            makeItem(QString::number(r.count), true));
        ui->recordTable->setItem(row, 7, makeItem(r.status, true));
    }
}

void MainWindow::refreshRemind()
{
    QVector<BorrowRecord> records;
    QString error;
    if (!loadRecords(records, &error))
        return;

    const QDate today = QDate::currentDate();
    QVector<BorrowRecord> soon;

    for (const BorrowRecord &r : records) {
        if (r.status == QStringLiteral("已归还") || r.count <= 0)
            continue;

        const QDate due = QDate::fromString(
            r.dueDate, QStringLiteral("yyyy-MM-dd"));
        if (!due.isValid())
            continue;

        if (today.daysTo(due) <= 7)
            soon.append(r);
    }

    std::sort(soon.begin(), soon.end(),
              [](const BorrowRecord &a, const BorrowRecord &b) {
                  return a.dueDate < b.dueDate;
              });

    ui->remindTable->setRowCount(static_cast<int>(soon.size()));
    for (int row = 0; row < soon.size(); ++row) {
        const BorrowRecord &r = soon.at(row);
        const QDate due = QDate::fromString(
            r.dueDate, QStringLiteral("yyyy-MM-dd"));
        const int left = static_cast<int>(today.daysTo(due));

        QString leftText;
        if (left < 0)
            leftText = QStringLiteral("已逾期 %1 天").arg(-left);
        else if (left == 0)
            leftText = QStringLiteral("今天到期");
        else
            leftText = QStringLiteral("还剩 %1 天").arg(left);

        ui->remindTable->setItem(row, 0, makeItem(r.id));
        ui->remindTable->setItem(row, 1, makeItem(r.bookName));
        ui->remindTable->setItem(row, 2, makeItem(r.studentId));
        ui->remindTable->setItem(row, 3, makeItem(r.personName));
        ui->remindTable->setItem(row, 4, makeItem(r.dueDate, true));
        ui->remindTable->setItem(row, 5, makeItem(leftText, true));
        ui->remindTable->setItem(row, 6, makeItem(r.status, true));
    }
}

void MainWindow::filterBooks(const QString &keyword)
{
    const QString key = keyword.trimmed();

    for (int row = 0; row < ui->bookTable->rowCount(); ++row) {
        bool match = key.isEmpty();

        for (int col : {0, 1, 2, 7}) {
            const QTableWidgetItem *it = ui->bookTable->item(row, col);
            if (it && it->text().contains(key, Qt::CaseInsensitive))
                match = true;
        }

        ui->bookTable->setRowHidden(row, !match);
    }
}

void MainWindow::addBook()
{
    QVector<Book> books;
    QString error;
    if (!loadBooks(books, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("添加图书"));
    dlg.setMinimumWidth(400);

    auto *campusCombo = new QComboBox(&dlg);
    campusCombo->addItem(QStringLiteral("仙林校区"), QStringLiteral("XL"));
    campusCombo->addItem(QStringLiteral("三牌楼校区"), QStringLiteral("PL"));

    auto *nameEdit = new QLineEdit(&dlg);
    auto *categoryEdit = new QLineEdit(&dlg);

    auto *priceSpin = new QDoubleSpinBox(&dlg);
    priceSpin->setRange(0.0, 500.0);
    priceSpin->setDecimals(2);
    priceSpin->setSingleStep(1.0);

    auto *totalSpin = new QSpinBox(&dlg);
    totalSpin->setRange(1, 200);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("校区："), campusCombo);
    form->addRow(QStringLiteral("书名："), nameEdit);
    form->addRow(QStringLiteral("类别："), categoryEdit);
    form->addRow(QStringLiteral("价格："), priceSpin);
    form->addRow(QStringLiteral("总数："), totalSpin);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    useChineseButtons(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    auto *layout = new QVBoxLayout(&dlg);
    layout->addLayout(form);
    layout->addWidget(buttons);

    if (execForm(dlg) != QDialog::Accepted)
        return;

    const QString prefix = campusCombo->currentData().toString();
    const QString name = nameEdit->text().trimmed();
    const QString category = categoryEdit->text().trimmed();

    if (name.isEmpty() || category.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("输入有误"),
                             QStringLiteral("书名和类别不能为空。"));
        return;
    }
    if (name.contains(QRegularExpression(QStringLiteral("\\s")))
        || category.contains(QRegularExpression(QStringLiteral("\\s")))) {
        QMessageBox::warning(this, QStringLiteral("输入有误"),
                             QStringLiteral("书名和类别不能包含空格。"));
        return;
    }

    if (name.toUtf8().size() > 99 || category.toUtf8().size() > 49) {
        QMessageBox::warning(this, QStringLiteral("输入有误"),
                             QStringLiteral("书名或类别过长，不能保存到现有数据格式。"));
        return;
    }

    // 同校区同名：总数和可借各 +1（与原 C 程序一致）
    for (Book &b : books) {
        if (b.name == name && b.id.startsWith(prefix)) {
            if (b.total >= 200) {
                QMessageBox::warning(this, QStringLiteral("添加失败"),
                                     QStringLiteral("该书总数量已达到 200 本。"));
                return;
            }
            b.total += 1;
            b.available += 1;

            if (!saveBooks(books, &error)) {
                QMessageBox::warning(this, QStringLiteral("保存失败"), error);
                return;
            }
            refreshBooks();
            QMessageBox::information(
                this, QStringLiteral("添加成功"),
                QStringLiteral("该书已存在，总数和可借数量已各 +1。"));
            return;
        }
    }

    if (books.size() >= 300) {
        QMessageBox::warning(this, QStringLiteral("添加失败"),
                             QStringLiteral("图书记录已达到 300 条上限。"));
        return;
    }

    int maxNo = 0;
    const QString head = prefix + QStringLiteral("-B");
    for (const Book &b : books) {
        if (b.id.startsWith(head))
            maxNo = qMax(maxNo, b.id.mid(head.size()).toInt());
    }

    QVector<BorrowRecord> records;
    if (!loadRecords(records, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }
    for (const BorrowRecord &r : records) {
        if (r.bookId.startsWith(head))
            maxNo = qMax(maxNo, r.bookId.mid(head.size()).toInt());
    }
    if (maxNo == std::numeric_limits<int>::max()) {
        QMessageBox::warning(this, QStringLiteral("添加失败"), QStringLiteral("图书编号已用尽。"));
        return;
    }

    Book b;
    b.id = QStringLiteral("%1-B%2")
               .arg(prefix)
               .arg(maxNo + 1, 3, 10, QChar('0'));
    b.name = name;
    b.category = category;
    b.price = priceSpin->value();
    b.total = totalSpin->value();
    b.available = b.total;
    b.borrowed = 0;
    books.append(b);

    if (!saveBooks(books, &error)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), error);
        return;
    }

    refreshBooks();
    QMessageBox::information(
        this, QStringLiteral("添加成功"),
        QStringLiteral("新书编号：%1").arg(b.id));
}

void MainWindow::deleteBook()
{
    const int row = ui->bookTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("请先在表格里选中一本书。"));
        return;
    }

    const QString id = ui->bookTable->item(row, 0)->text();
    const QString name = ui->bookTable->item(row, 1)->text();

    const auto answer = QMessageBox::question(
        this, QStringLiteral("确认删除"),
        QStringLiteral("确定要删除《%1》吗？").arg(name));
    if (answer != QMessageBox::Yes)
        return;

    QVector<Book> books;
    QString error;
    if (!loadBooks(books, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }

    int index = -1;
    for (int i = 0; i < books.size(); ++i) {
        if (books[i].id == id) index = i;
    }
    if (index < 0) {
        QMessageBox::warning(this, QStringLiteral("删除失败"), QStringLiteral("该图书已经不存在，请刷新列表。"));
        return;
    }
    if (books[index].available != books[index].total) {
        QMessageBox::warning(this, QStringLiteral("删除失败"),
                             QStringLiteral("该书仍有借出库存或库存数据不一致，不能删除。"));
        return;
    }
    if (!QFile::exists(dataPath(QStringLiteral("borrow.txt")))) {
        QMessageBox::warning(this, QStringLiteral("删除失败"), QStringLiteral("无法检查借阅记录，未执行删除。"));
        return;
    }
    QVector<BorrowRecord> records;
    if (!loadRecords(records, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }
    for (const BorrowRecord &r : records) {
        if ((r.bookId.isEmpty() ? r.bookName == books[index].name : r.bookId == id)
            && (r.count > 0 || r.status != QStringLiteral("已归还"))) {
            QMessageBox::warning(this, QStringLiteral("删除失败"),
                QStringLiteral("存在未归还记录，不能删除。旧同名记录没有编号时需要先核实归属。"));
            return;
        }
    }
    books.removeAt(index);

    if (!saveBooks(books, &error)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), error);
        return;
    }
    refreshBooks();
}

void MainWindow::sortBooks()
{
    m_sorted = true;
    refreshBooks();
    statusBar()->showMessage(
        QStringLiteral("已按借阅次数从高到低排序"), 3000);
}

void MainWindow::borrowBook()
{
    QVector<Book> books;
    QString error;
    if (!loadBooks(books, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("借阅图书"));
    dlg.setMinimumWidth(420);

    auto *bookCombo = new QComboBox(&dlg);
    for (const Book &b : books) {
        if (b.available > 0) {
            bookCombo->addItem(
                QStringLiteral("%1 [%2]（可借 %3）").arg(b.name, b.id).arg(b.available),
                b.id);
        }
    }
    if (bookCombo->count() == 0) {
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("当前没有可借的图书。"));
        return;
    }

    auto *sidEdit = new QLineEdit(&dlg);
    auto *personEdit = new QLineEdit(&dlg);

    auto *countSpin = new QSpinBox(&dlg);
    countSpin->setRange(1, 200);

    auto *dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    dateEdit->setCalendarPopup(true);

    auto *dueLabel = new QLabel(&dlg);
    auto updateDue = [dateEdit, dueLabel]() {
        dueLabel->setText(
            QStringLiteral("应还日期：%1")
                .arg(dateEdit->date().addDays(30)
                         .toString(QStringLiteral("yyyy-MM-dd"))));
    };
    updateDue();
    connect(dateEdit, &QDateEdit::dateChanged, &dlg,
            [updateDue](const QDate &) { updateDue(); });

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("书名："), bookCombo);
    form->addRow(QStringLiteral("学号："), sidEdit);
    form->addRow(QStringLiteral("姓名："), personEdit);
    form->addRow(QStringLiteral("数量："), countSpin);
    form->addRow(QStringLiteral("借书日期："), dateEdit);
    form->addRow(dueLabel);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    useChineseButtons(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    auto *layout = new QVBoxLayout(&dlg);
    layout->addLayout(form);
    layout->addWidget(buttons);

    if (execForm(dlg) != QDialog::Accepted)
        return;

    const QString bookId = bookCombo->currentData().toString();
    const QString sid = sidEdit->text().trimmed();
    const QString person = personEdit->text().trimmed();
    const int count = countSpin->value();
    const QDate borrowDate = dateEdit->date();

    if (sid.isEmpty() || person.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("输入有误"),
                             QStringLiteral("学号和姓名不能为空。"));
        return;
    }
    if (sid.contains(QRegularExpression(QStringLiteral("\\s")))
        || person.contains(QRegularExpression(QStringLiteral("\\s")))) {
        QMessageBox::warning(this, QStringLiteral("输入有误"),
                             QStringLiteral("学号和姓名不能包含空格。"));
        return;
    }

    int index = -1;
    for (int i = 0; i < books.size(); ++i) {
        if (books[i].id == bookId) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        QMessageBox::warning(this, QStringLiteral("借阅失败"),
                             QStringLiteral("这本书已经不存在了。"));
        return;
    }
    if (books[index].available < count) {
        QMessageBox::warning(this, QStringLiteral("借阅失败"),
                             QStringLiteral("可借数量不足。"));
        return;
    }

    QVector<BorrowRecord> records;
    if (!loadRecords(records, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }

    if (records.size() >= 5000) {
        QMessageBox::warning(this, QStringLiteral("借阅失败"),
                             QStringLiteral("借阅记录已达到 5000 条上限。"));
        return;
    }

    int maxNo = 0;
    for (const BorrowRecord &r : records) {
        if (r.id.startsWith(QStringLiteral("L")))
            maxNo = qMax(maxNo, r.id.mid(1).toInt());
    }
    const QString recordId = QStringLiteral("L%1")
                                 .arg(maxNo + 1, 6, 10, QChar('0'));

    BorrowRecord r;
    r.id = recordId;
    r.bookName = books[index].name;
    r.bookId = books[index].id;
    r.studentId = sid;
    r.personName = person;
    r.borrowDate = borrowDate.toString(QStringLiteral("yyyy-MM-dd"));
    r.dueDate = borrowDate.addDays(30).toString(QStringLiteral("yyyy-MM-dd"));
    r.count = count;
    r.days = 30;
    r.status = QStringLiteral("未归还");

    books[index].available -= count;
    books[index].borrowed += 1;

    if (!saveBooks(books, &error)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), error);
        return;
    }
    if (!appendRecord(r, &error)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), error);
        return;
    }

    refreshBooks();
    refreshRecords();
    refreshRemind();

    QMessageBox::information(
        this, QStringLiteral("借阅成功"),
        QStringLiteral("《%1》× %2\n借阅人：%3（%4）\n应还日期：%5")
            .arg(r.bookName)
            .arg(count)
            .arg(person)
            .arg(sid)
            .arg(r.dueDate));
}

void MainWindow::returnBook()
{
    const int row = ui->recordTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("请先在借阅记录里选中一行。"));
        return;
    }

    QVector<BorrowRecord> records;
    QString error;
    if (!loadRecords(records, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }
    if (row >= records.size())
        return;

    if (records[row].status == QStringLiteral("已归还")
        || records[row].count <= 0) {
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("这条记录已经还完了。"));
        return;
    }

    BorrowRecord &r = records[row];

    QDialog dlg(this);
    dlg.setWindowTitle(QStringLiteral("归还图书"));
    dlg.setMinimumWidth(400);

    auto *countSpin = new QSpinBox(&dlg);
    countSpin->setRange(1, r.count);
    countSpin->setValue(1);

    auto *dateEdit = new QDateEdit(QDate::currentDate(), &dlg);
    dateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    dateEdit->setCalendarPopup(true);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("《%1》未还 %2 本，本次归还：")
                     .arg(r.bookName).arg(r.count),
                 countSpin);
    form->addRow(QStringLiteral("归还日期："), dateEdit);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    useChineseButtons(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    auto *layout = new QVBoxLayout(&dlg);
    layout->addLayout(form);
    layout->addWidget(buttons);

    if (execForm(dlg) != QDialog::Accepted)
        return;

    const int back = countSpin->value();
    const QDate returnDate = dateEdit->date();

    r.count -= back;
    if (r.count == 0)
        r.status = QStringLiteral("已归还");

    const QDate borrowDate = QDate::fromString(
        r.borrowDate, QStringLiteral("yyyy-MM-dd"));
    if (borrowDate.isValid())
        r.days = static_cast<int>(borrowDate.daysTo(returnDate));

    QVector<Book> books;
    if (!loadBooks(books, &error)) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), error);
        return;
    }

    int bookIndex = -1;
    int matches = 0;
    for (int i = 0; i < books.size(); ++i) {
        if ((!r.bookId.isEmpty() && books[i].id == r.bookId && books[i].name == r.bookName)
            || (r.bookId.isEmpty() && books[i].name == r.bookName)) {
            bookIndex = i;
            ++matches;
        }
    }
    if (matches != 1 || books[bookIndex].available + back > books[bookIndex].total) {
        QMessageBox::warning(this, QStringLiteral("归还失败"),
            QStringLiteral("无法唯一确定图书，或归还数量超过借出库存。旧记录没有编号时需要先核实归属。"));
        return;
    }
    r.bookId = books[bookIndex].id;
    books[bookIndex].available += back;

    if (!saveRecords(records, &error)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), error);
        return;
    }
    if (!saveBooks(books, &error)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), error);
        return;
    }

    refreshBooks();
    refreshRecords();
    refreshRemind();

    QMessageBox::information(
        this, QStringLiteral("归还成功"),
        QStringLiteral("《%1》已归还 %2 本。").arg(r.bookName).arg(back));
}
