// sort.h : 图书排序显示模块的头文件
//
// 提供按借阅次数降序排列并输出图书列表的功能。

#ifndef SORT_H
#define SORT_H

// 按借阅次数从高到低排序，并格式化输出所有图书信息
// 从 book.txt 读取数据，冒泡排序后以表格形式打印。
void printBooksByBorrowCount();

#endif