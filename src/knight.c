/* ==========================================================
 * Name:         Adarsh Jha, Aka Mr. Fool
 * File:         knight.c
 * Date:         2026-06-05
 * Time:         00:20:31
 * ========================================================== */

#include "uthash.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
    implementing only cli part now like
    $ ./knight block googl.com or some_ip
    $ ./knight unblock googl.com or some_ip
    $ ./knight block show -> this is not very well thought i am doing it like in block command if 2nd arg is show then show blocked list (i should change variable names to be more general)

*/

// what i will do in this prototyping is like have a struct with uthash.h UT_hash_handle included which will make it hashable
// this struct will store an string domain/ip
// add to hash means blocked, remove means unblock

#define BLOCK "block"
#define UNBLOCK "unblock"
#define SHOW "show"
#define BLOCKED_FILE "blocked.txt"

typedef struct
{
    char *domain_or_ip;
    // bool is_blocked; // i don't think this is required for my use case like if present in hash means blocked only remove form hash it is unblocked
    UT_hash_handle hh;
} Blocked;

Blocked *block_list = NULL;

// TODO: Load from blocked.txt to map (to persist the data)
void load_from_file()
{
    FILE *file = fopen(BLOCKED_FILE, "r");
    if (file == NULL)
    {
        fprintf(stderr,
                "Failed to open blocked config file: %s\n",
                strerror(errno));
        return EXIT_FAILURE;
    }
}
// TODO: Add newly blocked domain or ip to the file (to persist the data)
void save_to_file();

// TODO: Remove blocked domain or ip from the file.
void save_to_file();

void block_domain_or_ip(char const *domain_or_ip)
{
    Blocked *entry;
    HASH_FIND_STR(block_list, domain_or_ip, entry);
    if (entry == NULL)
    {
        entry = malloc(sizeof(*entry));
        entry->domain_or_ip = strdup(domain_or_ip);
        HASH_ADD_KEYPTR(hh, block_list, entry->domain_or_ip, strlen(entry->domain_or_ip), entry);
        printf("%s is added in blocked", domain_or_ip);
    }
    else
    {
        printf("%s is already blocked", domain_or_ip);
    }
}

void unblock_domain_or_ip(char const *domain_or_ip)
{
    Blocked *entry;
    HASH_FIND_STR(block_list, domain_or_ip, entry);
    if (entry != NULL)
    {
        HASH_DEL(block_list, entry);
        free(entry->domain_or_ip);
        free(entry);
        printf("%s is unblocked", domain_or_ip);
    }
    else
    {
        printf("%s is already unblock", domain_or_ip);
    }
}

void show_block_list()
{
    Blocked *entry;
    if (block_list == NULL)
    {
        printf("\nNothing is in the list, Block to see some.\n");
        return; // early exit should i do this on C ??
    }
    printf("\n********** Blocked Domain/IP List **********\n");
    for (entry = block_list; entry != NULL; entry = entry->hh.next)
    {
        printf("Blocked:\t%s\n", entry->domain_or_ip);
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "\nUsage:\n\tknight [options] domain/ip\n\toptions:\n\t\tblock: block certain domain or ip\n\t\tunblock: unblock blocked domain or ip\n");
        return EXIT_FAILURE;
    }
    char const *command = argv[1];
    char const *domain_or_ip = argv[2]; // it can have more good name....but idk what to call it :0

    if (strcmp(command, BLOCK) == 0)
    {
        // below code variable name is causing problem to understand the code right?? but not to me cause i'm coding, will fix it later (hopefully :/)
        if (strcmp(domain_or_ip, SHOW) == 0)
        {
            show_block_list();
        }
        else
        {
            // i have to check for valid domain_or_ip too
            block_domain_or_ip(domain_or_ip);
        }
    }
    else if (strcmp(command, UNBLOCK) == 0)
    {
        // i have to check for valid domain_or_ip here too
        unblock_domain_or_ip(domain_or_ip);
    }
    else
    {
        fprintf(stderr, "\nUsage:\n\tknight [options] domain/ip\n\toptions:\n\t\tblock: block certain domain or ip\n\t\tunblock: unblock blocked domain or ip\n");
        return EXIT_FAILURE;
    }
    return 0;
}