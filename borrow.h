#ifndef borrow_h
#define borrow_h

int lookname(char *bookname);  //这个函数专门根据书名查是否有这本书，如果有，返回1，如果没有，返回0
int lookcount(char *bookname);  //这个函数专门根据书名查这本书的可借数量，如果有，返回数量，如果没有，返回0
void updatebook(char *bookname,int change);        //功能：这个是修改book.txt文件
void saveborrow(char *bookname,char *sid,char *peoplename,char *borrowtime,char *guihuantime,int borrowcount);      //保存借阅记录的函数
int dateculculate(char *borrowtime,char *guihuantime);  //传入起始时间和最终时间，这个函数可以计算出经过的天数
char *jisuan_guihuantime(char *borrowtime);  //这个函数是根据借阅时间计算归还时间，归还时间是借阅时间加上30天
void borrow();
int looksid1(char *sid);
#endif