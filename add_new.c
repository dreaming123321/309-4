#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "add_new.h"
#include "book.h"

void add_new_book(void)
{
    char id[20];
    char name[100];
    char leibie[50];
    int total;
    int available;
    int borrow = 0; // 初始借阅次数为 0

    // 1. 自动生成索引号
    int book_count = 0;
    FILE *fp_read = fopen("book.txt", "r");
    if (fp_read != NULL) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), fp_read) != NULL) {
            if (strlen(buffer) > 5) { // 排除空行
                book_count++;
            }
        }
        fclose(fp_read);
    }
    
    // 生成带 XL- 前缀的编号，比如 XL-B006
    sprintf(id, "XL-B%03d", book_count + 1);

    // 2. 用户输入
    printf("\n--- 添加新书 ---\n");
    
    printf("请输入书名: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0; // 清除换行符

    printf("请输入类别: ");
    fgets(leibie, sizeof(leibie), stdin);
    leibie[strcspn(leibie, "\n")] = 0; // 清除换行符

    printf("请输入总数: ");
    char totalStr[20];
    fgets(totalStr, sizeof(totalStr), stdin);
    total = atoi(totalStr); // 字符串转整数

    // 3. 自动计算
    available = total; // 新书可借数量 = 总数量

    // 4. 写入文件（纯空格格式）
    FILE *fp_write = fopen("book.txt", "a");
    if (fp_write == NULL) {
        printf("文件打开失败！\n");
        return;
    }

    // 【修改】按照纯空格短格式写入
    fprintf(fp_write, "%s %s %s %d %d %d\n", id, name, leibie, total, available, borrow);
            
    fclose(fp_write);
    printf("\n添加成功！系统自动分配编号: %s\n", id);
}