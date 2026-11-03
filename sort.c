#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sort.h"
#include "book.h" 

static int disp_width(const char *str)
{
    int w = 0;
    while (*str) {
        if ((*str & 0x80) == 0) {
            w++;
            str++;
        } else {
            w += 2;
            while ((*++str & 0xC0) == 0x80);
        }
    }
    return w;
}

static void print_col(const char *str, int width)
{
    int w = disp_width(str);
    printf("%s", str);
    for (int i = w; i < width; i++) printf(" ");
}

void printBooksByBorrowCount()
{
    FILE *fp = fopen("book.txt", "r");
    if (fp == NULL)
    {
        printf("未找到 book.txt，请先添加数据。\n");
        return;
    }

    Book books[100]; 
    int count = 0;
    char line[512];
    
    // 【修改】极简空格读取法，直接按 6 个字段解析
    while (fgets(line, sizeof(line), fp) != NULL && count < 100)
    {
        if (strlen(line) < 5) continue; // 跳过空行
        
        if (sscanf(line, "%s %s %s %d %d %d", 
                   books[count].id, books[count].name, books[count].leibie,
                   &books[count].totalCopies, &books[count].availableCopies, &books[count].borrowCount) == 6)
        {
            count++;
        }
    }
    fclose(fp);

    if (count == 0)
    {
        printf("数据为空，或格式读取失败。\n");
        return;
    }

    // 冒泡排序：按借阅次数从高到低
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - 1 - i; j++)
        {
            if (books[j].borrowCount < books[j + 1].borrowCount)
            {
                Book temp = books[j];
                books[j] = books[j + 1];
                books[j + 1] = temp;
            }
        }
    }

    print_col("索引号", 12); printf(" ");
    print_col("书名", 24); printf(" ");
    print_col("类别", 12); printf(" ");
    print_col("总数", 8); printf(" ");
    print_col("已借阅", 8); printf(" ");
    print_col("剩余", 8); printf(" ");
    print_col("借阅次数", 10);
    printf("\n");
    printf("----------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++)
    {
        int borrowed = books[i].totalCopies - books[i].availableCopies;
        char num_buf[20];
        print_col(books[i].id, 12); printf(" ");
        print_col(books[i].name, 24); printf(" ");
        print_col(books[i].leibie, 12); printf(" ");
        sprintf(num_buf, "%d", books[i].totalCopies); print_col(num_buf, 8); printf(" ");
        sprintf(num_buf, "%d", borrowed); print_col(num_buf, 8); printf(" ");
        sprintf(num_buf, "%d", books[i].availableCopies); print_col(num_buf, 8); printf(" ");
        sprintf(num_buf, "%d", books[i].borrowCount); print_col(num_buf, 10);
        printf("\n");
    }
}