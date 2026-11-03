#include <stdio.h>
#include "borrow.h"
#include <time.h>
#include <string.h>          //后面的strlen需要用到
#include "book.h"
int looktotal(char *bookname);
void guihuan()
{
    char choose;
    char bookname[100]; int borrowcount; char peoplename[100];char borrowtime[15];char guihuantime[100];
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
        printf("是否归还?(y/n): ");
        scanf("%c", &choose);
        if(choose=='y')           //字符串用双引号，单个字符用单引号
        {
           printf("请输入归还数量：\n");
           scanf("%d",&borrowcount);
           printf("请输入归还人姓名：\n");
           scanf("%s",peoplename);
           printf("请输入借阅日期；(格式：2026-11-01)\n");        //尽量按照这个日期格式
           scanf("%s",borrowtime);
           while(strlen(borrowtime)!=10 || borrowtime[4]!='-' || borrowtime[7]!='-')
           {
            printf("你的日期格式输入错误,严格按照2026-11-01来写:");
            scanf("%s",borrowtime);
           }
           strcpy(guihuantime,jisuan_guihuantime(borrowtime));        //strcpy(A,B)将B的内容复制到A中
           printf("归还日期为：%s\n",guihuantime);
        }
        else
        {
            return;
        }  
        saveborrow(bookname,peoplename,borrowtime,guihuantime);           //写入借阅的borrow.txt文件
        updatebook(bookname,borrowcount);
    }
 
}