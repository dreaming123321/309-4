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

    // 1. 选择校区
    char prefix[3];
    int campus_choice;
    printf("\n--- 添加新书 ---\n");
    printf("请选择校区: 1. 仙林校区(XL)  2. 三牌楼校区(PL)\n");
    printf("请输入选择: ");
    if (scanf("%d", &campus_choice) != 1) {
        campus_choice = 1; // 解析失败默认仙林校区
    }
    getchar(); // 清除换行符

    if (campus_choice == 2) {
        strcpy(prefix, "PL");
    } else {
        strcpy(prefix, "XL");
    }

    // 2. 输入书名
    printf("请输入书名: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0; // 清除换行符

    // 3. 查重：如果书名在同校区已存在，数量+1，不新增条目
    {
        Book b;
        Book books[200];
        int count = 0;
        int found = 0;
        FILE *fp = fopen("book.txt", "r");
        if (fp != NULL) {
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                if (strlen(buffer) > 5) {
                    sscanf(buffer, "%19s %99s %49s %d %d %d",
                           b.id, b.name, b.leibie,
                           &b.totalCopies, &b.availableCopies, &b.borrowCount);
                    books[count] = b;
                    // 书名相同且校区前缀匹配
                    if (strcmp(b.name, name) == 0 && strncmp(b.id, prefix, 2) == 0) {
                        found = 1;
                        books[count].totalCopies += 1;
                        books[count].availableCopies += 1;
                    }
                    count++;
                }
            }
            fclose(fp);
        }

        if (found) {
            FILE *fp_write = fopen("book.txt", "w");
            if (fp_write == NULL) {
                printf("文件打开失败！\n");
                return;
            }
            int i;
            for (i = 0; i < count; i++) {
                fprintf(fp_write, "%s %s %s %d %d %d\n",
                        books[i].id, books[i].name, books[i].leibie,
                        books[i].totalCopies, books[i].availableCopies, books[i].borrowCount);
            }
            fclose(fp_write);
            printf("\n该书已存在，总数和可借数量已各+1！\n");
            return;
        }
    }

    // 4. 书不存在 → 继续录入新书信息
    printf("请输入类别: ");
    fgets(leibie, sizeof(leibie), stdin);
    leibie[strcspn(leibie, "\n")] = 0; // 清除换行符

    printf("请输入总数: ");
    char totalStr[20];
    fgets(totalStr, sizeof(totalStr), stdin);
    total = atoi(totalStr); // 字符串转整数

    // 5. 自动生成索引号（按校区统计）
    {
        int book_count = 0;
        FILE *fp_read = fopen("book.txt", "r");
        if (fp_read != NULL) {
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), fp_read) != NULL) {
                if (strlen(buffer) > 5) {
                    if (strncmp(buffer, prefix, 2) == 0) {
                        book_count++;
                    }
                }
            }
            fclose(fp_read);
        }
        sprintf(id, "%s-B%03d", prefix, book_count + 1);
    }

    // 6. 自动计算
    available = total; // 新书可借数量 = 总数量

    // 7. 写入文件
    FILE *fp_write = fopen("book.txt", "a");
    if (fp_write == NULL) {
        printf("文件打开失败！\n");
        return;
    }

    // 如果文件非空且末尾没有换行，先补一个换行
    fseek(fp_write, 0, SEEK_END);
    long fsize = ftell(fp_write);
    if (fsize > 0) {
        fseek(fp_write, -1, SEEK_END);
        char last;
        fread(&last, 1, 1, fp_write);
        if (last != '\n') {
            fseek(fp_write, 0, SEEK_END);
            fprintf(fp_write, "\n");
        }
    }

    fprintf(fp_write, "%s %s %s %d %d %d\n", id, name, leibie, total, available, borrow);
    fclose(fp_write);
    printf("\n添加成功！系统自动分配编号: %s\n", id);
}