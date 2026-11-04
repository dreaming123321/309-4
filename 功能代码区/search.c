#include <stdio.h>
#include <string.h>
#include "book.h"
int searchbook(void)
{
    char name[100];
    printf("请输入书名、图书编号、类别或校区关键词: ");
    if(scanf("%99s", name)!=1) return 0;

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
        if(sscanf(line,"%19s %99s %49s %f %d %d %d",b.id,b.name,b.leibie,&b.price,&b.totalCopies,&b.availableCopies,&b.borrowCount)!=7) continue;
        char *campus=strncmp(b.id,"XL",2)==0 ? "仙林" : strncmp(b.id,"PL",2)==0 ? "三牌楼" : "未知";
          if(strstr(b.name,name)!=NULL || strstr(b.id,name)!=NULL || strstr(b.leibie,name)!=NULL || strstr(campus,name)!=NULL)
          {
              printf("找到书籍:\n");
              printf("ID: %s\n", b.id);
              printf("书名: %s\n", b.name);
              printf("校区: %s\n", campus);
              printf("类别: %s\n", b.leibie);
              printf("价格: %.2f\n", b.price);
              printf("总数量: %d\n", b.totalCopies);
              printf("可借数量: %d\n", b.availableCopies);
              printf("借阅次数: %d\n", b.borrowCount);
              found++;
          }
    }

    fclose(fp);

    if(!found)
        printf("未找到书籍: %s\n", name);

    if(found) printf("共找到 %d 条图书记录，请用编号区分同名图书。\n",found);
    return found;
}
