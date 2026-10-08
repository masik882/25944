#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

struct Node
{
    char *str;
    struct Node *next;
};

void remove_escape_sequences(char *str)
{
    int read_pos = 0;
    int write_pos = 0;

    while (str[read_pos] != '\0')
    {
        if ((unsigned char)str[read_pos] == 27)
        {
            if (str[read_pos + 1] == '[')
            {
                if (str[read_pos + 2] == 'A' ||
                    str[read_pos + 2] == 'B' ||
                    str[read_pos + 2] == 'C' ||
                    str[read_pos + 2] == 'D')
                {
                    read_pos += 3;
                    continue;
                }

                read_pos += 2;
                continue;
            }

            if (str[read_pos + 1] == 'O')
            {
                if (str[read_pos + 2] == 'A' ||
                    str[read_pos + 2] == 'B' ||
                    str[read_pos + 2] == 'C' ||
                    str[read_pos + 2] == 'D')
                {
                    read_pos += 3;
                    continue;
                }

                read_pos += 2;
                continue;
            }

            read_pos++;
            continue;
        }

        str[write_pos] = str[read_pos];
        write_pos++;
        read_pos++;
    }

    str[write_pos] = '\0';
}

int main()
{
    char buffer[BUFFER_SIZE];
    size_t len;

    struct Node *head = NULL;
    struct Node *tail = NULL;
    struct Node *new_node;
    struct Node *current;
    struct Node *temp;

    printf("Enter strings ('.' to stop):\n");

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
    {
        remove_escape_sequences(buffer);

        if (buffer[0] == '.')
        {
            break;
        }

        len = strlen(buffer);

        if (len == 0 || (len == 1 && buffer[0] == '\n'))
        {
            continue;
        }

        new_node = malloc(sizeof(struct Node));

        if (new_node == NULL)
        {
            perror("malloc");
            return 1;
        }

        new_node->str = malloc(len + 1);

        if (new_node->str == NULL)
        {
            perror("malloc");
            free(new_node);
            return 1;
        }

        strcpy(new_node->str, buffer);

        new_node->next = NULL;

        if (head == NULL)
        {
            head = new_node;
            tail = new_node;
        }
        else
        {
            tail->next = new_node;
            tail = new_node;
        }
    }

    printf("\nList:\n");

    current = head;

    while (current != NULL)
    {
        printf("%s", current->str);
        current = current->next;
    }

    current = head;

    while (current != NULL)
    {
        temp = current;
        current = current->next;

        free(temp->str);
        free(temp);
    }

    return 0;
}
