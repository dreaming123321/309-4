// libraryshell.cpp : 首页、二级页面和界面动画。
// 原有业务函数保留在 mainwindow.cpp。
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractButton>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEventLoop>
#include <QFile>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSaveFile>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTabBar>
#include <QTextStream>
#include <QTimer>
#include <QVariantAnimation>
#include <QVBoxLayout>
#include <QStyle>

namespace {
QLabel *label(const QString &text, const char *name, QWidget *parent = nullptr)
{
    auto *item = new QLabel(text, parent);
    item->setObjectName(QString::fromLatin1(name));
    item->setWordWrap(true);
    return item;
}

void drawIcon(QPainter &p, int type, const QRectF &rect, QColor color)
{
    p.save();
    p.translate(rect.topLeft());
    p.scale(rect.width() / 24, rect.height() / 24);
    p.setPen(QPen(color, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    switch (type) {
    case 0:
        p.drawRoundedRect(QRectF(4, 3, 14, 18), 2, 2);
        p.drawLine(8, 3, 8, 21); p.drawLine(11, 10, 17, 10); p.drawLine(14, 7, 14, 13); break;
    case 1:
        p.drawLine(4, 6, 20, 6); p.drawLine(9, 3, 15, 3);
        p.drawLine(6, 6, 7, 21); p.drawLine(18, 6, 17, 21); p.drawLine(7, 21, 17, 21);
        p.drawLine(10, 10, 10, 17); p.drawLine(14, 10, 14, 17); break;
    case 2:
        p.drawEllipse(QRectF(3, 3, 13, 13)); p.drawLine(14, 14, 21, 21); break;
    case 3:
        p.drawLine(4, 20, 4, 13); p.drawLine(11, 20, 11, 8); p.drawLine(18, 20, 18, 3);
        p.drawLine(2, 22, 22, 22); break;
    case 4:
        p.drawLine(5, 14, 5, 21); p.drawLine(5, 21, 20, 21); p.drawLine(20, 21, 20, 14);
        p.drawLine(12, 3, 12, 16); p.drawLine(8, 7, 12, 3); p.drawLine(16, 7, 12, 3); break;
    case 5:
        p.drawRoundedRect(QRectF(3, 3, 13, 18), 2, 2); p.drawLine(7, 3, 7, 21);
        p.drawLine(11, 12, 22, 12); p.drawLine(18, 8, 22, 12); p.drawLine(18, 16, 22, 12); break;
    case 6:
        p.drawRoundedRect(QRectF(7, 3, 13, 18), 2, 2); p.drawLine(16, 3, 16, 21);
        p.drawLine(2, 12, 13, 12); p.drawLine(2, 12, 6, 8); p.drawLine(2, 12, 6, 16); break;
    default:
        p.drawArc(QRectF(6, 3, 12, 15), 0, 180 * 16);
        p.drawLine(6, 10, 5, 18); p.drawLine(18, 10, 19, 18); p.drawLine(5, 18, 19, 18);
        p.drawArc(QRectF(10, 18, 4, 4), 180 * 16, 180 * 16); break;
    }
    p.restore();
}

class MenuCard : public QAbstractButton
{
public:
    MenuCard(int type, const QString &title, const QString &detail, bool *motion, QWidget *parent = nullptr)
        : QAbstractButton(parent), m_type(type), m_title(title), m_detail(detail), m_motion(motion)
    {
        setMinimumSize(170, 133);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setCursor(Qt::PointingHandCursor);
        setFocusPolicy(Qt::StrongFocus);
        setAccessibleName(title);
        setAccessibleDescription(detail);
        m_animation.setDuration(150);
        connect(&m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
            m_hover = v.toReal(); update();
        });
    }
protected:
    void enterEvent(QEnterEvent *event) override { animate(1); QAbstractButton::enterEvent(event); }
    void leaveEvent(QEvent *event) override { animate(0); QAbstractButton::leaveEvent(event); }
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        QRectF box = QRectF(rect()).adjusted(1, 1, -1, -1);
        QColor surface = QColor::fromRgbF(1 - .035 * m_hover, 1 - .015 * m_hover, 1 - .025 * m_hover);
        p.setBrush(isDown() ? QColor("#e6f0ee") : surface);
        p.setPen(QPen(hasFocus() ? QColor("#157d70") : (m_hover > .2 ? QColor("#9fc4bc") : QColor("#e1e7e5")), hasFocus() ? 2 : 1));
        p.drawRoundedRect(box, 14, 14);
        p.setPen(Qt::NoPen); p.setBrush(QColor("#edf4f1")); p.drawRoundedRect(QRectF(18, 17, 38, 38), 11, 11);
        drawIcon(p, m_type, QRectF(26, 25, 22, 22), QColor("#2b756a"));
        p.setPen(QColor("#82958e")); QFont small = font(); small.setPixelSize(11); p.setFont(small);
        p.drawText(QRectF(width()-45, 20, 25, 20), Qt::AlignRight, QStringLiteral("0%1").arg(m_type + 1));
        QFont title = font(); title.setPixelSize(17); title.setBold(true); p.setFont(title); p.setPen(QColor("#233b39"));
        p.drawText(QRectF(18, 66, width()-36, 25), Qt::AlignLeft | Qt::AlignVCenter, m_title);
        QFont detail = font(); detail.setPixelSize(12); p.setFont(detail); p.setPen(QColor("#657873"));
        p.drawText(QRectF(18, 97, width()-36, height()-104), Qt::AlignLeft | Qt::TextWordWrap, m_detail);
    }
private:
    void animate(qreal end) {
        m_animation.stop();
        if (!*m_motion) { m_hover = end; update(); return; }
        m_animation.setStartValue(m_hover); m_animation.setEndValue(end); m_animation.start();
    }
    int m_type; QString m_title, m_detail; bool *m_motion; qreal m_hover = 0; QVariantAnimation m_animation;
};

class LibraryHero : public QFrame
{
public:
    explicit LibraryHero(QWidget *parent = nullptr) : QFrame(parent) { setMinimumHeight(158); }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        QLinearGradient g(0, 0, width(), height()); g.setColorAt(0, QColor("#163f3d")); g.setColorAt(1, QColor("#25665b"));
        p.setPen(Qt::NoPen); p.setBrush(g); p.drawRoundedRect(rect(), 18, 18);
        p.setPen(QPen(QColor(255,255,255,20), 1)); p.setBrush(Qt::NoBrush);
        p.drawEllipse(QPointF(width()-100, 70), 150, 150); p.drawEllipse(QPointF(width()-100, 70), 122, 122);
        p.save(); p.translate(width()-210, 24); p.rotate(-7);
        const QColor colors[] = {QColor("#a7c7b8"), QColor("#e6cf9d"), QColor("#d9e3cf")};
        for (int i=0; i<3; ++i) {
            QRectF book(18 + i*42, 12-i*4, 34, 103+i*4);
            p.setPen(Qt::NoPen); p.setBrush(colors[i]); p.drawRoundedRect(book, 4, 4);
            p.setPen(QPen(QColor(22,63,61,60),1)); p.drawLine(book.left()+6, book.top()+4, book.left()+6, book.bottom()-4);
            p.drawLine(book.left()+12, book.top()+19, book.right()-6, book.top()+19);
            p.drawLine(book.left()+12, book.top()+25, book.right()-6, book.top()+25);
        }
        p.restore();
    }
};
}

void MainWindow::setupShell()
{
    resize(1220, 820); setMinimumSize(1000, 720);
    setWindowTitle(QStringLiteral("书屿 · 图书馆管理系统"));
    setFont(QFont(QStringLiteral("Microsoft YaHei UI"), 10));
    setStyleSheet(QString::fromUtf8(R"QSS(
        QMainWindow, QWidget#shell, QWidget#homeContent, QWidget#formContent { background: #f4f6f2; color: #253b38; }
        QWidget { font-family: "Microsoft YaHei UI"; font-size: 13px; }
        QLabel { color: #253b38; background: transparent; }
        QFrame#sidebar { background: #152e30; border: none; }
        QLabel#brand { color: #f3f7f0; font-size: 24px; font-weight: 700; }
        QLabel#brandCaption { color: #b2c7c0; font-size: 11px; }
        QLabel#sideSection { color: #9eb4ad; font-size: 11px; padding-top: 18px; padding-bottom: 6px; }
        QLabel#sideFoot { color: #9eb4ad; font-size: 11px; }
        QPushButton[nav="true"] { background: transparent; color: #d5e3dd; text-align: left; border: none; border-radius: 9px; padding: 12px 15px; }
        QPushButton[nav="true"][active="true"], QPushButton[nav="true"]:hover, QPushButton[nav="true"]:focus { background: #2b4947; color: white; }
        QPushButton[nav="true"]:disabled { color: #6f8882; }
        QLabel#pageTitle { font-size: 26px; font-weight: 700; }
        QLabel#pageDescription { color: #687b74; font-size: 12px; }
        QLabel#eyebrow { color: #568175; font-size: 10px; font-weight: 700; }
        QLabel#heroTitle { color: #f9fbf5; font-size: 28px; font-weight: 700; }
        QLabel#heroText { color: #d5e6dc; font-size: 13px; }
        QLabel#heroEyebrow { color: #b4d1bc; font-size: 10px; }
        QLabel#sectionTitle { font-size: 16px; font-weight: 700; }
        QLabel#muted, QLabel#dataStatus { color: #687b74; font-size: 12px; }
        QFrame#statCard, QFrame#formCard { background: #ffffff; border: 1px solid #e0e7e1; border-radius: 14px; }
        QLabel#statValue { color: #244d43; font-size: 27px; font-weight: 700; }
        QLabel#statCaption { color: #687b74; font-size: 12px; }
        QPushButton { background: #ffffff; color: #264b42; border: 1px solid #d9e2dc; border-radius: 8px; padding: 10px 17px; min-height: 20px; }
        QPushButton:hover { background: #eaf2ed; border-color: #9dbeb2; }
        QPushButton:pressed { background: #d9e9df; }
        QPushButton:focus { border: 2px solid #278473; }
        QPushButton[primary="true"] { background: #247969; color: white; border-color: #247969; }
        QPushButton[primary="true"]:hover { background: #1b6659; }
        QPushButton[primary="true"]:focus { border: 2px solid #a0d0bc; }
        QPushButton#deleteButton { color: #a14e46; }
        QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateEdit { background: white; border: 1px solid #ccd9d0; border-radius: 8px; padding: 9px 12px; min-height: 24px; selection-background-color: #247969; color: #253b38; }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus { border: 2px solid #278473; }
        QComboBox::drop-down { width: 25px; border: none; }
        QSpinBox::up-button, QDoubleSpinBox::up-button { width: 22px; }
        QSpinBox::down-button, QDoubleSpinBox::down-button { width: 22px; }
        QTableWidget { border: 1px solid #dce5df; border-radius: 10px; background: #ffffff; alternate-background-color: #f6f9f5; gridline-color: #edf1eb; selection-background-color: #dceee4; selection-color: #174d3d; }
        QTableWidget::item { padding: 9px 5px; border: none; }
        QHeaderView::section { background: #eaf0e9; color: #486158; border: none; padding: 13px 5px; font-weight: 700; }
        QTabWidget::pane { border: none; background: transparent; }
        QScrollArea { border: none; background: transparent; }
        QScrollBar:vertical { background: transparent; width: 8px; margin: 2px; }
        QScrollBar::handle:vertical { background: #c4d3c9; border-radius: 4px; min-height: 30px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QCheckBox { color: #60796d; spacing: 7px; font-size: 11px; }
        QStatusBar { background: #eef2eb; color: #60756a; padding: 3px 12px; font-size: 11px; }
    )QSS"));

    auto *shell = new QWidget; shell->setObjectName(QStringLiteral("shell"));
    auto *root = new QHBoxLayout(shell); root->setContentsMargins(0,0,0,0); root->setSpacing(0);
    auto *side = new QFrame; side->setObjectName(QStringLiteral("sidebar")); side->setFixedWidth(186);
    auto *sideLayout = new QVBoxLayout(side); sideLayout->setContentsMargins(21,30,21,22); sideLayout->setSpacing(7);
    sideLayout->addWidget(label(QStringLiteral("书 屿"), "brand"));
    sideLayout->addWidget(label(QStringLiteral("LIBRARY  /  309-4"), "brandCaption"));
    sideLayout->addWidget(label(QStringLiteral("工作空间"), "sideSection"));
    m_navigation = new QWidget; auto *nav = new QVBoxLayout(m_navigation); nav->setContentsMargins(0,0,0,0); nav->setSpacing(6);
    const QStringList names = {QStringLiteral("首页总览"), QStringLiteral("馆藏图书"), QStringLiteral("借阅记录"), QStringLiteral("到期提醒")};
    for (int i=0; i<names.size(); ++i) {
        auto *button = new QPushButton(names[i]); button->setProperty("nav", true); button->setObjectName(QStringLiteral("nav%1").arg(i)); button->setCursor(Qt::PointingHandCursor); nav->addWidget(button);
        connect(button, &QPushButton::clicked, this, [this,i] { if (i==0) showHome(); else showWorkspace(i-1); });
    }
    sideLayout->addWidget(m_navigation); sideLayout->addStretch();
    sideLayout->addWidget(label(QStringLiteral("仙林 · 三牌楼\n一处整理，两地共享"), "sideFoot"));
    auto *exit = new QPushButton(QStringLiteral("退出系统")); exit->setProperty("nav",true); sideLayout->addWidget(exit); connect(exit,&QPushButton::clicked,this,&QWidget::close);
    root->addWidget(side);

    auto *body = new QWidget; auto *bodyLayout = new QVBoxLayout(body); bodyLayout->setContentsMargins(28,22,28,20); bodyLayout->setSpacing(17);
    auto *top = new QHBoxLayout;
    auto *heading = new QVBoxLayout; heading->setSpacing(5);
    heading->addWidget(label(QStringLiteral("LIBRARY WORKSPACE"), "eyebrow"));
    m_pageTitle = label(QStringLiteral("让每一本书，找到它的位置。"), "pageTitle"); heading->addWidget(m_pageTitle);
    m_pageDescription = label(QStringLiteral("从一个操作开始，轻松管理今天的馆藏与借阅。"), "pageDescription"); heading->addWidget(m_pageDescription);
    top->addLayout(heading,1);
    auto *tools = new QVBoxLayout; tools->setAlignment(Qt::AlignRight);
    m_backButton = new QPushButton(QStringLiteral("←  返回首页")); m_backButton->setObjectName(QStringLiteral("pageBack")); tools->addWidget(m_backButton);
    connect(m_backButton,&QPushButton::clicked,this,[this] { if (m_activeForm) m_activeForm->reject(); else showHome(); });
    auto *motion = new QCheckBox(QStringLiteral("页面动效")); motion->setChecked(true); motion->setObjectName(QStringLiteral("motionToggle")); tools->addWidget(motion);
    connect(motion,&QCheckBox::toggled,this,[this](bool on) { m_motionEnabled=on; if (!on && m_pageAnimation) { m_pageAnimation->stop(); if (auto *effect=qobject_cast<QGraphicsOpacityEffect *>(m_pages->graphicsEffect())) effect->setOpacity(1); } });
    top->addLayout(tools); bodyLayout->addLayout(top);
    m_pages = new QStackedWidget; m_pages->setObjectName(QStringLiteral("pageStack")); bodyLayout->addWidget(m_pages,1); root->addWidget(body,1);

    // 首页：与 main.c 的八个菜单入口保持对应。
    auto *homeScroll = new QScrollArea; homeScroll->setWidgetResizable(true); homeScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_homePage=homeScroll; auto *home=new QWidget; home->setObjectName(QStringLiteral("homeContent")); homeScroll->setWidget(home);
    auto *homeLayout=new QVBoxLayout(home); homeLayout->setContentsMargins(0,0,5,0); homeLayout->setSpacing(16);
    auto *hero = new LibraryHero; auto *heroLayout=new QVBoxLayout(hero); heroLayout->setContentsMargins(25,22,235,20); heroLayout->setSpacing(7);
    heroLayout->addWidget(label(QStringLiteral("READ. RETURN. REPEAT."),"heroEyebrow"));
    heroLayout->addWidget(label(QStringLiteral("书在流动，知识在生长。"),"heroTitle"));
    heroLayout->addWidget(label(QStringLiteral("每一次借阅，都是下一段探索的开始。"),"heroText"));
    homeLayout->addWidget(hero);
    auto *stats=new QHBoxLayout; stats->setSpacing(14);
    const QStringList captions={QStringLiteral("馆藏记录 / 最多 300 条"),QStringLiteral("当前可借 / 册"),QStringLiteral("待归还提醒 / 条")};
    QLabel **values[]={&m_bookStat,&m_availableStat,&m_dueStat};
    for(int i=0;i<3;i++) { auto *card=new QFrame; card->setObjectName(QStringLiteral("statCard")); auto *l=new QVBoxLayout(card); l->setContentsMargins(20,13,20,13); l->setSpacing(3); *values[i]=label(QStringLiteral("—"),"statValue"); l->addWidget(*values[i]); l->addWidget(label(captions[i],"statCaption")); stats->addWidget(card); }
    homeLayout->addLayout(stats);
    auto *section = new QHBoxLayout; section->addWidget(label(QStringLiteral("今天，你想做什么？"),"sectionTitle")); section->addStretch(); section->addWidget(label(QStringLiteral("选择功能，进入下一步"),"muted")); homeLayout->addLayout(section);
    auto *grid = new QGridLayout; grid->setSpacing(12);
    const QStringList titles={QStringLiteral("添加新书"),QStringLiteral("删除图书"),QStringLiteral("查询图书"),QStringLiteral("借阅排行"),QStringLiteral("输出信息"),QStringLiteral("借阅图书"),QStringLiteral("归还图书"),QStringLiteral("到期提醒")};
    const QStringList details={QStringLiteral("录入校区、书名与数量"),QStringLiteral("按编号选择待删除图书"),QStringLiteral("查找书名、类别或校区"),QStringLiteral("查看大家最常借阅的书"),QStringLiteral("导出当前馆藏信息"),QStringLiteral("填写读者与借阅日期"),QStringLiteral("选择记录，登记归还"),QStringLiteral("查看即将到期与逾期记录")};
    for(int i=0;i<8;i++) {
        auto *card=new MenuCard(i,titles[i],details[i],&m_motionEnabled); card->setObjectName(QStringLiteral("homeAction%1").arg(i+1)); grid->addWidget(card,i/4,i%4);
        connect(card,&QAbstractButton::clicked,this,[this,i] {
            switch(i) {
            case 0: addBook(); break;
            case 1: showWorkspace(0,QStringLiteral("删除图书")); m_pageDescription->setText(QStringLiteral("选中要删除的图书，再点击「删除选中」。")); break;
            case 2: showWorkspace(0,QStringLiteral("查询图书")); ui->searchEdit->setFocus(); break;
            case 3: showWorkspace(0,QStringLiteral("借阅排行")); sortBooks(); break;
            case 4: exportBooks(); break;
            case 5: borrowBook(); break;
            case 6: showWorkspace(1,QStringLiteral("归还图书")); m_pageDescription->setText(QStringLiteral("选中一条未归还记录，再点击「归还选中」。")); break;
            case 7: showWorkspace(2); break;
            }
        });
    }
    homeLayout->addLayout(grid); m_dataStatus=label(QString(),"dataStatus"); homeLayout->addWidget(m_dataStatus); homeLayout->addStretch();
    m_pages->addWidget(m_homePage);

    // 原有表格页面移入第二层，保持控件名称和功能连接。
    ui->mainTabs->setParent(nullptr); ui->mainTabs->tabBar()->hide();
    m_workspacePage=new QWidget; auto *workLayout=new QVBoxLayout(m_workspacePage); workLayout->setContentsMargins(0,0,0,0); workLayout->setSpacing(14);
    auto *tabs=new QHBoxLayout;
    for(int i=0;i<3;i++) { auto *button=new QPushButton(names[i+1]); button->setObjectName(QStringLiteral("workspaceTab%1").arg(i)); tabs->addWidget(button); connect(button,&QPushButton::clicked,this,[this,i] { showWorkspace(i); }); }
    tabs->addStretch(); workLayout->addLayout(tabs); workLayout->addWidget(ui->mainTabs,1); m_pages->addWidget(m_workspacePage);
    for (auto *table : {ui->bookTable,ui->recordTable,ui->remindTable}) { table->setShowGrid(false); table->verticalHeader()->setDefaultSectionSize(46); table->setFocusPolicy(Qt::StrongFocus); }
    ui->addButton->setText(QStringLiteral("添加新书")); ui->borrowButton->setText(QStringLiteral("借阅图书")); ui->addButton->setProperty("primary",true); ui->borrowButton->setProperty("primary",true);

    auto *formScroll=new QScrollArea; formScroll->setWidgetResizable(true); formScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); m_formPage=formScroll;
    auto *formBody=new QWidget; formBody->setObjectName(QStringLiteral("formContent")); formScroll->setWidget(formBody); auto *formLayout=new QVBoxLayout(formBody); formLayout->setContentsMargins(0,10,0,10); formLayout->setSpacing(15);
    auto *steps=label(QStringLiteral("01  选择功能     /     02  填写信息     /     03  确认完成"),"muted"); formLayout->addWidget(steps);
    auto *formCard=new QFrame; formCard->setObjectName(QStringLiteral("formCard")); formCard->setMaximumWidth(800); auto *cardLayout=new QVBoxLayout(formCard); cardLayout->setContentsMargins(28,25,28,25);
    cardLayout->addWidget(label(QStringLiteral("填写操作信息"),"sectionTitle"));
    m_formHost=new QWidget; m_formHost->setObjectName(QStringLiteral("formHost")); auto *hostLayout=new QVBoxLayout(m_formHost); hostLayout->setContentsMargins(0,15,0,0); cardLayout->addWidget(m_formHost);
    formLayout->addWidget(formCard); formLayout->addWidget(label(QStringLiteral("确认前请核对信息。点击「返回」或「取消」可退出本次填写。"),"muted")); formLayout->addStretch(); m_pages->addWidget(m_formPage);
    QWidget *old=takeCentralWidget(); setCentralWidget(shell); if(old) old->deleteLater();
    auto *opacity=new QGraphicsOpacityEffect(m_pages); opacity->setOpacity(1); m_pages->setGraphicsEffect(opacity);
    m_pageAnimation=new QPropertyAnimation(opacity,"opacity",this); m_pageAnimation->setDuration(200); m_pageAnimation->setEasingCurve(QEasingCurve::OutCubic);
    auto *timer=new QTimer(this); timer->setInterval(60000); connect(timer,&QTimer::timeout,this,[this] { if(!m_activeForm) { refreshDashboard(); if(m_pages->currentWidget()==m_workspacePage && ui->mainTabs->currentIndex()==2) refreshRemind(); } }); timer->start();
    showHome();
}

void MainWindow::switchPage(QWidget *page)
{
    m_pageAnimation->stop(); m_pages->setCurrentWidget(page);
    auto *effect=qobject_cast<QGraphicsOpacityEffect *>(m_pages->graphicsEffect());
    if(m_motionEnabled) { m_pageAnimation->setStartValue(.2); m_pageAnimation->setEndValue(1.0); m_pageAnimation->start(); }
    else effect->setOpacity(1);
}

void MainWindow::showHome()
{
    if(m_activeForm) return;
    m_pageTitle->setText(QStringLiteral("让每一本书，找到它的位置。"));
    m_pageDescription->setText(QStringLiteral("从一个操作开始，轻松管理今天的馆藏与借阅。"));
    for(int i=0;i<4;i++) { auto *button=m_navigation->findChild<QPushButton *>(QStringLiteral("nav%1").arg(i)); button->setProperty("active",i==0); button->style()->unpolish(button); button->style()->polish(button); }
    m_backButton->hide(); refreshDashboard(); switchPage(m_homePage);
}

void MainWindow::showWorkspace(int tab, const QString &title)
{
    if(m_activeForm) return;
    const QStringList titles={QStringLiteral("馆藏图书"),QStringLiteral("借阅记录"),QStringLiteral("到期提醒")};
    const QStringList notes={QStringLiteral("按书名、编号、类别或校区筛选，选择一行继续操作。"),QStringLiteral("每一笔借阅都有记录，选中后可登记部分或全部归还。"),QStringLiteral("到期前 7 天开始提醒，逾期未还的记录继续保留。")};
    m_pageTitle->setText(title.isEmpty()?titles[tab]:title); m_pageDescription->setText(notes[tab]);
    m_backButton->setText(QStringLiteral("←  返回首页")); m_backButton->show(); ui->mainTabs->setCurrentIndex(tab);
    if(tab==0) refreshBooks(); else if(tab==1) refreshRecords(); else refreshRemind();
    for(int i=0;i<4;i++) { auto *button=m_navigation->findChild<QPushButton *>(QStringLiteral("nav%1").arg(i)); button->setProperty("active",i==tab+1); button->style()->unpolish(button); button->style()->polish(button); }
    switchPage(m_workspacePage);
}

void MainWindow::refreshDashboard()
{
    if(!m_bookStat) return;
    QVector<Book> books; QVector<BorrowRecord> records; QString error;
    bool booksOk=loadBooks(books,&error), recordsOk=loadRecords(records,&error);
    int available=0, due=0;
    for(const Book &b:books) available+=b.available;
    for(const BorrowRecord &r:records) { QDate date=QDate::fromString(r.dueDate,QStringLiteral("yyyy-MM-dd")); if(r.count>0 && r.status!=QStringLiteral("已归还") && date.isValid() && QDate::currentDate().daysTo(date)<=7) due++; }
    m_bookStat->setText(booksOk?QString::number(books.size()):QStringLiteral("—"));
    m_availableStat->setText(booksOk?QString::number(available):QStringLiteral("—"));
    m_dueStat->setText(recordsOk?QString::number(due):QStringLiteral("—"));
    m_dataStatus->setText(booksOk && recordsOk ? QStringLiteral("%1  ·  本地馆藏已读取  ·  单机工作空间").arg(QDate::currentDate().toString(QStringLiteral("yyyy 年 M 月 d 日"))) : QStringLiteral("数据读取失败：%1").arg(error));
}

int MainWindow::execForm(QDialog &dialog)
{
    QWidget *previous=m_pages->currentWidget(); QString title=m_pageTitle->text(), description=m_pageDescription->text();
    m_activeForm=&dialog; m_navigation->setEnabled(false);
    m_pageTitle->setText(dialog.windowTitle());
    const bool isAdd=dialog.windowTitle()==QStringLiteral("添加图书");
    m_pageDescription->setText(isAdd ? QStringLiteral("新书按所填总数入库；同校区同名书按原规则增加 1 本。") : QStringLiteral("填写下方信息，确认后完成本次操作。"));
    m_backButton->setText(QStringLiteral("←  返回上页")); m_backButton->show();
    dialog.setParent(m_formHost,Qt::Widget); dialog.setMinimumWidth(0); dialog.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred); m_formHost->layout()->addWidget(&dialog);
    if(auto *layout=dialog.layout()) { layout->setContentsMargins(0,0,0,0); layout->setSpacing(23); }
    for(auto *form:dialog.findChildren<QFormLayout *>()) { form->setVerticalSpacing(18); form->setHorizontalSpacing(22); form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow); form->setLabelAlignment(Qt::AlignLeft|Qt::AlignVCenter); }
    for(auto *buttons:dialog.findChildren<QDialogButtonBox *>()) { if(auto *ok=buttons->button(QDialogButtonBox::Ok)) { ok->setText(QStringLiteral("确认提交")); ok->setProperty("primary",true); ok->style()->unpolish(ok); ok->style()->polish(ok); } }
    dialog.show(); switchPage(m_formPage);
    QEventLoop loop; connect(&dialog,&QDialog::finished,&loop,&QEventLoop::quit);
    loop.exec(); int result=dialog.result();
    m_formHost->layout()->removeWidget(&dialog); dialog.hide(); dialog.setParent(this,Qt::Dialog);
    m_activeForm=nullptr; m_navigation->setEnabled(true);
    m_pageTitle->setText(title); m_pageDescription->setText(description);
    m_backButton->setText(QStringLiteral("←  返回首页")); m_backButton->setVisible(previous!=m_homePage); switchPage(previous);
    QTimer::singleShot(0,this,&MainWindow::refreshDashboard);
    return result;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_activeForm) m_activeForm->reject();
    QMainWindow::closeEvent(event);
}

void MainWindow::exportBooks()
{
    QDialog dialog(this); dialog.setWindowTitle(QStringLiteral("输出信息"));
    auto *layout=new QVBoxLayout(&dialog);
    layout->addWidget(label(QStringLiteral("导出当前馆藏清单"),"sectionTitle"));
    layout->addWidget(label(QStringLiteral("输出图书编号、书名、类别、价格、库存和借阅次数，并标明所属校区。"),"muted"));
    auto *path=new QLineEdit(dataPath(QStringLiteral("output.txt"))); path->setReadOnly(true); layout->addWidget(path);
    auto *buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel); buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消")); layout->addWidget(buttons);
    connect(buttons,&QDialogButtonBox::accepted,&dialog,&QDialog::accept); connect(buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);
    if(execForm(dialog)!=QDialog::Accepted) return;
    QVector<Book> books; QString error;
    if(!loadBooks(books,&error)) { QMessageBox::warning(this,QStringLiteral("输出失败"),error); return; }
    QSaveFile file(dataPath(QStringLiteral("output.txt")));
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)) { QMessageBox::warning(this,QStringLiteral("输出失败"),file.errorString()); return; }
    QTextStream out(&file); out.setEncoding(QStringConverter::Utf8);
    for(const Book &b:books) out<<b.id<<' '<<b.name<<' '<<b.category<<' '<<QString::number(b.price,'f',2)<<' '<<b.total<<' '<<b.available<<' '<<b.borrowed<<' '<<campusName(b.id)<<'\n';
    out.flush();
    if(out.status()!=QTextStream::Ok || !file.commit()) { QMessageBox::warning(this,QStringLiteral("输出失败"),QStringLiteral("文件未能成功保存。")); return; }
    QMessageBox::information(this,QStringLiteral("输出完成"),QStringLiteral("已输出 %1 条图书记录。\n%2").arg(books.size()).arg(file.fileName()));
}
