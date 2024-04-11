// #include <stdio.h>

// // A simple program to sort an array of integers in C

// /* YOUR WORK HERE */

// void printArray(int *arr, int len)
// {
//     for (int i = 0; i < len; i++)
//     {
//         printf("%d ", arr[i]);
//     }
//     printf("\n");
// }

// // Input: An array of integers and its length
// // Output: Nothing
// // Side Effect: The input array is sorted from least to greatest
// void sort(int *arr, size_t siz)
// {
//     int *tmp = malloc(siz * sizeof(int));
// }

// /* END YOUR WORK */

// /* FEEDBACK FUNCTIONS */

// // Input: An array of integers and its length
// // Output: 1 if the array is sorted least to greatest, 0 otherwise
// // Side Effect: None
// int is_sorted(int *arr, int len)
// {
//     int i = 0;
//     while (i < (len - 2))
//     {
//         if (arr[i] > arr[i + 1])
//         {
//             return 0;
//         }
//         i++;
//     }
//     return 1;
// }

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
// uint64_t exp(uint64_t x, uint64_t n)
// {
//     uint64_t temp = x;
//     if (n == 0)
//         return 1;
//     for (int i = 0; i < n - 1; i++)
//     {
//         x *= temp;
//     }
//     return x;
// }

// uint64_t exp(uint64_t x, uint64_t n)
// {
//     if (n == 0)
//         return 1;
//     if (n == 1)
//         return x;
//     if (n == 2)
//         return x * x;
//     if (n & 1)
//     {
//         return x * exp(exp(x, 2), (n - 1) / 2);
//     }
//     return exp(exp(x, 2), n / 2);
// }

// char *strdup(char *s)
// {
//     char temp = s[0];
//     size_t siz = 0;
//     while (temp != '\0') {
//         siz += 1;
//         temp = s[siz];
//     }
//     siz += 1;
//     char *dupe = malloc(siz * sizeof(char));
//     for (int i = 0; i < siz; i++) {
//         dupe[i] = s[i];
//     }
//     return dupe;
// }

// typedef struct bst_struct *bst;

// struct bst_struct
// {
//     int data;
//     bst less;
//     bst more;
// } 

// void *max(bst t)
// {
//     while (t) {
//         t = t->more;
//     }
//     return t->data;
// }
// int main()
// {
//     char * w = "hi";
//     char * ww = strdup(w);
//     printf("%s", ww);
//     free(ww);
// }


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DOMAIN AF_INET6
#define PORT 8080

int sock = socket(DOMAIN, SOCK_STREAM, 0);

struct sockaddr_in6 addr;

addr.sin6family = DOMAIN;
addr.sin6port = htons(PORT);
SERVER: addr.sin6_addr = in6addr_any;
CLIENT: inet_pton(DOMAIN, "::1", &addr.sin6addr);

if (bind(sock, add4, s))
{
    fprintf(stderr, "Binding failed.\n");
    exit(-1);
}
if (listen(sock, 1))
{
    fprintf(stderr, "Listen failed.\n");
    exit(-1);
}
conx = accept(sock, add4, &s);
if (conx == -1)
{
    fprintf(stderr, "Accept failed.\n");
    exit(-1);
}