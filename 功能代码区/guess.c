// guess.c : AI玩家猜数字游戏模块
//
// 电脑生成1~1000的随机数，AI用二分法模拟玩家猜测，
// 将完整游戏过程输出至屏幕和 guess_log.txt 文件。

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>
#include "guess.h"

// 同时输出到屏幕和日志文件
static void log_out(FILE *fp, const char *fmt, ...)
{
    va_list args1, args2;
    va_start(args1, fmt);
    va_copy(args2, args1);
    vprintf(fmt, args1);
    va_end(args1);
    if (fp != NULL) {
        vfprintf(fp, fmt, args2);
    }
    va_end(args2);
}

void ai_guess_number()
{
    FILE *fp;
    int target, guess, low, high, step;

    srand((unsigned int)time(NULL));
    target = rand() % 1000 + 1; // 1~1000

    fp = fopen("guess_log.txt", "w");

    log_out(fp, "\n");
    log_out(fp, "**************************************************\n");
    log_out(fp, "        AI玩家 -- 猜数字游戏（二分查找法）\n");
    log_out(fp, "        猜数范围：1 ~ 1000\n");
    log_out(fp, "**************************************************\n\n");

    log_out(fp, "目标数字已由电脑生成（保密中...）\n\n");

    log_out(fp, "%-6s  %-12s  %-12s  %-12s  %s\n",
            "轮次", "下界(low)", "上界(high)", "AI猜测值", "判断结果");
    log_out(fp, "------------------------------------------------------------------\n");

    low = 1;
    high = 1000;
    step = 0;

    while (low <= high) {
        step++;
        guess = (low + high) / 2; // 二分取中点

        if (guess == target) {
            log_out(fp, "%-6d  %-12d  %-12d  %-12d  *** 猜中！***\n",
                    step, low, high, guess);
            break;
        } else if (guess < target) {
            log_out(fp, "%-6d  %-12d  %-12d  %-12d  太小，目标更大\n",
                    step, low, high, guess);
            low = guess + 1;
        } else {
            log_out(fp, "%-6d  %-12d  %-12d  %-12d  太大，目标更小\n",
                    step, low, high, guess);
            high = guess - 1;
        }
    }

    log_out(fp, "------------------------------------------------------------------\n\n");
    log_out(fp, "==================== 游戏结束 ====================\n");
    log_out(fp, "  目标数字 : %d\n", target);
    log_out(fp, "  AI猜测次数 : %d 次\n", step);
    log_out(fp, "  二分法理论上限 : %d 次 (log2(1000) ~= 10)\n",
            (int)(log(1000.0) / log(2.0)) + 1);
    log_out(fp, "=================================================\n\n");
    log_out(fp, "[提示] 完整游戏日志已保存至 guess_log.txt\n");

    if (fp != NULL) {
        fclose(fp);
    }

    printf("\n按回车键返回主菜单...");
    getchar(); // 等待用户按回车键
}