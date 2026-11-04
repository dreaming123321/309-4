#include <stdio.h>
#include <time.h>
// 仅用于测试：从临时目录读取模拟日期，不修改电脑时间。
time_t test_clock(time_t *output)
{
    FILE *fp=fopen("clock.txt","r"); int year,month,day; struct tm t={0}; time_t result;
    if (!fp || fscanf(fp,"%d-%d-%d",&year,&month,&day)!=3) { if (fp) fclose(fp); return (time_t)-1; }
    fclose(fp); t.tm_year=year-1900; t.tm_mon=month-1; t.tm_mday=day; t.tm_hour=12; t.tm_isdst=-1;
    result=mktime(&t); if (output) *output=result; return result;
}
