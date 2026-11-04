#ifndef borrow_h
#define borrow_h

#include <stddef.h>
#include "book.h"
int resolvebook(const char *input, Book *book);

int lookname(char *bookname);  //这个函数专门根据书名查是否有这本书，如果有，返回1，如果没有，返回0
int lookcount(char *bookname);  //这个函数专门根据书名查这本书的可借数量，如果有，返回数量，如果没有，返回0
void lockdata(void);
void unlockdata(void);
int validdate(const char *date);
int validtoken(const char *value, size_t size);
int readword(char *value, size_t size);
int readnumber(int *value);
int readrecords(Borrow *records, int *count);
int writerecords(Borrow *records, int count);
int preparebook(const char *id, int change, int borrowing);
int checkstock(Book *book, Borrow *records, int count);
int commitborrow(void);
int recoverborrow(void);
int saveborrow(char *bookname,char *sid,char *peoplename,char *borrowtime,char *guihuantime,int borrowcount);      //保存借阅记录的函数
int dateculculate(char *borrowtime,char *guihuantime);  //传入起始时间和最终时间，这个函数可以计算出经过的天数
char *jisuan_guihuantime(char *borrowtime);  //这个函数是根据借阅时间计算归还时间，归还时间是借阅时间加上30天
void borrow();
int looksid1(char *sid);
#endif