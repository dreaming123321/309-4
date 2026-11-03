// sort.c : 图书排序显示模块的实现文件
//
// 实现 printBooksByBorrowCount() 函数：
//   从 book.txt 读取所有图书 → 按借阅次数冒泡降序排列 → 格式化表格输出。
// 包含中文显示宽度计算函数，用于中英文混排时列对齐。

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sort.h"
#include "book.h"

// 计算字符串在终端中的显示宽度
// 中文字符（UTF-8 多字节）占 2 个显示宽度，英文/数字占 1 个。
// 参数 str : 要计算宽度的字符串
// 返回值 : 字符串在终端中占据的列数
static int disp_width(const char *str)
{
    int w = 0;                        // 累计显示宽度
    while (*str) {
        if ((*str & 0x80) == 0) {     // ASCII 字符（单字节），宽度 +1
            w++;
            str++;
        } else {
            w += 2;                    // 中文等多字节字符，宽度 +2
            while ((*++str & 0xC0) == 0x80); // 跳过 UTF-8 后续字节
        }
    }
    return w;
}

// 按指定宽度左对齐打印字符串
// 先用 disp_width 算出实际显示宽度，再补空格补齐到目标宽度。
// 参数 str   : 要打印的字符串
// 参数 width : 目标显示宽度（列宽）
static void print_col(const char *str, int width)
{
    int w = disp_width(str);          // 计算字符串实际显示宽度
    printf("%s", str);                // 输出字符串内容
    for (int i = w; i < width; i++) printf(" "); // 不足部分用空格补齐
}

// 按借阅次数从高到低排序，并格式化输出所有图书信息
// 实现步骤：
//   1. 打开 book.txt 文件
//   2. 逐行解析图书数据到 Book 数组
//   3. 冒泡排序（borrowCount 降序）
//   4. 以表格形式打印表头和数据行，中文列左对齐
void printBooksByBorrowCount()
{
    // 打开图书数据文件
    FILE *fp = fopen("book.txt", "r");
    if (fp == NULL)
    {
        printf("未找到 book.txt，请先添加数据。\n");
        return;
    }

    Book books[100];   // 图书数组，最多容纳 100 条记录
    int count = 0;     // 实际读取到的图书数量
    char line[512];    // 行缓冲区

    // 逐行读取文件，按空格分隔的 6 个字段解析每条图书记录
    while (fgets(line, sizeof(line), fp) != NULL && count < 100)
    {
        if (strlen(line) < 5) continue; // 跳过空行或无效短行

        // 使用 sscanf 按格式 "%s %s %s %d %d %d" 解析一行数据
        if (sscanf(line, "%s %s %s %d %d %d",
                   books[count].id,           // 图书编号
                   books[count].name,         // 图书名称
                   books[count].leibie,       // 图书类别
                   &books[count].totalCopies, // 总数量
                   &books[count].availableCopies, // 可借数量
                   &books[count].borrowCount) == 6) // 借阅次数
        {
            count++; // 解析成功，计数 +1
        }
    }
    fclose(fp); // 关闭文件

    // 检查是否有有效数据
    if (count == 0)
    {
        printf("数据为空，或格式读取失败。\n");
        return;
    }

    // 冒泡排序算法：按借阅次数 borrowCount 从高到低降序排列
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - 1 - i; j++)
        {
            // 当前记录借阅次数 < 下一条记录 → 交换位置
            if (books[j].borrowCount < books[j + 1].borrowCount)
            {
                Book temp = books[j];       // 临时变量暂存
                books[j] = books[j + 1];    // 上移借阅次数更多的记录
                books[j + 1] = temp;         // 补回原位置
            }
        }
    }

    // 打印表头（使用 print_col 实现中文左对齐）
    print_col("索引号", 12); printf(" ");
    print_col("书名", 24); printf(" ");
    print_col("类别", 12); printf(" ");
    print_col("总数", 8); printf(" ");
    print_col("已借阅", 8); printf(" ");
    print_col("剩余", 8); printf(" ");
    print_col("借阅次数", 10);
    printf("\n");
    printf("----------------------------------------------------------------------------------------\n");

    // 逐行打印每条图书记录
    for (int i = 0; i < count; i++)
    {
        int borrowed = books[i].totalCopies - books[i].availableCopies; // 已借出 = 总数 - 可借
        char num_buf[20]; // 数字转字符串的临时缓冲区

        print_col(books[i].id, 12); printf(" ");    // 索引号列
        print_col(books[i].name, 24); printf(" ");  // 书名列
        print_col(books[i].leibie, 12); printf(" "); // 类别列

        // 数字字段需要先 sprintf 转为字符串，再用 print_col 对齐
        sprintf(num_buf, "%d", books[i].totalCopies);
        print_col(num_buf, 8); printf(" ");          // 总数列

        sprintf(num_buf, "%d", borrowed);
        print_col(num_buf, 8); printf(" ");          // 已借阅列

        sprintf(num_buf, "%d", books[i].availableCopies);
        print_col(num_buf, 8); printf(" ");          // 剩余列

        sprintf(num_buf, "%d", books[i].borrowCount);
        print_col(num_buf, 10);                       // 借阅次数列

        printf("\n");
    }
}