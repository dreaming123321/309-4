#include <stdio.h>
#include "borrow.h"
#include <string.h>          
#include "book.h"
int looktotal(char *bookname);
int saveguihuan(char *bookname,char *sid,char *peoplename,char *guihuantime1,int guihuancount);
void guihuan()
{
    char choose;char sid[20]={0};
    char bookname[100]; int guihuancount; char peoplename[100];char guihuantime1[15];
    printf("请输入归还书名：");
    scanf("%s", bookname);
    if(lookname(bookname)==0)
    {
        printf("该书不是我们这里的书,不要归还\n");
        return;
    }
    else if(looktotal(bookname)==0)
    {
        printf("该书已经满了,请检查一下是不是你还错图书馆了\n");
        return;
    }
    else
    {
        printf("确定你要归还%s\n",bookname);
        printf("是否归还?(y/n):\n ");
        scanf("%c", &choose);
        if(choose=='y')           //字符串用双引号，单个字符用单引号
        {
           printf("请输入归还数量：\n");
           scanf("%d",&guihuancount);
           printf("请输入归还人姓名：\n");
           scanf("%s",peoplename);
           printf("请输入归还人学号：\n");
           scanf("%s",sid);
           printf("请输入归还日期；(格式:2026-11-01)\n");        //尽量按照这个日期格式
           scanf("%s",guihuantime1);
           while(strlen(guihuantime1)!=10 || guihuantime1[4]!='-' || guihuantime1[7]!='-')
           {
            printf("你的日期格式输入错误,严格按照2026-11-01来写:");
            scanf("%s",guihuantime1);
           }       
           saveguihuan(bookname,sid,peoplename,guihuantime1,guihuancount);
        }
        else
        {
            return;
        }  
        saveguihuan(bookname,sid,peoplename,guihuantime1,guihuancount);           //写入借阅的borrow.txt文件
        updatebook(bookname,guihuancount);
    }
}

int saveguihuan(char *bookname,char *sid,char *peoplename,char *guihuantime1,int guihuancount)
{
    Borrow b;Borrow match[100];int count=0;int i=0;int choose;
    FILE *fp;char line[512];
    fp=fopen("borrow.txt","r+");
    if(fp==NULL)
    {
        printf("文件打开失败");
        return 0;
    }
    while(fgets(line,sizeof(line),fp)!=NULL)
    {
        sscanf(line,"%99s %19s %99s %14s %14s %d %d %9s",b.bookname,b.sid,b.peoplename,b.borrowtime,b.guihuantime,&b.borrowcount,&b.havetime,b.status);
        if(strcmp(b.sid,sid)==0 && strcmp(b.bookname,bookname)==0 && strcmp(b.status,"未归还")==0)
    {
        match[count]=b;
        match[count].havetime=dateculculate(b.borrowtime,guihuantime1);
        count++;
    }
    }
    printf("你有多次的记录与这个要归还的书匹配，选择一个进行归还：\n");
    for (i=0;i<count;i++)
    {
        printf("%d %s %s %s %s %d %d %s\n",i+1,match[i].bookname,match[i].sid,match[i].peoplename,match[i].borrowtime,match[i].havetime,match[i].status);
    }
    printf("请输入你要归还的记录序号：\n");
    
    scanf("%d",&choose);
    while(choose<1 || choose>count)
    {
        printf("你输入的序号有误，请重新输入\n");
        printf("请输入你要归还的记录序号：\n");
        scanf("%d",&choose);
    } 
    choose--;
    fclose(fp);
    match[choose].borrowcount-=guihuancount;
    if(match[choose].borrowcount==0)
    {
        strcpy(match[choose].status, "已归还");
        printf("%s在%s借的%s已全部归还\n",match[choose].peoplename,match[choose].borrowtime,match[choose].bookname);
    }
    else
    {
        printf("检查书本数量，日期是否正确\n");
    }
    return 0;
}