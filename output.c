#include<stdio.h>
#include<string.h>
#include "book.h"

int outputmenu(void)
{
    FILE *bookFile = fopen("book.txt", "r");
    FILE *outputFile = fopen("output.txt", "w");

    if (bookFile == NULL || outputFile == NULL) {
        printf("文件打开失败！\n");

        if (bookFile != NULL)
            fclose(bookFile);
        if (outputFile != NULL)
            fclose(outputFile);

        return 0;
    }

    char line[512];
    char id[20];

    while (fgets(line, sizeof(line), bookFile) != NULL) {
        if (line[0] == '\0')
            continue;

        sscanf(line, "%19s", id);

        line[strcspn(line, "\r\n")] = '\0';
        fprintf(outputFile, "%s", line);
        if (strncmp(id, "XL", 2) == 0)
            fprintf(outputFile, "仙林");

        fprintf(outputFile, "\n");
    }

    fclose(bookFile);
    fclose(outputFile);

    outputFile = fopen("output.txt", "r");
    if (outputFile == NULL) {
        printf("output.txt打开失败！\n");
        return 0;
    }

    printf("\n--- output.txt 内容 ---\n");
    while (fgets(line, sizeof(line), outputFile) != NULL)
        printf("%s", line);

    fclose(outputFile);
    printf("输出完成！\n");
    return 1;
}