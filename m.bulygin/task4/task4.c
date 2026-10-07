#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

struct Node
{
    char *str;
    struct Node *next;
};

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
        if (buffer[0] == '.')
        {
            break;
        }

        len = strlen(buffer);

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
