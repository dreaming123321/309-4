#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "sort.h" 
#include "delete.h"
#include "book.h"
#include "add_new.h"
#include "output.h"
#include "search.h"

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    int choice;
    
    do {
        printf("\n========== 图书管理系统 ==========\n");
        printf("1. 添加新书\n");
        printf("2. 删除图书\n");
        printf("3. 查询图书\n");
        printf("4. 按借阅次数排序显示\n");
        printf("5. 输出信息\n");
        printf("0. 退出\n");
        printf("==================================\n");
        printf("请选择操作: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                add_new_book();
                break;
            case 2:
                deletebook();
                break;
            case 3:
                searchbook();
                break;
            case 4:
                printBooksByBorrowCount();
                break;
            case 5:
                outputmenu();
                break;
            case 0:
                printf("再见！\n");
                break;
            default:
                printf("无效选择，请重新输入。\n");
                break;
        }
    } while (choice != 0);

    return 0;
}