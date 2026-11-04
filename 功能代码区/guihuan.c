#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "book.h"
#include "borrow.h"
#include "guihuan.h"
#include "remind.h"

void guihuan()
{
    char choose[10], input[100], sid[20], peoplename[100], guihuantime1[15];
    int guihuancount; Book selected;
    do
    {
        printf("请输入归还图书编号（书名唯一时也可输入书名）：");
        if (!readword(input,sizeof(input))) { printf("输入无效。\n"); return; }
        int result=resolvebook(input,&selected);
        if (result!=1) { printf(result<0 ? "匹配不唯一，请输入图书编号。\n" : "未找到图书。\n"); return; }
        if (selected.totalCopies==selected.availableCopies)
        { printf("该书已经全部在馆，请检查图书编号。\n"); return; }
        printf("确定你要归还%s\n是否归还?(y/n):\n",selected.id);
        if (!readword(choose,sizeof(choose)) || strcmp(choose,"y")) return;
        printf("请输入归还数量：\n");
        if (!readnumber(&guihuancount) || guihuancount>selected.totalCopies-selected.availableCopies)
        { printf("归还数量无效或超过借出数量。\n"); return; }
        printf("请输入归还人姓名：\n");
        if (!readword(peoplename,sizeof(peoplename))) { printf("姓名无效。\n"); return; }
        printf("请输入归还人学号：\n");
        if (!readword(sid,sizeof(sid))) { printf("学号无效。\n"); return; }
        printf("请输入归还日期；(格式:2026-11-01)\n");
        if (!readword(guihuantime1,sizeof(guihuantime1)) || !validdate(guihuantime1) ||
            dateculculate(guihuantime1,todayis())<0)
        { printf("日期无效，归还日期不能晚于今天。\n"); return; }
        if (!saveguihuan(selected.id,sid,peoplename,guihuantime1,guihuancount))
        { printf("归还失败，库存未更新！\n"); return; }
        printf("是否需要继续归还：y/n:");
        if (!readword(choose,sizeof(choose))) return;
    } while (!strcmp(choose,"y"));
}

static int saveguihuan1(char *bookname,char *sid,char *peoplename,char *guihuantime1,int guihuancount)
{
    static Borrow tmp[MAX_BORROW_RECORDS]; static int matchIndex[MAX_BORROW_RECORDS];
    int count=0,matches=0,i,choose=1; Book selected,unique; Borrow *b; char chosenid[20];
    if (!recoverborrow() || !validtoken(sid,20) || !validtoken(peoplename,100) ||
        resolvebook(bookname,&selected)!=1 || guihuancount<=0 ||
        !validdate(guihuantime1) || dateculculate(guihuantime1,todayis())<0 ||
        !readrecords(tmp,&count) || !checkstock(&selected,tmp,count)) return 0;
    for (i=0;i<count;i++)
    {
        b=&tmp[i];
        if (strcmp(b->sid,sid) || strcmp(b->peoplename,peoplename) ||
            strcmp(b->bookname,selected.name) || strcmp(b->status,"未归还")) continue;
        if ((b->bookid[0] && !strcmp(b->bookid,selected.id)) ||
            (!b->bookid[0] && resolvebook(b->bookname,&unique)==1 && !strcmp(unique.id,selected.id)))
            matchIndex[matches++]=i;
    }
    if (!matches) { printf("没有匹配的未归还记录，请核对姓名、学号和图书编号。\n"); return 0; }
    if (matches>1)
    {
        printf("你有多次借阅记录，请选择一条归还：\n");
        for (i=0;i<matches;i++)
        {
            b=&tmp[matchIndex[i]];
            printf("%d %s %s %s %s %d %s\n",i+1,b->bookname,b->sid,b->peoplename,
                   b->borrowtime,b->borrowcount,b->status);
        }
        printf("请输入你要归还的记录序号：\n");
        { char text[32], *end; int ok; long number;
          unlockdata();
          ok=readword(text,sizeof(text));
          errno=0; number=ok ? strtol(text,&end,10) : 0;
          ok=ok && !errno && !*end && number>=1 && number<=matches;
          if (ok) choose=(int)number;
          lockdata();
          if (!ok) { printf("序号无效。\n"); return 0; }
          strcpy(chosenid,tmp[matchIndex[choose-1]].borrowid);
          if (!readrecords(tmp,&count) || !checkstock(&selected,tmp,count)) return 0;
          for (i=0;i<count;i++) if (!strcmp(tmp[i].borrowid,chosenid)) break;
          if (i==count || strcmp(tmp[i].status,"未归还") || strcmp(tmp[i].sid,sid) ||
              strcmp(tmp[i].peoplename,peoplename) || strcmp(tmp[i].bookname,selected.name)) return 0;
          matchIndex[choose-1]=i;
        }
    }
    b=&tmp[matchIndex[choose-1]];
    if (guihuancount>b->borrowcount || dateculculate(b->borrowtime,guihuantime1)<0)
    { printf("归还数量超过本条记录，或归还日期早于借书日期。\n"); return 0; }
    b->borrowcount-=guihuancount;
    b->havetime=dateculculate(b->borrowtime,guihuantime1);
    strcpy(b->bookid,selected.id);
    if (b->borrowcount==0) { strcpy(b->status,"已归还"); strcpy(b->actualtime,guihuantime1); }
    if (!preparebook(selected.id,guihuancount,0) || !writerecords(tmp,count) || !commitborrow()) return 0;
    printf(b->borrowcount ? "还有部分图书没有归还。\n" : "图书已全部归还。\n");
    remind(); return 1;
}

int looktotal(char *bookname)
{
    Book b; return resolvebook(bookname,&b)==1 ? b.totalCopies-b.availableCopies : 0;
}

int saveguihuan(char *bookname,char *sid,char *peoplename,char *guihuantime1,int guihuancount)
{
    int result;
    lockdata();
    result=saveguihuan1(bookname,sid,peoplename,guihuantime1,guihuancount);
    unlockdata();
    return result;
}
