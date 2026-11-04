#include <stdio.h>
#include <string.h>
#include "book.h"
int deletebook(void)
{
    char bookname[100];
    char line[200];
    int deleted = 0;
    
    FILE *bookfile;
    FILE *tempfile;

    printf("请输入要删除的书名：");
    scanf("%99s", bookname);

    bookfile = fopen("book.txt", "r");
    tempfile = fopen("temp.txt", "w");
    
    Book b;
    if(bookfile==NULL)
    {
        printf("文件打开失败");
        return 0;
    }
    while(fgets(line,sizeof(line),bookfile)!=NULL)
    {
        sscanf(line,"%19s %99s %49s %f %d %d %d",b.id,b.name,b.leibie,&b.price,&b.totalCopies,&b.availableCopies,&b.borrowCount);
          if(strcmp(b.name,bookname)==0)
    {
        deleted = 1;
        continue;
    }
        fputs(line, tempfile);
    }
    fclose(bookfile);
    fclose(tempfile);

    if(deleted)
    {
        remove("book.txt");
        rename("temp.txt", "book.txt");
        printf("书籍已成功删除\n");
    }

}





/*Book b;
   
    char line[512];
    bookfile=fopen("book.txt","r");
    if(bookfile==NULL)
    {
        printf("文件打开失败");
        return 0;
    }
    if(fgets(line,sizeof(line),bookfile)!=NULL)
    {
        sscanf(line,"%19s %99s %49s %d %d %d",b.id,b.name,b.leibie,&b.totalCopies,&b.availableCopies,&b.borrowCount);
          if(strcmp(b.name,bookname)==0)
    {
        deleted = 1;
        continue;
    }
        fputs(line, tempfile);
    }*/