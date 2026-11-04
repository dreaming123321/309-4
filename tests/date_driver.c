#include <stdio.h>
#include <string.h>
#include "../borrow.h"
#include "../remind.h"
int main(int argc, char **argv)
{
    if (argc==2) return remindat(argv[1]) ? 0 : 1;
    if (validdate("2025-02-29") || !validdate("2024-02-29") || validdate("2026-04-31")) return 1;
    if (strcmp(jisuan_guihuantime("2024-02-10"),"2024-03-11")) return 2;
    if (dateculculate("2024-02-28","2024-03-01")!=2) return 3;
    return 0;
}
