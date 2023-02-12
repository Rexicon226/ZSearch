#ifndef HEADER_FILE_H
#define HEADER_FILE_H

#define UNCONST(type, var) (*(type*)&(var))

inline int min_value(const int a, const int b) {
    int result;
    if (a < b) {
        result = a;
    } else {
        result = b;
    }
    return result;
}

inline void swap(float a, float b) 
{
    const float temp = a;
    a = b;
    b = temp;
}

inline void swap_int(int a, int b) 
{
    const int temp = a;
    a = b;
    b = temp;
}

inline char* copy_string(const char* s)
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

inline char* request_prompt(void)
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

inline char* copy_sqlite_string(const unsigned char* s)
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

inline char* to_lower_case(const char* str)
{
    const size_t len = strlen(str);
    char* lower = calloc(len + 1, sizeof(char));
    for (size_t i = 0; i < len; ++i)
    {
        lower[i] = (char)tolower(str[i]);
    }
    return lower;
}

inline int edit_distance(const char* s1, const char* s2)
{
    s1 = to_lower_case(s1);
    s2 = to_lower_case(s2);
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);

    int** dp = malloc((len1 + 1) * sizeof(int*));
    for (size_t i = 0; i <= len1; i++)
    {
        dp[i] = malloc((len2 + 1) * sizeof(int));
        dp[i][0] = (int)i;
    }
    for (size_t j = 1; j <= len2; j++)
    {
        dp[0][j] = (int)j;
    }

    for (size_t i = 1; i <= len1; i++)
    {
        for (size_t j = 1; j <= len2; j++)
        {
            const int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = min_value(min_value(dp[i - 1][j] + 1, dp[i][j - 1] + 1), dp[i - 1][j - 1] + cost);
        }
    }

    const int result = dp[len1][len2];

    for (size_t i = 0; i <= len1; i++)
    {
        free(dp[i]);
    }
    free(dp);

    return result;
}

inline float similarity(const char* s1, const char* s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    const char* longer = (len1 > len2) ? s1 : s2;
    const char* shorter = (len1 > len2) ? s2 : s1;
    const int longer_length = (int) strlen(longer);

    if (longer_length == 0)
    {
        return 1;
    }

    // Replace the function call to edit_distance with a more efficient algorithm
    return (float)(longer_length - edit_distance(longer, shorter)) / (float)longer_length;
}

inline void test_performance(void)
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

inline float url_metric(char* row[], const char* perfect_url, char* prompt)
{
    const char* used_domain = row[0];
    const float urldata = similarity(perfect_url, used_domain);
    return urldata;
}

inline int compare_floats(const void *a, const void *b)
{
    const float x = *(float *)a;
    const float y = *(float *)b;
    
    if (x < y)
        return 1;
    if (x > y)
        return -1;
    return 0;
}

#endif /* HEADER_FILE_H */