#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    int a = 0;
    char *string = (char *)malloc(sizeof(char)*128);

    printf("Enter something: ");
    scanf("%s", string);

    printf("The string you entered is %s\n", string);
    //printf("%s\n", strtok(&qqqqq, ""));
    //printf("%s\n", strtok(NULL, ""));
    return 0;
}
