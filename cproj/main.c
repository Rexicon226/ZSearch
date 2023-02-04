#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <time.h>
enum
{
    MAX_ROWS = 30000,
    MAX_STABLE_ROWS = 10000,
};

#include "sqlite/sqlite3.h"

char* copy_string(const char* s)
{
    if (s == NULL)
    {
        return NULL;
    }
    const int size = (int)strlen(s) + 1;
    char* s2 = malloc(size);
    strcpy_s(s2, size, s);
    return s2;
}

int min_value(int a, int b)
{
    return (a > b) ? b : a;
}

char* toLowerCase(const char* str)
{
    const size_t len = strlen(str);
    char* lower = calloc(len + 1, sizeof(char));
    for (size_t i = 0; i < len; ++i)
    {
        lower[i] = (char)tolower(str[i]);
    }
    return lower;
}

int editDistance(const char* s1, const char* s2)
{
    s1 = toLowerCase(s1);
    s2 = toLowerCase(s2);

    char* costs = malloc((strlen(s2) + 1) * sizeof(char));

    for (size_t i = 0; i <= strlen(s1); i++)
    {
        int last_value = (int)i;

        for (unsigned int j = 0; j <= strlen(s2); j++)
        {
            if (i == 0)
            {
                costs[j] = (char)j;
            }
            else
            {
                if (j > 0)
                {
                    int newValue = (unsigned char)costs[j - 1];
                    if (s1[i - 1] != s2[j - 1])
                    {
                        newValue = min_value(min_value(newValue, last_value), costs[j]) + 1;
                    }
                    costs[j - 1] = (char)last_value;
                    last_value = newValue;
                }
            }
        }
        if (i > 0)
        {
            costs[strlen(s2)] = (char)last_value;
        }
    }
    return costs[strlen(s2)];
}

float similarity(const char* s1, const char* s2)
{
    const char* longer = s1;
    const char* shorter = s2;

    if (strlen(s1) < strlen(s2))
    {
        longer = s2;
        shorter = s1;
    }

    const int longer_length = (int)strlen(longer);

    if (longer_length == 0)
    {
        return 1;
    }

    return (float)(longer_length - editDistance(longer, shorter)) / (float)longer_length;
}

void test_performance(void)
{
    clock_t t = clock();
    for (int i = 0; i < 100000; i++)
    {
        similarity("Test", "Tes");
    }

    t = clock() - t;


    const double time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Time Taken Avg: %.6f micros\n", time_taken * 10);
}

char* request_prompt(void)
{
    char* prompt = malloc(50 * sizeof(char));
    printf("Enter a prompt: ");
    if (fgets(prompt, 50, stdin) == NULL)
    {
        printf("Error reading input\nInput Size Limit: 50");
    }

    prompt[strcspn(prompt, "\n")] = 0;

    return prompt;
}

float url_metric(char* row[], const char* perfect_url, char* prompt)
{
    const char* used_domain = row[0];
    const float urldata = similarity(perfect_url, used_domain);
    return urldata;
}



char* get_prompt(char* prompt)
{
    const clock_t start_total = clock();
    sqlite3* db;
    sqlite3_stmt* stmt;
    sqlite3_open("crawler.db", &db);
    sqlite3_prepare_v2(db, "SELECT * FROM crawles LIMIT 260000", -1, &stmt, 0);

    char* rows[MAX_ROWS][3] = {{"0"}};

    for (int i = 0; i < MAX_ROWS; i++)
    {
        sqlite3_step(stmt);
        rows[i][0] = copy_string((char*)sqlite3_column_text(stmt, 0));
        rows[i][1] = copy_string((char*)sqlite3_column_text(stmt, 1));
        rows[i][2] = copy_string((char*)sqlite3_column_text(stmt, 2));
    }

    const clock_t end_total = clock();
    const double cpu_time_used_total = ((double)(end_total - start_total)) / CLOCKS_PER_SEC;
    printf("Time taken to get DB:  %.3f ms\n", cpu_time_used_total * 1000);

    const clock_t start = clock();

    char* perfect_url = copy_string(prompt);
    strcat_s(perfect_url, sizeof prompt + 4, ".com");

    float best_url[MAX_ROWS];

    for (int i = 0; i < MAX_ROWS; i++)
    {
        char* row[3];
        row[0] = rows[i][0];
        row[1] = rows[i][1];
        row[2] = rows[i][2];
        const float urlm = url_metric(row, perfect_url, prompt);
        best_url[i] = urlm;
    }

    const clock_t end = clock();
    const double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to search:  %.3f ms\n", cpu_time_used * 1000);    
    return "NULL";
}

int main(void)
{
    char* prompt = "amazon";
    printf("%s\n", get_prompt(prompt));
    return 0;
}
