#include <stdio.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include "book.h"
#include "borrow.h"
#include "remind.h"

void remindlook()
{
    Borrow b; FILE *fp; char line[512];
    remind();
    fp=fopen("remind.txt","r");
    if (!fp) { printf("无法读取提醒文件。\n"); return; }
    while (fgets(line,sizeof(line),fp))
    {
        if (sscanf(line,"%19s %99s %19s %99s %14s %14s %d %d %9s",b.borrowid,b.bookname,b.sid,
                   b.peoplename,b.borrowtime,b.guihuantime,&b.borrowcount,&b.havetime,b.status)!=9) continue;
        printf("借阅编号：%s  书名：%s  学号：%s  姓名：%s  借阅时间：%s  到期时间：%s  未还数量：%d  状态：%s\n",
               b.borrowid,b.bookname,b.sid,b.peoplename,b.borrowtime,b.guihuantime,b.borrowcount,b.status);
    }
    fclose(fp);
}

// 接收今天的日期，方便检查提醒和三天保留期的边界。
int remindat(char *today)
{
    static Borrow tmp[MAX_BORROW_RECORDS]; int count=0,i,j=0,changed=0,error; FILE *fp;
    if (!recoverborrow() || !validdate(today) || !readrecords(tmp,&count)) return 0;
    for (i=0;i<count;i++)
    {
        if (!strcmp(tmp[i].status,"已归还") && tmp[i].actualtime[0] &&
            dateculculate(tmp[i].actualtime,today)>=3)
        { changed=1; continue; }
        if (j!=i) tmp[j]=tmp[i];
        j++;
    }
    if (changed)
    {
        if (!writerecords(tmp,j) || !MoveFileExA("borrowtemp.txt","borrow.txt",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
        { printf("已归还历史清理失败，原记录保留。\n"); return 0; }
    }
    fp=fopen("remindtemp.txt","w"); if (!fp) return 0;
    for (i=0;i<j;i++)
    {
        Borrow *b=&tmp[i];
        if (!strcmp(b->status,"未归还") && b->borrowcount>0 && dateculculate(today,b->guihuantime)<=7)
        {
            fprintf(fp,"%s %s %s %s %s %s %d %d %s",b->borrowid,b->bookname,b->sid,b->peoplename,
                    b->borrowtime,b->guihuantime,b->borrowcount,b->havetime,b->status);
            if (b->bookid[0]) fprintf(fp," %s",b->bookid);
            fputc('\n',fp);
        }
    }
    error=ferror(fp); if (fclose(fp)!=0) error=1;
    if (error || !MoveFileExA("remindtemp.txt","remind.txt",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
    { printf("提醒更新失败，请检查文件。\n"); return 0; }
    return 1;
}
void remind() { remindat(todayis()); }
int timecha(char *guihuantime1) { return dateculculate(todayis(),guihuantime1); }
char *todayis()
{
    static _Thread_local char today[15]; time_t now=time(NULL); struct tm t;
    if (localtime_s(&t,&now)!=0 || !strftime(today,sizeof(today),"%Y-%m-%d",&t)) today[0]=0;
    return today;
}
