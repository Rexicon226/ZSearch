#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "util.h"

#include "sqlite/sqlite3.h"

enum
{
    max_rows = 250000,
    max_stable_rows = 30000,
};

void progressbar(int i, int count)
{
    static int last_percent = 0;
    const int percent = (int)((double)i / count * 100.0);
    if (percent > last_percent)
    {
        printf("\rProgress: [");
        for (int n = 0; n < 100; n += 2)
        {
            if (n < percent)
                printf("=");
            else
                printf(" ");
        }
        printf("] %d%%", percent);
        if (fflush(stdout))
            printf("Error flushing stdout");
        last_percent = percent;
    }
    if (i == count - 1)
        printf("\n");
}


void sort_elements(int *index_map, float *elements, const int count) {
    elements[0] = 0;
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (elements[i] < elements[j]) {
                const float temp = elements[i];
                elements[i] = elements[j];
                elements[j] = temp;
                const int temp2 = index_map[i];
                index_map[i] = index_map[j];
                index_map[j] = temp2;
            }
        }
    }
}


float get_prompt(char* prompt)
{
    sqlite3* db;
    sqlite3_stmt* stmt;
    sqlite3_open("crawler.db", &db);

    const clock_t start_total = clock();

    const char* query = "Select * FROM crawles";

    sqlite3_prepare_v2(db, query, -1, &stmt, 0);
    
    
    char* best_rows[max_stable_rows][3];
    int best_rows_index = 0;

    for (int n = 0; n < max_rows / max_stable_rows; n++)
    {
            
        int count = 0;
        
        printf("Round: %i\n", n);
        
        float url_metric_values[max_stable_rows] = {0};

        for (int url_count = 0; url_count < max_stable_rows; url_count++)
        {
            url_metric_values[url_count] = 0;
            url_metric_values[url_count] = 0;
            url_metric_values[url_count] = 0;
        }
        
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
        printf("Time taken to get DB:  %.0f ms\n", cpu_time_used_db * 1000);

        const clock_t start = clock();

        char* perfect_url = copy_string(prompt);
        strcat(perfect_url, ".com");

        for (int i = 0; i < max_stable_rows; i++)
        {
            char* row[3];
            row[0] = rows[i][0];
            count++;
            const float urlm = url_metric(row, perfect_url, prompt);
            url_metric_values[i] = urlm;
        }

        int *indices = malloc(count * sizeof(int));

        for (int count_i = 0; count_i < count; count_i++)
        {
            indices[count_i] = count_i;
        }   
        
        sort_elements(indices, url_metric_values, count);

        for (int i = 0; i < 20; i++)
        {
            if (best_rows_index < max_stable_rows)
            {
                best_rows[best_rows_index][0] = rows[indices[i]][0];
                best_rows_index++;
            }
        }

        free(indices);

        const clock_t end = clock();
        const double cpu_time_used = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Time taken to get metric:  %.0f ms\n", cpu_time_used * 1000);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    for (int o = 0; o < 20; o++)
    {
        printf("%s\n", best_rows[o][0]);
    }   

    const clock_t end_total = clock();

    const double cpu_time_used_total = (double)(end_total - start_total) / CLOCKS_PER_SEC;
    printf("Time taken:  %.0f ms\n", cpu_time_used_total * 1000);
    
    return 0;
}

int main(void)
{
    char* prompt = "amzon";
    printf("%f\n", (float)get_prompt(prompt));
    //test_performance();
    return 0;
}
