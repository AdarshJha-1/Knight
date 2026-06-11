/* ==========================================================
 * Name:         Adarsh Jha, Aka Mr. Fool
 * File:         knight.c
 * Date:         2026-06-05
 * Time:         00:20:31
 * ========================================================== */

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L

#include "uthash.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BLOCK "block"
#define UNBLOCK "unblock"
#define SHOW "show"
#define BLOCKED_FILE "blocked.txt"

typedef struct
{
    char *domain_or_ip;
    UT_hash_handle hh;
} Blocked;

Blocked *block_list = NULL;

void load_from_file()
{
    FILE *file = fopen(BLOCKED_FILE, "r");
    if (file == NULL)
    {
        if (errno != ENOENT)
        {
            fprintf(stderr, "Failed to open blocked config file: %s\n", strerror(errno));
        }
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file))
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) > 0)
        {

            Blocked *entry = malloc(sizeof(*entry));
            entry->domain_or_ip = strdup(buffer);
            HASH_ADD_KEYPTR(hh, block_list, entry->domain_or_ip, strlen(entry->domain_or_ip), entry);
        }
    }

    fclose(file);
}

void save_to_file()
{
    FILE *file = fopen(BLOCKED_FILE, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open blocked config file: %s\n", strerror(errno));
        return;
    }
    Blocked *entry;
    for (entry = block_list; entry != NULL; entry = entry->hh.next)
    {
        fprintf(file, "%s\n", entry->domain_or_ip);
    }
    fclose(file);
}

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
    if (block_list == NULL)
    {
        printf("\nNothing is in the list, Block to see some.\n");
        return;
    }
    printf("\n********** Blocked Domain/IP List **********\n");
    Blocked *entry;
    for (entry = block_list; entry != NULL; entry = entry->hh.next)
    {
        printf("Blocked:\t%s\n", entry->domain_or_ip);
    }
}

bool is_valid_ip(char const *ip_addr)
{
    struct in_addr dst1, dst2;
    int v4, v6;
    v4 = inet_pton(AF_INET, ip_addr, &dst1);
    v6 = inet_pton(AF_INET6, ip_addr, &dst2);
    return v4 || v6;
}

bool is_valid_domain(char const *domain)
{
    struct addrinfo hints, *res;
    int status;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(domain, NULL, &hints, &res);
    if (status == 0)
    {
        freeaddrinfo(res);
        return true;
    }

    freeaddrinfo(res);
    return false;
}

int main(int argc, char const *argv[])
{
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "\nUsage:\n\tknight [options] [domain/ip]\n\toptions:\n\t\tblock <domain/ip>: block certain domain or ip\n\t\tunblock <domain/ip>: unblock blocked domain or ip\n\t\tshow: show currently blocked list\n");
        return EXIT_FAILURE;
    }
    load_from_file();

    char const *command = argv[1];

    if (argc == 2)
    {
        if (strcmp(command, SHOW) == 0)
        {
            show_block_list();
        }
        else
        {
            fprintf(stderr, "Unknown option: %s\n", command);
            return EXIT_FAILURE;
        }
    }
    else if (argc == 3)
    {
        char const *domain_or_ip = argv[2];

        if (!is_valid_domain(domain_or_ip))
        {
            if (!is_valid_ip(domain_or_ip))
            {
                fprintf(stderr, "Invalid Domain/IP\n");
                return EXIT_FAILURE;
            }
        }

        if (strcmp(command, BLOCK) == 0)
        {
            if (strcmp(domain_or_ip, SHOW) == 0)
            {
                show_block_list();
            }
            else
            {
                block_domain_or_ip(domain_or_ip);
            }
        }
        else if (strcmp(command, UNBLOCK) == 0)
        {
            unblock_domain_or_ip(domain_or_ip);
        }
        else
        {
            fprintf(stderr, "\nUsage:\n\tknight [options] domain/ip\n\toptions:\n\t\tblock: block certain domain or ip\n\t\tunblock: unblock blocked domain or ip\n");
            return EXIT_FAILURE;
        }
    }

    save_to_file();

    Blocked *curr, *temp;
    HASH_ITER(hh, block_list, curr, temp)
    {
        HASH_DEL(block_list, curr);
        free(curr->domain_or_ip);
        free(curr);
    }

    return 0;
}

/*
Knight CLI - Command List

1. knight og
   - Creates original backup of /etc/hosts
   - Stores it as /etc/hosts.knight.og

2. knight block <domain|ip>
   - Adds domain or IP to block list
   - Stores in memory + blocked.txt

3. knight unblock <domain|ip>
   - Removes domain or IP from block list
   - Updates blocked.txt

4. knight apply
   - Loads OG hosts file
   - Merges with blocked list
   - Removes duplicates
   - Writes final result to /etc/hosts

5. knight reset
   - Restores /etc/hosts from OG backup
   - Removes all knight applied changes

6. knight backup
   - Creates backup of current /etc/hosts
   - Stores as /etc/hosts.knight.backup

7. knight show
   - Displays all blocked domains and IPs
*/