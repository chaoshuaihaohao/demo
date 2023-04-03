#include<stdio.h>

void main()
{
char str[100];
    FILE *fp;

    fp = fopen("network.json", "r");

if (fp == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    while (fscanf(fp, "%s", str) != EOF) {
        printf("%s", str);
    }

    fclose(fp);

//printf("%s\n", json_string);
}
