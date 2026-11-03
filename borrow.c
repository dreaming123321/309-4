#include <stdio.h>
#include <time.h>
#include <string.h>          //后面的strlen需要用到
#include "book.h"
#define max_havetime 30        //规定借阅时间上限是30天
int lookname(char *bookname);  //这个函数专门根据书名查是否有这本书，如果有，返回1，如果没有，返回0
int lookcount(char *bookname);  //这个函数专门根据书名查这本书的可借数量，如果有，返回数量，如果没有，返回0
void updatebook(char *bookname,int change);        //功能：这个是修改book.txt文件
void saveborrow(char *bookname,char *sid,char *peoplename,char *borrowtime,char *guihuantime,int borrowcount);      //保存借阅记录的函数
int dateculculate(char *borrowtime,char *guihuantime);  //传入起始时间和最终时间，这个函数可以计算出经过的天数
char *jisuan_guihuantime(char *borrowtime);  //这个函数是根据借阅时间计算归还时间，归还时间是借阅时间加上30天
void borrow()
{
    char choose;
    char bookname[100]={0}; int borrowcount=0; char peoplename[100]={0};char borrowtime[15]={0};char guihuantime[100]={0};char sid[20]={0};
    printf("请输入借阅书名：");
    scanf("%s", bookname);
    if(lookname(bookname)==0)
    {
        printf("该书不存在,无法借阅");
        return;
    }
    else if(lookcount(bookname)==0)
    {
        printf("该书已被借完,无法借阅\n");
        return;
    }
    else
    {
        printf("可借阅数量为：%d\n",lookcount(bookname));
        printf("是否借阅?(y/n): ");
        scanf(" %c", &choose);
        if(choose=='y')           //字符串用双引号，单个字符用单引号
        {
           printf("请输入借阅数量：\n");
           scanf("%d",&borrowcount);
           if(borrowcount>lookcount(bookname))
           {
            printf("借阅数量不能大于可借数量");
           }
           printf("请输入借阅人姓名：\n");
           scanf("%s",peoplename);
           printf("请输入借阅人学号：%s\n",sid);
           scanf("%s",sid);
           if(looksid1(sid)==1)
           {
            printf("你已经借过书了并且没有归还,先还书，再借书");
            return;
           }
           printf("请输入借阅日期；(格式：2026-11-01)\n");        //尽量按照这个日期格式
           scanf("%s",borrowtime);
           while(strlen(borrowtime)!=10 || borrowtime[4]!='-' || borrowtime[7]!='-')
           {
            printf("你的日期格式输入错误,严格按照2026-11-01来写:");
            scanf("%s",borrowtime);
           }
           strcpy(guihuantime,jisuan_guihuantime(borrowtime));        //strcpy(A,B)将B的内容复制到A中
           printf("应归还日期为：%s\n",guihuantime);
        }
        else
        {
            return;
        }  
        saveborrow(bookname,sid,peoplename,borrowtime,guihuantime,borrowcount);           //写入借阅的borrow.txt文件
        updatebook(bookname,borrowcount);
    }
 
}
void saveborrow(char *bookname,char *sid,char *peoplename,char *borrowtime,char *guihuantime,int borrowcount)
{
    int havetime=0;char status[10]="未归还";
    havetime=dateculculate(borrowtime,guihuantime);
    FILE *fp;
    fp=fopen("borrow.txt","a");
    if (fp==NULL)
    {
        printf("文件打开失败");
        return;
    }
    fprintf(fp,"%s  %s %s  %s %s %d %d %s\n",bookname,sid,peoplename,borrowtime,guihuantime,borrowcount,havetime,status);
    printf("已保存借阅记录：书名：%s  借阅人学号：%s  借阅人姓名：%s 借阅时间：%s 归还时间：%s 借书数目：%d 可借阅%d天 状态:%s\n",bookname,sid,peoplename,borrowtime,guihuantime,borrowcount,havetime,status);
    fclose(fp);
}
int dateculculate(char *borrowtime,char *guihuantime)
{
    int year1,month1,date1,year2,month2,date2;
    struct tm a={0};
    struct tm b={0};
    sscanf(borrowtime,"%d-%d-%d",&year1,&month1,&date1);
    sscanf(guihuantime,"%d-%d-%d",&year2,&month2,&date2);       //sscanf()是将字符串提取里面得数字类型数据
    a.tm_year=year1-1900;   
    a.tm_mon=month1-1;
    a.tm_mday=date1;
    b.tm_year=year2-1900;
    b.tm_mon=month2-1;                            //tm结构体是将之前提取得数字类型得数据赋予他们时间含义
    b.tm_mday=date2;
    time_t a1=mktime(&a);                    //mktime()函数是将结构体tm转换为时间戳（也就是一个数字来进行计算）
    time_t b1=mktime(&b);
    return (int)difftime(b1,a1)/86400;       //一天有86400秒

}
char *jisuan_guihuantime(char *borrowtime)                //规定归还时间上限是30天吧,这个函数就是dateculculate()的逆运算，传入借阅时间，返回归还时间
{
    static  char guihuantime[100];
    struct tm a={0};
    int year1,month1,date1;
    sscanf(borrowtime,"%d-%d-%d",&year1,&month1,&date1);
    a.tm_year=year1-1900;   
    a.tm_mon=month1-1;
    a.tm_mday=date1;
    a.tm_mday+=max_havetime;
    mktime(&a);
    sprintf(guihuantime,"%04d-%02d-%02d",a.tm_year+1900,a.tm_mon+1,a.tm_mday);
    return guihuantime;
}
void updatebook(char *bookname,int change)   //change 为正的话代表借出去
{
    Book b;int i=0,count=0;Book book[100];
    FILE *fp;
    char line[512];
    fp=fopen("book.txt","r");
    if(fp==NULL)
    {
        printf("文件打开失败");
        return;
    }
    while(fgets(line,sizeof(line),fp)!=NULL)
    {
        sscanf(line,"%19s %99s %49s %d %d %d",b.id,b.name,b.leibie,&b.totalCopies,&b.availableCopies,&b.borrowCount);
        book[i]=b;
        if(strcmp(b.name,bookname)==0)
    {
        b.availableCopies-=change;
        b.borrowCount+=1;
        book[i]=b;
        
    }
        i++;
        count++;
    }
    printf("获取了%d本图书信息,对其进行更新ing...\n",count);
    fclose(fp);
    FILE *fp2;
    fp2=fopen("booktemp.txt","w");
    if (fp2==NULL)
    {
        printf("文件打开失败");
        return;
    }
    for(i=0;i<count;i++)
    {
        fprintf(fp2,"%s %s %s %d %d %d\n",book[i].id,book[i].name,book[i].leibie,book[i].totalCopies,book[i].availableCopies,book[i].borrowCount);
    }
    fclose(fp2);
    remove("book.txt");
    printf("已删除book.txt文件\n");
    rename("booktemp.txt","book.txt");
    printf("已重命名booktemp.txt文件为book.txt文件\n");
    printf("book.txt文件已更新\n");
    return;
  

}
int lookname(char *bookname)
{
    Book b;
    FILE *fp;char line[512];
    fp=fopen("book.txt","r");
    if(fp==NULL)
    {
        printf("文件打开失败");
        return 0;
    }
    while(fgets(line,sizeof(line),fp)!=NULL)
    {
        sscanf(line,"%19s %99s %49s %d %d %d",b.id,b.name,b.leibie,&b.totalCopies,&b.availableCopies,&b.borrowCount);
          if(strcmp(b.name,bookname)==0)
    {
        fclose(fp);
        return 1;
    }
    }
    fclose(fp);
    return 0;
}


int lookcount(char *bookname)
{
    Book b;
    FILE *fp;char line[512];
    fp=fopen("book.txt","r");
    if(fp==NULL)
    {
        printf("文件打开失败");
        return 0;
    }
    while(fgets(line,sizeof(line),fp)!=NULL)
    {
        sscanf(line,"%19s %99s %49s %d %d %d",b.id,b.name,b.leibie,&b.totalCopies,&b.availableCopies,&b.borrowCount);
          if(strcmp(b.name,bookname)==0)
    {
        fclose(fp);
        return b.availableCopies;
    }
    }
    fclose(fp);
    return 0;
}
int looksid1(char *sid)
{
    Borrow b;
    FILE *fp;char line[512];
    fp=fopen("borrow.txt","r");
    if(fp==NULL)
    {
        printf("文件打开失败");
        return 0;
    }
    while(fgets(line,sizeof(line),fp)!=NULL)
    {
        sscanf(line,"%99s %19s %99s %14s %14s %d %d",b.bookname,b.sid,b.peoplename,b.borrowtime,b.guihuantime,&b.borrowcount,&b.havetime,b.status);
        if(strcmp(b.sid,sid)==0 && strcmp(b.status,"未归还")==0)
    {
        fclose(fp);
        return 1;
    }
    }
    fclose(fp);
    return 0;
}



