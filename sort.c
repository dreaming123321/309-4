#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sort.h"
#include "book.h" 

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

    // 打印结果：包含索引号、书名、类别、总数、已借阅、剩余、借阅次数
    printf("%-12s %-20s %-12s %-8s %-8s %-8s %-8s\n", "索引号", "书名", "类别", "总数", "已借阅", "剩余", "借阅次数");
    printf("----------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++)
    {
        int borrowed = books[i].totalCopies - books[i].availableCopies;
        printf("%-12s %-20s %-12s %-8d %-8d %-8d %-8d\n", 
               books[i].id, 
               books[i].name, 
               books[i].leibie, 
               books[i].totalCopies, 
               borrowed, 
               books[i].availableCopies, 
               books[i].borrowCount);
    }
}