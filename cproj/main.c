#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sqlite/sqlite3.h"

#define UNCONST(type, var) (*(type*)&(var))

enum
{
    max_rows = 260000,
    max_stable_rows = 50000,
};

void swap(float a, float b) 
{
    const float temp = a;
    a = b;
    b = temp;
}

void swap_int(int a, int b) 
{
    const int temp = a;
    a = b;
    b = temp;
}
 
int partition(float arr[], int indices[], int left, int right) 
{
    const float pivot = arr[right];
    int i = left - 1;
 
    for (int j = left; j <= right - 1; j++) 
    {
        if (arr[j] >= pivot) 
        {
            i++;
            swap(arr[i], arr[j]);
            swap_int(indices[i], indices[j]);
        }
    }
    swap(arr[i + 1], arr[right]);
    swap_int(indices[i + 1], indices[right]);
    return i + 1;
}

char* copy_string(const char* s)
{
    if (s == NULL)
    {
        return NULL;
    }
    const int size = (int)strlen(s) + 1;
    char* s2 = malloc(size);
    strcpy(s2, s);
    return s2;
}

char* copy_sqlite_string(const unsigned char* s)
{
    if (s == NULL)
    {
        return NULL;
    }
    const char* temp = UNCONST(char*, s);
    const int size = (int)strlen(temp) + 1;
    char* s2 = malloc(size);
    strcpy(s2, temp);
    return s2;
}

int min_value(const int a, const int b)
{
    return a > b ? b : a;
}

char* to_lower_case(const char* str)
{
    const size_t len = strlen(str);
    char* lower = calloc(len + 1, sizeof(char));
    for (size_t i = 0; i < len; ++i)
    {
        lower[i] = (char)tolower(str[i]);
    }
    return lower;
}

int edit_distance(const char* s1, const char* s2)
{
    s1 = to_lower_case(s1);
    s2 = to_lower_case(s2);

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
                    int new_value = (unsigned char)costs[j - 1];
                    if (s1[i - 1] != s2[j - 1])
                    {
                        new_value = min_value(min_value(new_value, last_value), costs[j]) + 1;
                    }
                    costs[j - 1] = (char)last_value;
                    last_value = new_value;
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

    return (float)(longer_length - edit_distance(longer, shorter)) / (float)longer_length;
}

void test_performance(void)
{
    clock_t t = clock();
    for (int i = 0; i < 100000; i++)
    {
        similarity("Test", "Tes");
    }

    t = clock() - t;
    
    const double time_taken = (double)t / CLOCKS_PER_SEC;
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

int compare_floats(const void *a, const void *b)
{
    const float x = *(float *)a;
    const float y = *(float *)b;
    
    if (x < y)
        return 1;
    if (x > y)
        return -1;
    return 0;
}


float get_prompt(char* prompt)
{
    sqlite3* db;
    sqlite3_stmt* stmt;
    sqlite3_open("crawler.db", &db);

    const clock_t start_total = clock();

    const char* query = "Select * FROM crawles";

    sqlite3_prepare_v2(db, query, -1, &stmt, 0);

    float url_metric_values[max_rows] = {0};

    int count = 0;

    for (int n = 0; n < max_rows / max_stable_rows; n++)
    {
        const clock_t start_db = clock();
        
        sqlite3_step(stmt);

        char* rows[max_stable_rows][3] = {{"0"}};

        for (int i = 0; i < max_stable_rows; i++)
        {
            sqlite3_step(stmt);
            rows[i][0] = copy_sqlite_string(sqlite3_column_text(stmt, 0));
            rows[i][1] = copy_sqlite_string(sqlite3_column_text(stmt, 1));
            rows[i][2] = copy_sqlite_string(sqlite3_column_text(stmt, 2));
        }

        const clock_t end_db = clock();
        const double cpu_time_used_db = (double)(end_db - start_db) / CLOCKS_PER_SEC;
        printf("Time taken to get DB:  %.3f ms\n", cpu_time_used_db * 1000);

        const clock_t start = clock();

        char* perfect_url = copy_string(prompt);
        strcat(perfect_url, ".com");

        for (int i = 0; i < max_stable_rows; i++)
        {
            char* row[3];
            row[0] = rows[i][0];
            count++;
            const float urlm = url_metric(row, perfect_url, prompt);
            
            url_metric_values[i + n * max_stable_rows] = urlm;
            
        }

        const clock_t end = clock();
        const double cpu_time_used = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Time taken to get metric:  %.3f ms\n", cpu_time_used * 1000);
    }

    
    const clock_t start_sort = clock();
    int *indices = malloc(count * sizeof(int));

    for (int count_i = 0; count_i < count; count_i++)
    {
        indices[count_i] = count_i;
    }

    printf("Sorted url_metric_values array: \n");
    for (int i = 0; i < 100; i++)
    {
        printf("%f %i\n", (float)url_metric_values[i], (int)indices[i]);
    }
    const clock_t end_sort = clock();
    const double cpu_time_used_sort = (double)(end_sort - start_sort) / CLOCKS_PER_SEC;
    printf("Time taken to sort:  %.3f ms\n", cpu_time_used_sort * 1000);

    const clock_t end_total = clock();

    const double cpu_time_used_total = (double)(end_total - start_total) / CLOCKS_PER_SEC;
    printf("Time taken:  %.3f ms\n", cpu_time_used_total * 1000);
    
    return (float)count;
}

int main(void)
{
    char* prompt = "amazon";
    printf("%f\n", (float)get_prompt(prompt));
    return 0;
}
