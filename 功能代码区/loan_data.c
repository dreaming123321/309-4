#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <windows.h>
#include "borrow.h"

static INIT_ONCE lockOnce=INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION dataLock;
static BOOL CALLBACK initlock(PINIT_ONCE once, PVOID param, PVOID *context)
{ (void)once; (void)param; (void)context; InitializeCriticalSection(&dataLock); return TRUE; }
void lockdata(void) { InitOnceExecuteOnce(&lockOnce,initlock,NULL,NULL); EnterCriticalSection(&dataLock); }
void unlockdata(void) { LeaveCriticalSection(&dataLock); }

int validtoken(const char *value, size_t size)
{
    size_t i, len=strlen(value);
    if (!len || len>=size) return 0;
    for (i=0;i<len;i++) if (isspace((unsigned char)value[i])) return 0;
    return 1;
}

// 借还时先检查整行输入，避免多余字符留在输入缓冲区。
int readword(char *value, size_t size)
{
    char line[512]; size_t len, i;
    if (!fgets(line, sizeof(line), stdin)) return 0;
    len = strlen(line);
    if (len && line[len-1] == '\n') line[--len] = '\0';
    else if (!feof(stdin)) { int c; while ((c=getchar())!='\n' && c!=EOF) {} return 0; }
    if (len && line[len-1] == '\r') line[--len] = '\0';
    if (!len || len >= size) return 0;
    for (i=0; i<len; i++) if (isspace((unsigned char)line[i])) return 0;
    strcpy(value, line);
    return 1;
}

int readnumber(int *value)
{
    char line[32], *end; long n;
    if (!readword(line, sizeof(line))) return 0;
    n = strtol(line, &end, 10);
    if (*end || n < 1 || n > MAX_COPIES) return 0;
    *value = (int)n;
    return 1;
}

// 日期必须是真实存在的日历日期，不让 mktime 自动修正错误日期。
int validdate(const char *date)
{
    int y,m,d,days; int i;
    if (strlen(date)!=10 || date[4]!='-' || date[7]!='-') return 0;
    for (i=0; i<10; i++) if (i!=4 && i!=7 && !isdigit((unsigned char)date[i])) return 0;
    if (sscanf(date,"%d-%d-%d",&y,&m,&d)!=3 || y<1900 || y>9999 || m<1 || m>12) return 0;
    { int monthdays[]={31,28,31,30,31,30,31,31,30,31,30,31};
      days=monthdays[m-1]; if (m==2 && (y%400==0 || (y%4==0 && y%100!=0))) days++; }
    return d>=1 && d<=days;
}

// 借阅文件兼容旧的 9/10 字段，11 字段为实际还清日期。
int readrecords(Borrow *records, int *count)
{
    FILE *fp=fopen("borrow.txt","r"); char line[512], extra[2], amount[32], days[32], *end;
    Borrow b; int fields, i; long value;
    *count=0;
    if (!fp) { printf("无法读取借阅记录。\n"); return 0; }
    while (fgets(line,sizeof(line),fp))
    {
        if (*count>=MAX_BORROW_RECORDS || (!strchr(line,'\n') && !feof(fp))) { fclose(fp); return 0; }
        memset(&b,0,sizeof(b));
        fields=sscanf(line,"%19s %99s %19s %99s %14s %14s %31s %31s %9s %19s %14s %1s",
            b.borrowid,b.bookname,b.sid,b.peoplename,b.borrowtime,b.guihuantime,
            amount,days,b.status,b.bookid,b.actualtime,extra);
        if (fields<9 || fields>11) { fclose(fp); printf("借阅记录格式错误。\n"); return 0; }
        errno=0; value=strtol(amount,&end,10);
        if (errno || *end || value<0 || value>MAX_COPIES) { fclose(fp); return 0; }
        b.borrowcount=(int)value;
        errno=0; value=strtol(days,&end,10);
        if (errno || *end || value<INT_MIN || value>INT_MAX) { fclose(fp); return 0; }
        b.havetime=(int)value;
        for (i=0;i<*count;i++) if (!strcmp(records[i].borrowid,b.borrowid))
        { fclose(fp); printf("借阅编号重复，未修改数据。\n"); return 0; }
        if (fields<9 || fields>11 || !validdate(b.borrowtime) || !validdate(b.guihuantime) ||
            b.borrowcount<0 || b.borrowcount>MAX_COPIES ||
            (strcmp(b.status,"未归还") && strcmp(b.status,"已归还")) ||
            (!strcmp(b.status,"未归还") && (b.borrowcount==0 || fields==11)) ||
            (!strcmp(b.status,"已归还") && b.borrowcount!=0) ||
            (fields==11 && (!validdate(b.actualtime) || dateculculate(b.borrowtime,b.actualtime)<0)))
        { fclose(fp); printf("借阅记录格式错误，未修改数据。\n"); return 0; }
        records[(*count)++]=b;
    }
    if (ferror(fp)) { fclose(fp); return 0; }
    fclose(fp); return 1;
}

int writerecords(Borrow *records, int count)
{
    FILE *fp=fopen("borrowtemp.txt","w"); int i;
    if (!fp) return 0;
    for (i=0;i<count;i++)
    {
        Borrow *b=&records[i];
        fprintf(fp,"%s %s %s %s %s %s %d %d %s",b->borrowid,b->bookname,b->sid,b->peoplename,
                b->borrowtime,b->guihuantime,b->borrowcount,b->havetime,b->status);
        if (b->bookid[0]) fprintf(fp," %s",b->bookid);
        if (b->actualtime[0]) fprintf(fp," %s",b->actualtime);
        fputc('\n',fp);
    }
    i=ferror(fp); if (fclose(fp)!=0) i=1;
    if (i) { remove("borrowtemp.txt"); return 0; }
    return 1;
}

// change 为库存变化，借书扣减，归还增加。
int preparebook(const char *id, int change, int borrowing)
{
    FILE *fp=fopen("book.txt","r"), *out; char line[512],extra[2]; Book b; int found=0, error=0;
    if (!fp) return 0;
    out=fopen("booktemp.txt","w"); if (!out) { fclose(fp); return 0; }
    while (fgets(line,sizeof(line),fp))
    {
        if ((!strchr(line,'\n') && !feof(fp)) ||
            sscanf(line,"%19s %99s %49s %f %d %d %d %1s",b.id,b.name,b.leibie,&b.price,
                   &b.totalCopies,&b.availableCopies,&b.borrowCount,extra)!=7 ||
            b.totalCopies<1 || b.totalCopies>MAX_COPIES || b.availableCopies<0 ||
            b.availableCopies>b.totalCopies || b.borrowCount<0)
        { error=1; break; }
        if (!strcmp(b.id,id))
        {
            found++;
            if (b.availableCopies+change<0 || b.availableCopies+change>b.totalCopies ||
                (borrowing && b.borrowCount==INT_MAX)) { error=1; break; }
            b.availableCopies+=change; if (borrowing) b.borrowCount++;
            fprintf(out,"%s %s %s %.2f %d %d %d\n",b.id,b.name,b.leibie,b.price,
                    b.totalCopies,b.availableCopies,b.borrowCount);
        }
        else fputs(line,out);
    }
    if (ferror(fp) || ferror(out)) error=1;
    fclose(fp); if (fclose(out)!=0) error=1;
    if (error || found!=1) { remove("booktemp.txt"); printf("库存数据无效，未执行借还。\n"); return 0; }
    return 1;
}

// 有标记说明上次替换两个文件时中断，先恢复替换前的两个文件。
int recoverborrow(void)
{
    FILE *fp=fopen("borrow.pending","r");
    if (!fp)
    {
        DWORD attributes=GetFileAttributesA("borrow.pending");
        if (attributes==INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND) return 1;
        printf("无法检查借还恢复标记。\n"); return 0;
    }
    fclose(fp);
    if (!CopyFileA("book.backup","book.txt",FALSE) || !CopyFileA("borrow.backup","borrow.txt",FALSE))
    { printf("借还数据恢复失败，请保留 backup 和 pending 文件并检查。\n"); return 0; }
    if (remove("borrow.pending")!=0) return 0;
    remove("book.backup"); remove("borrow.backup");
    return 1;
}

int commitborrow(void)
{
    FILE *fp;
    if (!CopyFileA("book.txt","book.backup",FALSE) || !CopyFileA("borrow.txt","borrow.backup",FALSE))
    { printf("无法备份借还数据。\n"); return 0; }
    fp=fopen("borrow.pending","w");
    if (!fp) return 0;
    { int error=(fputs("借还正在保存\n",fp)==EOF); if (fclose(fp)!=0) error=1;
      if (error) { remove("borrow.pending"); return 0; } }
    if (!MoveFileExA("booktemp.txt","book.txt",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH) ||
        !MoveFileExA("borrowtemp.txt","borrow.txt",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
    { printf("借还保存失败，正在恢复原数据。\n"); recoverborrow(); return 0; }
    if (remove("borrow.pending")!=0) { recoverborrow(); return 0; }
    remove("book.backup"); remove("borrow.backup");
    return 1;
}

// 库存与未还记录对不上时先停止操作，避免继续扩大差错。
int checkstock(Book *book, Borrow *records, int count)
{
    int i, sum=0; Book unique;
    for (i=0;i<count;i++)
    {
        Borrow *b=&records[i];
        if (strcmp(b->status,"未归还")) continue;
        if (b->bookid[0])
        {
            if (strcmp(b->bookid,book->id)) continue;
            if (strcmp(b->bookname,book->name))
            { printf("借阅记录的编号和书名不一致，请先核对。\n"); return 0; }
        }
        else
        {
            if (strcmp(b->bookname,book->name)) continue;
            if (resolvebook(b->bookname,&unique)!=1 || strcmp(unique.id,book->id))
            { printf("旧借阅记录没有编号，同名图书无法确定校区，请先核对。\n"); return 0; }
        }
        sum+=b->borrowcount;
    }
    if (sum!=book->totalCopies-book->availableCopies)
    { printf("库存和未归还记录数量不一致，请先核对。\n"); return 0; }
    return 1;
}
