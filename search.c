#include <stdio.h>
#include <string.h>
#include "book.h"
int searchbook(void)
{
    char name[100];
    printf("请输入要搜索的书名: ");
    scanf("%99s", name);

    Book b;
    
    char line[512];
    FILE* fp=fopen("book.txt","r");
    if(fp==NULL)
    {
        printf("文件打开失败");
        return 0;
    }

    int found = 0;
    while(fgets(line,sizeof(line),fp)!=NULL)
    {
        sscanf(line,"%19s %99s %49s %d %d %d",b.id,b.name,b.leibie,&b.totalCopies,&b.availableCopies,&b.borrowCount);
          if(strcmp(b.name,name)==0)
          {
              printf("找到书籍:\n");
              printf("ID: %s\n", b.id);
              printf("书名: %s\n", b.name);
              printf("类别: %s\n", b.leibie);
              printf("总数量: %d\n", b.totalCopies);
              printf("可借数量: %d\n", b.availableCopies);
              printf("借阅次数: %d\n", b.borrowCount);
              found = 1;
              break;
          }
    }

    fclose(fp);

    if(!found)
        printf("未找到书籍: %s\n", name);

    return 0;
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
    */
