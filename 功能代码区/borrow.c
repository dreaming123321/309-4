#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include "book.h"
#include "borrow.h"
#include "remind.h"
#define max_havetime 30        //规定借阅时间上限是30天

// 返回 1 表示唯一匹配；同名或重复编号返回 -1。
int resolvebook(const char *input, Book *result)
{
    FILE *fp=fopen("book.txt","r"); char line[512]; Book b; int matches=0;
    if (!fp) return 0;
    while (fgets(line,sizeof(line),fp))
    {
        if (sscanf(line,"%19s %99s %49s %f %d %d %d",b.id,b.name,b.leibie,&b.price,
                   &b.totalCopies,&b.availableCopies,&b.borrowCount)!=7) continue;
        if (!strcmp(input,b.id) || !strcmp(input,b.name)) { *result=b; matches++; }
    }
    fclose(fp); return matches==1 ? 1 : matches ? -1 : 0;
}

void borrow()
{
    char choose[10], input[100], peoplename[100], sid[20], borrowtime[15];
    char guihuantime[15]; int borrowcount; Book selected;
    do
    {
        printf("请输入图书编号（书名唯一时也可输入书名）：");
        if (!readword(input,sizeof(input))) { printf("输入无效。\n"); return; }
        int result=resolvebook(input,&selected);
        if (result!=1) { printf(result<0 ? "匹配不唯一，请输入唯一的图书编号。\n" : "未找到图书。\n"); return; }
        if (selected.availableCopies<=0) { printf("该书已被借完，无法借阅。\n"); return; }
        printf("可借阅数量为：%d\n是否借阅?(y/n): ",selected.availableCopies);
        if (!readword(choose,sizeof(choose)) || strcmp(choose,"y")) return;
        printf("请输入借阅数量：\n");
        if (!readnumber(&borrowcount) || borrowcount>selected.availableCopies)
        { printf("借阅数量无效或大于可借数量。\n"); return; }
        printf("请输入借阅人姓名：\n");
        if (!readword(peoplename,sizeof(peoplename))) { printf("姓名无效。\n"); return; }
        printf("请输入借阅人学号：\n");
        if (!readword(sid,sizeof(sid))) { printf("学号无效。\n"); return; }
        printf("请输入借阅日期；(格式：2026-11-01)\n");
        if (!readword(borrowtime,sizeof(borrowtime)) || !validdate(borrowtime) ||
            dateculculate(borrowtime,todayis())<0 || !jisuan_guihuantime(borrowtime)[0])
        { printf("日期无效，借阅日期不能晚于今天。\n"); return; }
        strcpy(guihuantime,jisuan_guihuantime(borrowtime));
        printf("应归还日期为：%s\n",guihuantime);
        if (!saveborrow(selected.id,sid,peoplename,borrowtime,guihuantime,borrowcount))
        { printf("借阅失败，库存未更新。\n"); return; }
        printf("是否要继续借阅：(y/n):\n");
        if (!readword(choose,sizeof(choose))) return;
    } while (!strcmp(choose,"y"));
}

static int saveborrow1(char *bookname,char *sid,char *peoplename,char *borrowtime,char *guihuantime,int borrowcount)
{
    static Borrow tmp[MAX_BORROW_RECORDS]; Book selected; Borrow b; int count,i; long max=0,num; char *end;
    if (!recoverborrow() || !validtoken(sid,sizeof(b.sid)) || !validtoken(peoplename,sizeof(b.peoplename)) ||
        resolvebook(bookname,&selected)!=1 || borrowcount<=0 || borrowcount>selected.availableCopies ||
        !validdate(borrowtime) || !validdate(guihuantime) || dateculculate(borrowtime,todayis())<0 ||
        strcmp(guihuantime,jisuan_guihuantime(borrowtime)) || !sid[0] || !peoplename[0]) return 0;
    if (!readrecords(tmp,&count) || count>=MAX_BORROW_RECORDS || !checkstock(&selected,tmp,count)) return 0;
    for (i=0;i<count;i++)
    {
        if (tmp[i].borrowid[0]!='L') continue;
        errno=0; num=strtol(tmp[i].borrowid+1,&end,10);
        if (end==tmp[i].borrowid+1 || *end) continue;
        if (errno || num>=INT_MAX) return 0;
        if (num>max) max=num;
    }
    memset(&b,0,sizeof(b));
    snprintf(b.borrowid,sizeof(b.borrowid),"L%06ld",max+1);
    strcpy(b.bookname,selected.name); strcpy(b.bookid,selected.id);
    if (strlen(sid)>=sizeof(b.sid) || strlen(peoplename)>=sizeof(b.peoplename)) return 0;
    strcpy(b.sid,sid); strcpy(b.peoplename,peoplename);
    strcpy(b.borrowtime,borrowtime); strcpy(b.guihuantime,guihuantime);
    b.borrowcount=borrowcount; b.havetime=max_havetime; strcpy(b.status,"未归还");
    tmp[count++]=b;
    if (!preparebook(selected.id,-borrowcount,1) || !writerecords(tmp,count) || !commitborrow()) return 0;
    printf("已保存借阅记录：借阅编号：%s，书名：%s，未还数量：%d。\n",b.borrowid,b.bookname,b.borrowcount);
    remind(); return 1;
}

int saveborrow(char *bookname,char *sid,char *peoplename,char *borrowtime,char *guihuantime,int borrowcount)
{
    int result;
    lockdata();
    result=saveborrow1(bookname,sid,peoplename,borrowtime,guihuantime,borrowcount);
    unlockdata();
    return result;
}

// 用日历天数计算，不受夏令时或系统自动修正日期的影响。
static int daysnumber(const char *date)
{
    int y,m,d,i,days; int monthdays[]={31,28,31,30,31,30,31,31,30,31,30,31};
    sscanf(date,"%d-%d-%d",&y,&m,&d);
    days=(y-1)*365+(y-1)/4-(y-1)/100+(y-1)/400+d;
    if (y%400==0 || (y%4==0 && y%100!=0)) monthdays[1]++;
    for (i=0;i<m-1;i++) days+=monthdays[i];
    return days;
}
int dateculculate(char *borrowtime,char *guihuantime)
{
    if (!validdate(borrowtime) || !validdate(guihuantime)) return INT_MIN;
    return daysnumber(guihuantime)-daysnumber(borrowtime);
}
char *jisuan_guihuantime(char *borrowtime)
{
    static char guihuantime[15]; int y,m,d,i; int monthdays[]={31,28,31,30,31,30,31,31,30,31,30,31};
    guihuantime[0]=0; if (!validdate(borrowtime)) return guihuantime;
    sscanf(borrowtime,"%d-%d-%d",&y,&m,&d);
    for (i=0;i<max_havetime;i++)
    {
        monthdays[1]=(y%400==0 || (y%4==0 && y%100!=0)) ? 29 : 28;
        if (++d>monthdays[m-1]) { d=1; if (++m>12) { m=1; y++; } }
        if (y>9999) return guihuantime;
    }
    snprintf(guihuantime,sizeof(guihuantime),"%04d-%02d-%02d",y,m,d);
    return guihuantime;
}
int lookname(char *bookname) { Book b; return resolvebook(bookname,&b)==1; }
int lookcount(char *bookname) { Book b; return resolvebook(bookname,&b)==1 ? b.availableCopies : 0; }
