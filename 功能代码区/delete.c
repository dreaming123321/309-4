#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "book.h"

int deletebook(void)
{
    char bookname[100];
    char line[512];
    int deleted=0;
    FILE *bookfile;
    FILE *tempfile;
    Book b,selected;

    printf("请输入要删除的图书编号（书名唯一时也可输入书名）：");
    if(scanf("%99s",bookname)!=1) return 0;
    bookfile=fopen("book.txt","r");
    if(bookfile==NULL) { printf("文件打开失败\n"); return 0; }
    while(fgets(line,sizeof(line),bookfile)!=NULL)
    {
        if(strspn(line," \t\r\n")==strlen(line)) continue;
        if(sscanf(line,"%19s %99s %49s %f %d %d %d",b.id,b.name,b.leibie,&b.price,&b.totalCopies,&b.availableCopies,&b.borrowCount)!=7)
        { fclose(bookfile); printf("图书数据格式错误，未执行删除。\n"); return 0; }
        if(strcmp(b.id,bookname)==0 || strcmp(b.name,bookname)==0)
        {
            selected=b;
            deleted++;
        }
    }
    int failed=ferror(bookfile);
    fclose(bookfile);
    if(failed) { printf("图书读取失败。\n"); return 0; }
    if(deleted==0) { printf("未找到该图书。\n"); return 0; }
    if(deleted>1) { printf("存在多条同名或重复编号记录，请先查询并输入唯一图书编号。\n"); return 0; }
    if(selected.availableCopies!=selected.totalCopies)
    { printf("该书仍有借出库存或库存数据不一致，不能删除。\n"); return 0; }

    FILE *borrowfile=fopen("borrow.txt","r");
    if(borrowfile==NULL) { printf("无法检查借阅记录，未执行删除。\n"); return 0; }
    Borrow a;
    while(fgets(line,sizeof(line),borrowfile)!=NULL)
    {
        if(strspn(line," \t\r\n")==strlen(line)) continue;
        a.bookid[0]=0;
        if(sscanf(line,"%19s %99s %19s %99s %14s %14s %d %d %9s %19s",a.borrowid,a.bookname,a.sid,a.peoplename,a.borrowtime,a.guihuantime,&a.borrowcount,&a.havetime,a.status,a.bookid)<9)
        { fclose(borrowfile); printf("借阅数据格式错误，未执行删除。\n"); return 0; }
        if((a.bookid[0] ? strcmp(a.bookid,selected.id)==0 : strcmp(a.bookname,selected.name)==0) &&
           (a.borrowcount>0 || strcmp(a.status,"已归还")!=0))
        { fclose(borrowfile); printf("该书存在未归还记录，不能删除；旧同名记录没有编号时需先核实归属。\n"); return 0; }
    }
    failed=ferror(borrowfile);
    fclose(borrowfile);
    if(failed) { printf("借阅记录读取失败，未执行删除。\n"); return 0; }
    char choose;
    printf("确定删除 %s %s 吗？(y/n):",selected.id,selected.name);
    if(scanf(" %c",&choose)!=1 || choose!='y') { printf("已取消删除。\n"); return 0; }

    bookfile=fopen("book.txt","r");
    tempfile=fopen("temp.txt","w");
    if(bookfile==NULL || tempfile==NULL)
    { if(bookfile) fclose(bookfile); if(tempfile) fclose(tempfile); printf("文件打开失败\n"); return 0; }
    while(fgets(line,sizeof(line),bookfile)!=NULL)
    {
        if(sscanf(line,"%19s",b.id)==1 && strcmp(b.id,selected.id)==0) continue;
        fputs(line,tempfile);
    }
    failed=ferror(bookfile) || ferror(tempfile);
    fclose(bookfile);
    if(fclose(tempfile)!=0) failed=1;
    if(failed || !MoveFileExA("temp.txt","book.txt",MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    { printf("保存失败，原书库未替换。\n"); return 0; }
    printf("书籍已成功删除\n");
    return 1;
}
