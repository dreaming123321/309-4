#include <stdio.h>
#include "borrow.h"
#include "borrow.h"
#include "book.h"
#include <time.h>
#include <string.h>
int timecha(char *guihuantime1);
char *todayis();
void remind();
void remindlook();

void remindlook()
{
    Borrow b;
    FILE *fp;
    Borrow tmp[100];int j=0;
    char line1[512];
    fp=fopen("remind.txt","r");
    if(fp==NULL)
    {
        printf("文件打开失败");
        return;
    }
    while(fgets(line1,sizeof(line1),fp)!=NULL)
    {
        sscanf(line1,"%19s %99s %19s %99s %14s %14s %d %d %9s",b.borrowid,b.bookname,b.sid,b.peoplename,b.borrowtime,b.guihuantime,&b.borrowcount,&b.havetime,b.status);
        printf("借阅编号：%s  书名：%s  学号：%s  姓名：%s  借阅时间：%s  到期时间：%s  借阅次数：%d  已归还次数：%d  状态：%s\n",b.borrowid,b.bookname,b.sid,b.peoplename,b.borrowtime,b.guihuantime,b.borrowcount,b.havetime,b.status);
    }
    fclose(fp);
}
void remind()
{
    Borrow b;
    FILE *fp;
    Borrow tmp[100];int j=0;
    char line1[512];
    fp=fopen("borrow.txt","r");
    if(fp==NULL)
    {
        printf("文件打开失败");
        return;
    }
    while(fgets(line1,sizeof(line1),fp)!=NULL)
    {
        sscanf(line1,"%19s %99s %19s %99s %14s %14s %d %d %9s",b.borrowid,b.bookname,b.sid,b.peoplename,b.borrowtime,b.guihuantime,&b.borrowcount,&b.havetime,b.status);
        if(strcmp(b.status,"已归还")!=0 && timecha(b.guihuantime)<=7 && b.borrowcount>0)
        tmp[j]=b;
        j++;
    }
    fclose(fp);
    int i=0;
    FILE *fp2;
    fp2=fopen("remind.txt","w+");
    if(fp2==NULL)
    {
        printf("文件打开失败");
        return;
    }
    for (i=0;i<j;i++)
    {
        fprintf(fp2,"%s %s %s %s %s %s %d %d %s\n",tmp[i].borrowid,tmp[i].bookname,tmp[i].sid,tmp[i].peoplename,tmp[i].borrowtime,tmp[i].guihuantime,tmp[i].borrowcount,tmp[i].havetime,tmp[i].status);
    }
    fclose(fp2);
}
int timecha(char *guihuantime1)                   //传入归还时间可求得距离归还时间还有多少天的时间
{
    char today1[15]={0};int timecha=0;
    strcpy(today1,todayis());
    timecha=dateculculate(today1,guihuantime1);
    return timecha;
    
}
char *todayis()
{
    static char today[15]={0};
    time_t now=time(NULL);         //time():获取今日时间戳（就是从1900年到现在过去了多少秒，因为数值太大，专门设置了一个time_t类型）
    struct tm *t=localtime(&now);   //localtime():将时间戳转换为本地时间结构体
    strftime(today,sizeof(today),"%Y-%m-%d",t);      //将时间结构体转为自定义格式的时间表示
    return today;
}