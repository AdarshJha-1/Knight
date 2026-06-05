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
#include <stdbool.h>

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
    UT_hash_handle hh;
} Blocked;

Blocked *block_list = NULL;

// maybe i have to create a group for this program so this code is the only one who can perform operation on the txt file, which will give somewhat guarantee of not having inconsistent data

// TODO: Load from blocked.txt to map (to persist the data)
void load_from_file()
{
    FILE *file = fopen(BLOCKED_FILE, "r");
    if (file == NULL)
    {
        if (errno != ENOENT)
        {
            fprintf(stderr,
                    "Failed to open blocked config file: %s\n",
                    strerror(errno));
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
// TODO: Add newly blocked domain or ip to the file (to persist the data)
void save_to_file()
{
    FILE *file = fopen(BLOCKED_FILE, "w");
    if (file == NULL)
    {
        fprintf(stderr,
                "Failed to open blocked config file: %s\n",
                strerror(errno));
        return;
    }
    Blocked *entry;
    for (entry = block_list; entry != NULL; entry = entry->hh.next)
    {
        fprintf(file, "%s\n", entry->domain_or_ip);
    }
    fclose(file);
}

// TODO: Remove blocked domain or ip from the file.
// void remove_from_file();
// |-> not required in here as i am loading file once when program start and saving all the current state of config of blocked list into the file so the current state which is present inside RAM what we saving this state could have been modified any number of times like blocked, unblocked we don;t care only the last state we care about and we save that only.

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
        return; // early exit should i do this on C ??
    }
    printf("\n********** Blocked Domain/IP List **********\n");
    Blocked *entry;
    for (entry = block_list; entry != NULL; entry = entry->hh.next)
    {
        printf("Blocked:\t%s\n", entry->domain_or_ip);
    }
}

// -------------- N/W related things i am thinking -----------------------
/*
    -> i can get either a Ip or a Domain i can add them simply but i will validate them i will do advance dns lookups in later version, for now simple is better
*/

bool is_valid_ip(char const *ip_addr)
{
    return true;
}

bool is_valid_domain(char const *domain)
{
    return true;
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
        char const *domain_or_ip = argv[2]; // it can have more good name....but idk what to call it :0

        if (!is_valid_domain(domain_or_ip))
        {
            if (!is_valid_ip(domain_or_ip))
            {
                fprintf(stderr, "Invalid Domain/IP");
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
    }

    save_to_file();

    // here i am cleaning up all the memory used
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
    // code i will use maybe


    ----- FOR IP -----
    https://stackoverflow.com/questions/791982/determine-if-a-string-is-a-valid-ipv4-address-in-c
    https://thelinuxchannel.org/2023/12/c-code-to-check-valid-ip-address-ipv4-live-demo-and-example/
    // Source - https://stackoverflow.com/a/792016
    // Posted by Bill the Lizard, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-06-05, License - CC BY-SA 3.0
    bool isValidIpAddress(char *ipAddress)
    {
        struct sockaddr_in sa;
        int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
        return result != 0;
    }

    ----- FOR DOMAIN -----


*/