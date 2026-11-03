#ifndef book_h
#define book_h

typedef struct
{
    char id[20];         // 图书编号
    char name[100];      // 图书名称
    char leibie[50];     // 图书类别
    int totalCopies;     // 图书总数量
    int availableCopies; // 当前可借数量
    int borrowCount;     // 历史借阅次数
} Book;

typedef struct 
{
    char bookname[100];          //书名
    char sid[20];               //学号
    char peoplename[100];          //姓名
    char borrowtime[15];           //借书时间
    char guihuantime[15];         //归还时间
    int borrowcount;            //借阅数量
    int havetime;               //持有时间
    char status[10];             //状态“已归还”“未归还”“未完全归还”
}Borrow;


#endif