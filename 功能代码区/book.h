// book.h : 图书管理系统核心数据结构的头文件
//

#ifndef book_h
#define book_h

#define MAX_BOOKS 300
#define MAX_COPIES 200
#define MAX_BORROW_RECORDS 5000

// 图书信息结构体
// 用于存储图书的基本信息，包括编号、名称、类别、数量以及借阅统计。
typedef struct
{
    char id[20];              // 图书编号（如 XL-B001，前缀表示校区）
    char name[100];           // 图书名称
    char leibie[50];          // 图书类别（文学/计算机/科幻等）
    float price;              // 图书价格（单位：元）
    int totalCopies;          // 图书总数量（该种书的全部馆藏本数）
    int availableCopies;      // 当前可借数量（未被借出的本数，应 ≤ totalCopies）
    int borrowCount;           // 历史借阅次数（累计被借阅的总次数，availableCopies + 已借出 >= totalCopies）
} Book;

// 借阅信息结构体
// 用于记录每一笔借阅/归还操作的详细信息。
typedef struct 
{
    char bookid[20];           // 新记录关联具体图书；旧记录可能为空
    char borrowid[20];         // 借阅的唯一编号（系统自动生成）
    char bookname[100];        // 所借图书的书名
    char sid[20];              // 借阅人学号
    char peoplename[100];      // 借阅人姓名
    char borrowtime[15];       // 借书时间（格式: YYYY-MM-DD）
    char guihuantime[15];      // 应还时间（格式: YYYY-MM-DD）
    char actualtime[15];       // 全部还清的日期；旧记录可能没有
    int borrowcount;           // 本次借阅数量（借走了几本）
    int havetime;              // 持有时间（借书到还书相差的天数）
    char status[10];           // 借阅状态（"已归还" / "未归还" / "未完全归还"）
    int guihuancount;           // 已经归还的本数（部分归还场景）
} Borrow;

#endif