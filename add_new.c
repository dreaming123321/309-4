// add_new.c : 添加新书模块的实现文件
//
// 实现 add_new_book() 函数，完整流程如下：
//   1. 选择校区（XL 仙林 / PL 三牌楼）
//   2. 输入书名
//   3. 查重：遍历 book.txt，若同校区已有同名书 → totalCopies 和 availableCopies 各 +1，直接返回
//   4. 未命中 → 继续录入类别、总数
//   5. 按校区统计现有数量，自动生成编号（如 XL-B041）
//   6. 追加写入 book.txt

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "add_new.h"
#include "book.h"

// 添加新书的主函数
// 交互式录入，支持校区选择与同校区同名查重。
void add_new_book(void)
{
    char id[20];          // 自动生成的图书编号
    char name[100];       // 书名（用户输入）
    char leibie[50];      // 类别（用户输入）
    float price;          // 价格（用户输入）
    int total;            // 总数量
    int available;        // 可借数量（新书 = 总数量）
    int borrow = 0;        // 借阅次数（新书初始为 0）

    // ========== 步骤1：选择校区 ==========
    char prefix[3];       // 校区前缀字符串（"XL" 或 "PL"）
    int campus_choice;    // 用户输入的校区选项
    printf("\n--- 添加新书 ---\n");
    printf("请选择校区: 1. 仙林校区(XL)  2. 三牌楼校区(PL)\n");
    printf("请输入选择: ");
    if (scanf("%d", &campus_choice) != 1) {
        campus_choice = 1; // 输入解析失败时默认选择仙林校区，防止异常
    }
    getchar(); // 清除输入缓冲区中的换行符，避免干扰后续 fgets

    // 根据用户选择确定校区前缀
    if (campus_choice == 2) {
        strcpy(prefix, "PL"); // 三牌楼校区
    } else {
        strcpy(prefix, "XL"); // 仙林校区（含默认情况）
    }

    // ========== 步骤2：输入书名 ==========
    printf("请输入书名: ");
    fgets(name, sizeof(name), stdin);           // 读取整行（允许书名含空格）
    name[strcspn(name, "\n")] = 0;              // 去除末尾的换行符

    // ========== 步骤3：查重（同校区 + 同名 → 数量 +1） ==========
    {
        Book b;               // 临时解析单条图书记录
        Book books[200];      // 缓存文件中的所有图书记录
        int count = 0;        // 已读取的图书数量
        int found = 0;        // 标记是否找到同名同校区书

        // 以只读方式打开 book.txt
        FILE *fp = fopen("book.txt", "r");
        if (fp != NULL) {
            char buffer[256]; // 行读取缓冲区
            // 逐行读取并解析每条图书记录
            while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                if (strlen(buffer) > 5) {       // 跳过空行和过短的行
                    // 按格式解析：编号 书名 类别 价格 总数 可借 借阅次数
                    sscanf(buffer, "%19s %99s %49s %f %d %d %d",
                           b.id, b.name, b.leibie, &b.price,
                           &b.totalCopies, &b.availableCopies, &b.borrowCount);
                    books[count] = b;           // 存入缓存数组

                    // 书名相同 且 校区前缀匹配（如 XL-B001 的前缀 XL == 当前选择的校区）
                    if (strcmp(b.name, name) == 0 && strncmp(b.id, prefix, 2) == 0) {
                        found = 1;              // 标记为已找到
                        books[count].totalCopies += 1;      // 总数量 +1
                        books[count].availableCopies += 1;  // 可借数量 +1
                    }
                    count++;                    // 有效记录计数 +1
                }
            }
            fclose(fp); // 关闭读取文件
        }

        // 如果查重命中，将修改后的数组完整写回文件
        if (found) {
            FILE *fp_write = fopen("book.txt", "w"); // 以覆盖方式打开
            if (fp_write == NULL) {
                printf("文件打开失败！\n");
                return;
            }
            // 逐条写回（含已修改数量的那条记录）
            int i;
            for (i = 0; i < count; i++) {
                fprintf(fp_write, "%s %s %s %.2f %d %d %d\n",
                        books[i].id, books[i].name, books[i].leibie, books[i].price,
                        books[i].totalCopies, books[i].availableCopies, books[i].borrowCount);
            }
            fclose(fp_write);
            printf("\n该书已存在，总数和可借数量已各+1！\n");
            return; // 查重命中后直接返回，不再继续录入新书信息
        }
    }

    // ========== 步骤4：书不存在 → 继续录入新书信息 ==========
    printf("请输入类别: ");
    fgets(leibie, sizeof(leibie), stdin);      // 读取类别字符串
    leibie[strcspn(leibie, "\n")] = 0;         // 去除换行符

    printf("请输入价格: ");
    char priceStr[20];
    fgets(priceStr, sizeof(priceStr), stdin);  // 以字符串形式读取价格
    price = (float)atof(priceStr);             // 字符串转浮点数

    printf("请输入总数: ");
    char totalStr[20];
    fgets(totalStr, sizeof(totalStr), stdin);  // 以字符串形式读取数字
    total = atoi(totalStr);                    // 字符串转整数

    // ========== 步骤5：自动生成索引号（按校区独立编号） ==========
    {
        int book_count = 0;                     // 当前校区已有的图书数量
        FILE *fp_read = fopen("book.txt", "r");
        if (fp_read != NULL) {
            char buffer[256];
            // 遍历文件，统计与当前校区前缀匹配的记录数
            while (fgets(buffer, sizeof(buffer), fp_read) != NULL) {
                if (strlen(buffer) > 5) {
                    if (strncmp(buffer, prefix, 2) == 0) {
                        book_count++;           // 同校区记录 +1
                    }
                }
            }
            fclose(fp_read);
        }
        // 生成编号：前缀 + -B + 3位数字序号（如 XL-B041、PL-B016）
        sprintf(id, "%s-B%03d", prefix, book_count + 1);
    }

    // ========== 步骤6：自动计算与写入文件 ==========
    available = total; // 新书入库，当前可借数量 = 总数量

    FILE *fp_write = fopen("book.txt", "a"); // 以追加方式打开文件
    if (fp_write == NULL) {
        printf("文件打开失败！\n");
        return;
    }

    // 如果文件非空且末尾没有换行符，先补一个换行，防止新记录黏到上一行末尾
    fseek(fp_write, 0, SEEK_END);              // 移动到文件末尾
    long fsize = ftell(fp_write);              // 获取文件大小
    if (fsize > 0) {
        fseek(fp_write, -1, SEEK_END);         // 回退一个字节
        char last;
        fread(&last, 1, 1, fp_write);          // 读取最后一个字符
        if (last != '\n') {
            fseek(fp_write, 0, SEEK_END);      // 回到末尾
            fprintf(fp_write, "\n");           // 补写换行
        }
    }

    // 追加写入新图书记录
    fprintf(fp_write, "%s %s %s %.2f %d %d %d\n", id, name, leibie, price, total, available, borrow);
    fclose(fp_write);
    printf("\n添加成功！系统自动分配编号: %s\n", id);
}