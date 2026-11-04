// add_new.c : 添加新书模块的实现文件
//
// 实现 add_new_book() 函数，完整流程如下：
//   1. 选择校区（XL 仙林 / PL 三牌楼）
//   2. 输入书名
//   3. 查重：遍历 book.txt，若同校区已有同名书 → totalCopies 和 availableCopies 各 +1，直接返回
//   4. 未命中 → 继续录入类别、总数
//   5. 按校区取最大编号，自动生成编号（如 XL-B041）
//   6. 追加写入 book.txt

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <windows.h>
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
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("校区选择必须为 1 或 2。\n");
        return;
    }
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
    if (campus_choice != 1 && campus_choice != 2) {
        printf("校区选择必须为 1 或 2。\n");
        return;
    }

    // 根据用户选择确定校区前缀
    if (campus_choice == 2) {
        strcpy(prefix, "PL"); // 三牌楼校区
    } else {
        strcpy(prefix, "XL"); // 仙林校区（含默认情况）
    }

    // ========== 步骤2：输入书名 ==========
    printf("请输入书名: ");
    if (fgets(name, sizeof(name), stdin) == NULL) return;
    if (strchr(name, '\n') == NULL && !feof(stdin)) {
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("书名过长。\n"); return;
    }
    name[strcspn(name, "\r\n")] = 0;
    if (name[0] == 0 || strpbrk(name, " \t\v\f") != NULL) {
        printf("书名不能为空或包含空格。\n"); return;
    }

    // ========== 步骤3：查重（同校区 + 同名 → 数量 +1） ==========
    {
        Book b;               // 临时解析单条图书记录
        Book books[MAX_BOOKS];      // 缓存文件中的所有图书记录
        int count = 0;        // 已读取的图书数量
        int found = 0;        // 标记是否找到同名同校区书

        // 以只读方式打开 book.txt
        FILE *fp = fopen("book.txt", "r");
        if (fp == NULL && errno != ENOENT) { printf("文件打开失败！\n"); return; }
        if (fp != NULL) {
            char buffer[512]; // 行读取缓冲区
            char extra;
            // 逐行读取并解析每条图书记录
            while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                if (strspn(buffer, " \t\r\n") != strlen(buffer)) { // 只跳过空白行
                    // 按格式解析：编号 书名 类别 价格 总数 可借 借阅次数
                    if (sscanf(buffer, "%19s %99s %49s %f %d %d %d %c",
                           b.id, b.name, b.leibie, &b.price,
                           &b.totalCopies, &b.availableCopies, &b.borrowCount, &extra) != 7 ||
                        b.totalCopies < 1 || b.totalCopies > MAX_COPIES || b.availableCopies < 0 ||
                        b.availableCopies > b.totalCopies || !(b.price >= 0 && b.price <= 500) || b.borrowCount < 0) {
                        fclose(fp);
                        printf("图书数据格式错误，未进行添加。\n");
                        return;
                    }
                    if (count >= MAX_BOOKS) {
                        fclose(fp);
                        printf("图书记录超过上限，未进行添加。\n");
                        return;
                    }
                    for (int i = 0; i < count; i++) {
                        if (strcmp(books[i].id, b.id) == 0) {
                            fclose(fp); printf("存在重复图书编号，未进行添加。\n"); return;
                        }
                    }
                    books[count] = b;           // 存入缓存数组

                    // 书名相同 且 校区前缀匹配（如 XL-B001 的前缀 XL == 当前选择的校区）
                    if (strcmp(b.name, name) == 0 && strncmp(b.id, prefix, 2) == 0) {
                        if (b.totalCopies >= MAX_COPIES) {
                            fclose(fp);
                            printf("该书总数量已达到上限 %d 本。\n", MAX_COPIES);
                            return;
                        }
                        found = 1;              // 标记为已找到
                        books[count].totalCopies += 1;      // 总数量 +1
                        books[count].availableCopies += 1;  // 可借数量 +1
                    }
                    count++;                    // 有效记录计数 +1
                }
            }
            int failed = ferror(fp);
            fclose(fp); // 关闭读取文件
            if (failed) { printf("书库读取失败，未进行添加。\n"); return; }
        }

        // 如果查重命中，将修改后的数组完整写回文件
        if (found) {
            FILE *fp_write = fopen("addtemp.txt", "w"); // 先保存临时文件，避免写入失败损坏原书库
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
            int failed = ferror(fp_write);
            if (fclose(fp_write) != 0) failed = 1;
            if (failed || !MoveFileExA("addtemp.txt", "book.txt", MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
                printf("保存失败，原书库未替换。\n"); return;
            }
            printf("\n该书已存在，总数和可借数量已各+1！\n");
            return; // 查重命中后直接返回，不再继续录入新书信息
        }
        if (count >= MAX_BOOKS) {
            printf("图书种类已达到上限 %d 条。\n", MAX_BOOKS);
            return;
        }
    }

    // ========== 步骤4：书不存在 → 继续录入新书信息 ==========
    printf("请输入类别: ");
    if (fgets(leibie, sizeof(leibie), stdin) == NULL) return;
    if (strchr(leibie, '\n') == NULL && !feof(stdin)) {
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("类别过长。\n"); return;
    }
    leibie[strcspn(leibie, "\r\n")] = 0;
    if (leibie[0] == 0 || strpbrk(leibie, " \t\v\f") != NULL) {
        printf("类别不能为空或包含空格。\n"); return;
    }

    printf("请输入价格: ");
    char priceStr[20];
    if (fgets(priceStr, sizeof(priceStr), stdin) == NULL) return;
    char *priceEnd;
    double priceValue = strtod(priceStr, &priceEnd);
    int priceParsed = priceEnd != priceStr;
    while (*priceEnd == ' ' || *priceEnd == '\t' || *priceEnd == '\r' || *priceEnd == '\n') priceEnd++;
    if (!priceParsed || *priceEnd != '\0' || !(priceValue >= 0 && priceValue <= 500)) {
        printf("价格必须在 0 到 500 元之间。\n");
        return;
    }
    price = (float)priceValue;

    printf("请输入总数: ");
    char totalStr[20];
    if (fgets(totalStr, sizeof(totalStr), stdin) == NULL) return;
    char *totalEnd;
    long totalValue = strtol(totalStr, &totalEnd, 10);
    while (*totalEnd == ' ' || *totalEnd == '\t' || *totalEnd == '\r' || *totalEnd == '\n') totalEnd++;
    if (totalEnd == totalStr || *totalEnd != '\0' || totalValue < 1 || totalValue > MAX_COPIES) {
        printf("图书总数必须为 1 到 %d 的整数。\n", MAX_COPIES);
        return;
    }
    total = (int)totalValue;

    // ========== 步骤5：自动生成索引号（按校区独立编号） ==========
    {
        int max_number = 0;                     // 当前校区已使用的最大编号
        FILE *fp_read = fopen("book.txt", "r");
        if (fp_read != NULL) {
            char buffer[256];
            // 遍历文件，查找当前校区的最大图书编号
            while (fgets(buffer, sizeof(buffer), fp_read) != NULL) {
                if (strlen(buffer) > 5) {
                    if (strncmp(buffer, prefix, 2) == 0) {
                        int number;
                        if (sscanf(buffer + 2, "-B%d", &number) == 1 && number > max_number)
                            max_number = number;
                    }
                }
            }
            fclose(fp_read);
        }
        // 生成编号：前缀 + -B + 3位数字序号（如 XL-B041、PL-B016）
        // 借阅历史中出现过的图书编号也不重新使用。
        FILE *records = fopen("borrow.txt", "r");
        if (records == NULL && errno != ENOENT) { printf("借阅记录无法读取，未添加新书。\n"); return; }
        if (records != NULL) {
            char line[512], oldid[20];
            while (fgets(line, sizeof(line), records)) {
                if (sscanf(line, "%*s %*s %*s %*s %*s %*s %*d %*d %*s %19s", oldid) == 1 && strncmp(oldid, prefix, 2) == 0) {
                    int number;
                    if (sscanf(oldid + 2, "-B%d", &number) == 1 && number > max_number) max_number = number;
                }
            }
            int failed = ferror(records); fclose(records);
            if (failed) { printf("借阅记录读取失败，未添加新书。\n"); return; }
        }
        if (max_number == INT_MAX) { printf("编号已用尽。\n"); return; }
        snprintf(id, sizeof(id), "%s-B%03d", prefix, max_number + 1);
    }

    // ========== 步骤6：自动计算与写入文件 ==========
    available = total; // 新书入库，当前可借数量 = 总数量

    FILE *fp_write = fopen("book.txt", "a+"); // 以追加方式打开文件
    if (fp_write == NULL) {
        printf("文件打开失败！\n");
        return;
    }

    // 如果文件非空且末尾没有换行符，先补一个换行，防止新记录黏到上一行末尾
    fseek(fp_write, 0, SEEK_END);              // 移动到文件末尾
    long fsize = ftell(fp_write);              // 获取文件大小
    if (fsize > 0) {
        fseek(fp_write, -1, SEEK_END);         // 回退一个字节
        char last = '\n';
        if (fread(&last, 1, 1, fp_write) != 1) {
            fclose(fp_write); printf("无法检查文件末尾，未添加新书。\n"); return;
        }          // 读取最后一个字符
        if (last != '\n') {
            fseek(fp_write, 0, SEEK_END);      // 回到末尾
            fprintf(fp_write, "\n");           // 补写换行
        }
    }

    // 追加写入新图书记录
    fseek(fp_write, 0, SEEK_END);
    fprintf(fp_write, "%s %s %s %.2f %d %d %d\n", id, name, leibie, price, total, available, borrow);
    fclose(fp_write);
    printf("\n添加成功！系统自动分配编号: %s\n", id);
}