#include <stdio.h>

struct user {
    int id;
    char *password;
};

struct user user_list[] = { 
    {1, "User 1"}, 
    {2, "User 2"},
    {3, "User 3"}
};