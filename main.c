// main.c : 图书管理系统的主入口文件
//
// 提供主菜单循环，调用各功能模块完成图书管理操作。
// 包含模块：添加新书、删除图书、查询图书、排序显示、输出信息。

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "sort.h"
#include "delete.h"
#include "book.h"
#include "add_new.h"
#include "output.h"
#include "search.h"
#include "borrow.h"
#include "guihuan.h"
#include "remind.h"


// 程序主入口
// 显示菜单 → 接收用户选择 → 调用对应模块函数，0 退出循环。
int main()
{
    // 设置控制台输入输出编码为 UTF-8，以正确显示中文
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    int choice; // 用户菜单选择
    remind();

    // 主循环：反复显示菜单直到用户选择退出
    do {
        // 打印功能菜单
        printf("\n========== 图书管理系统 ==========\n");
        printf("1. 添加新书\n");
        printf("2. 删除图书\n");
        printf("3. 查询图书\n");
        printf("4. 按借阅次数排序显示\n");
        printf("5. 输出信息\n");
        printf("6. 借阅图书\n");
        printf("7. 归还图书\n");
        printf("8. 查看归还提醒列表\n");
        printf("0. 退出\n");
        printf("==================================\n");

        // 接收用户输入的数字选项
        printf("请选择操作: ");
        if (scanf("%d", &choice) != 1) {
            choice = 0; // 输入解析失败时默认退出，防止死循环
        }
        getchar(); // 清除输入缓冲区中的换行符

        // 根据用户选择调用对应功能模块
        switch (choice) {
            case 1:
                add_new_book();          // 添加新书（支持校区选择与查重）
                break;
            case 2:
                deletebook();            // 删除指定图书
                break;
            case 3:
                searchbook();            // 按条件查询图书
                break;
            case 4:
                printBooksByBorrowCount(); // 按借阅次数降序排列并显示
                break;
            case 5:
                outputmenu();            // 输出图书信息到屏幕或文件
                break;
            case 6:
                borrow();                // 借阅图书
                break;
            case 7:
                guihuan();               // 归还图书
                break;
            case 8:
                remindlook();                   // 查看归还提醒列表
                break;
            case 0:
                printf("再见！\n");       // 退出程序
                break;
            default:
                printf("无效选择，请重新输入。\n");
                break;
        }
    } while (choice != 0); // 选择 0 时退出循环

    return 0;
}