#include <stdio.h>
#include <string.h>
#include "book.h"
int addbook(void)
{
    char name[100];
    char leibie[50];
    int count;
   
    
    printf("请输入书名：");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;
    printf("请输入类别：");
    fgets(leibie, sizeof(leibie), stdin);
    leibie[strcspn(leibie, "\n")] = 0;
    printf("请输入数量：");
    scanf("%d", &count);
    FILE *fp = fopen("book.txt", "a");
    fprintf(fp, "\n%s %s %d\n", name, leibie, count);
    fclose(fp);
    printf("添加成功！\n");
    return 0;
}



/*int main() {
    addbook();
    return 0;
}*/
