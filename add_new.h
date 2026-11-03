// add_new.h : 添加新书模块的头文件
//
// 提供向书库中添加新图书的功能。
// 支持校区选择（XL仙林 / PL三牌楼），同校区同名图书仅增加数量。

#ifndef ADD_NEW_H
#define ADD_NEW_H

// 添加新书的主函数
// 流程：选择校区 → 输入书名 → 查重（同校区同名则数量+1）→ 录入类别和总数 → 自动生成编号 → 写入文件。
void add_new_book(void);

#endif