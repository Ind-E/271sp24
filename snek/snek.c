#include <sys/socket.h> // for socket()
#include <arpa/inet.h>  // for add6
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>   // for printf()
#include <unistd.h>  // for read()
#include <stdlib.h>  // for malloc()
#include <string.h>  // for strlen()
#include <time.h>    // for time()
#include <pthread.h> // for threads
#include <termios.h> // for terminal input mode

#define DOMAIN AF_INET6

#define TRUE 1
#define FALS 0
typedef int bool;

#define PORT htons(0xC271) // get it? CS 271?
#define DOMAIN AF_INET6    // ipv6
#define LOOPBACK "::1"
#define SIZE 1024

#define SERVER 1
#define CLIENT 0

#define HIGH 23
#define WIDE 80

#define SNAK "❦"
#define EVIL "☠"
#define SPED "🜛"
#define SNEK "⯀"

#define REDO 'r'
#define QUIT 'q'

#define FORE 'w'
#define BACK 's'
#define LEFT 'a'
#define RITE 'd'

#define STARTING_SPEED 150000 // in nanoseconds
#define QUEUE_SIZE 3          // move queue size

struct sockaddr_in6 address;

typedef struct node *node;
struct node
{
    int x, y;
    struct node *next;
};

typedef struct game_state *game_state;
struct game_state
{
    node snek;
    int dir;
    int apple[2];
    int q[QUEUE_SIZE];
    int speed;
};

void enqueue(int *q, int val)
{
    int index = QUEUE_SIZE - 1;
    for (int i = 0; i < QUEUE_SIZE - 1; i++)
    {
        if (q[i] == 0)
        {
            index = i;
            break;
        }
    }
    if (index <= QUEUE_SIZE - 1)
        q[index] = val;
}

int dequeue(int *q)
{
    if (q[0] == 0)
    {
        return 0; // Queue is empty
    }

    q[QUEUE_SIZE - 1] = 0;
    int val = q[0];
    for (int i = 0; i < QUEUE_SIZE - 1; i++)
    {
        q[i] = q[i + 1];
    }
    q[QUEUE_SIZE - 1] = 0;
    return val;
}

node append_head(node head, int x, int y)
{
    node new_head = malloc(sizeof(struct node));
    if (new_head == NULL)
    {
        perror("Failed to allocate memory for append");
        exit(-1);
    }
    new_head->x = x;
    new_head->y = y;
    new_head->next = head;
    return new_head;
}

void pop_tail(node head)
{
    if (head == NULL)
    {
        return;
    }

    node current = head;
    node prev = NULL;
    while (current->next != NULL)
    {
        prev = current;
        current = current->next;
    }

    if (prev != NULL)
    {
        prev->next = NULL;
    }

    free(current);
}

int isin(node snek, int x, int y)
{
    node current = snek;
    while (current != NULL)
    {
        if (current->x == x && current->y == y)
        {
            return TRUE;
        }
        current = current->next;
    }
    return FALS;
}

int randdir()
{
    int randomNum = rand() % 4;
    switch (randomNum)
    {
    case 0:
        return LEFT;
    case 1:
        return RITE;
    case 2:
        return FORE;
    case 3:
        return BACK;
    default:
        return -1;
    }
}

void init_snake(game_state state)
{
    int x = rand() % ((int)(WIDE * 0.75) - (int)(WIDE * 0.25)) + (int)(WIDE * 0.25);
    int y = rand() % ((int)(HIGH * 0.75) - (int)(HIGH * 0.25)) + (int)(HIGH * 0.25);

    state->snek = malloc(sizeof(struct node));
    if (state->snek == NULL)
    {
        perror("Failed to allocate memory for snek");
        exit(-1);
    }

    state->snek->x = x;
    state->snek->y = y;
    state->dir = randdir();

    // without this the first apple won't increase size
    node tail = malloc(sizeof(struct node));
    if (tail == NULL)
    {
        perror("Failed to allocate memory for tail");
        exit(-1);
    }

    switch (state->dir)
    {
    case FORE:
        tail->x = x;
        tail->y = y + 1;
        break;
    case BACK:
        tail->x = x;
        tail->y = y - 1;
        break;
    case LEFT:
        tail->x = x + 1;
        tail->y = y;
        break;
    case RITE:
        tail->x = x - 1;
        tail->y = y;
        break;
    }
    state->snek->next = tail;
}

void rand_apple(game_state state)
{
    int x, y;
    do
    {
        x = rand() % (WIDE - 2) + 1;
        y = rand() % (HIGH - 2) + 1;
    } while (isin(state->snek, x, y));
    state->apple[0] = x;
    state->apple[1] = y;
}

void start_game(game_state state)
{
    init_snake(state);
    rand_apple(state);
    state->speed = STARTING_SPEED;
}

void move(game_state state)
{
    node head = state->snek;
    int last_move = dequeue(state->q);
    if (last_move != 0)
    {
        state->dir = last_move;
    }
    switch (state->dir)
    {
    case FORE:
        head->y--;
        break;
    case BACK:
        head->y++;
        break;
    case LEFT:
        head->x--;
        break;
    case RITE:
        head->x++;
        break;
    }
    int x = head->x;
    int y = head->y;

    if (x == 0 || x == WIDE - 1 || y == 0 || y == HIGH - 1 || isin(head->next, x, y))
    {
        printf("--------\nGAME OVER\n--------\n");
        printf("Press 'q' to quit\n\n");
        printf("Starting over in 3..");
        fflush(stdout);
        sleep(1);
        printf("2..");
        fflush(stdout);
        sleep(1);
        printf("1..");
        fflush(stdout);
        sleep(1);
        start_game(state);
    }
    else if (x == state->apple[0] && y == state->apple[1])
    {
        state->snek = append_head(state->snek, x, y);
        rand_apple(state);
    }
    else
    {
        state->snek = append_head(state->snek, x, y);
        pop_tail(state->snek);
    }
}

void render(game_state state)
{
    char *screen[HIGH][WIDE];
    for (int i = 0; i < HIGH; i++)
    {
        for (int j = 0; j < WIDE; j++)
        {
            if (i == 0 && j == 0)
                screen[i][j] = "╔";
            else if (i == 0 && j == WIDE - 1)
                screen[i][j] = "╗";
            else if (i == HIGH - 1 && j == 0)
                screen[i][j] = "╚";
            else if (i == HIGH - 1 && j == WIDE - 1)
                screen[i][j] = "╝";
            else if (i == 0 || i == HIGH - 1)
                screen[i][j] = "═";
            else if (j == 0 || j == WIDE - 1)
                screen[i][j] = "║";
            else
                screen[i][j] = " ";
        }
    }
    node current = state->snek;
    while (current != NULL)
    {
        screen[current->y][current->x] = SNEK;
        current = current->next;
    }
    screen[state->apple[1]][state->apple[0]] = SNAK;

    for (int i = 0; i < HIGH; i++)
    {
        for (int j = 0; j < WIDE; j++)
        {
            printf("%s", screen[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void *server_loop(void *gstate)
{
    game_state state = (game_state)gstate;
    while (TRUE)
    {
        move(state);
        render(state);
        usleep(state->speed);
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("%s, expects (1) arg, %d provided", argv[0], argc - 1);
        return 1;
    }
    srand(time(NULL));
    char mode;

    switch (argv[1][1])
    {
    case 's':
        mode = SERVER;
        break;
    case 'c':
        mode = CLIENT;
        break;
    default:
        printf("Usage:  -s for server, -c for client\n");
    }

    int sock = socket(DOMAIN, SOCK_STREAM, 0), opt = 1;
    struct sockaddr_in6 address;
    socklen_t addr_size = sizeof(struct sockaddr_in6);
    address.sin6_family = DOMAIN;
    address.sin6_port = htons(PORT);
    char buff[SIZE];
    memset(buff, 0, SIZE);

    if (mode == CLIENT)
    {
        printf("Starting client...\n");
        inet_pton(DOMAIN, "::1", &address.sin6_addr);

        if (connect(sock, (const struct sockaddr *)&address, addr_size))
        {
            perror("Client - conect failed.");
            exit(-1);
        }

        // change terminal input mode to allow for reading input without needing to press enter (os dependent)
        struct termios original_t, new_t;
        tcgetattr(STDIN_FILENO, &original_t);
        new_t = original_t;
        new_t.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_t);
        char prev = '\0';

        while (TRUE)
        {
            int numRead = read(0, buff, SIZE);
            if (numRead > 0)
            {
                buff[numRead] = '\0';

                write(sock, buff, strlen(buff));

                if (buff[0] == 'q')
                {
                    tcsetattr(STDIN_FILENO, TCSANOW, &original_t);
                    close(sock);
                    printf("\nQUIT\n");
                    exit(0);
                }
                prev = buff[0];
            }
        }
    }

    if (mode == SERVER)
    {

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        {
            perror("Server - setsockopt addr failed.\n");
            exit(-1);
        }
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("Server - setsockopt port failed.\n");
            exit(-1);
        }

        address.sin6_addr = in6addr_any;
        printf("Starting server...\n");
        if (bind(sock, (const struct sockaddr *)&address, addr_size) == -1)
        {
            fprintf(stderr, "Binding failed.\n");
            exit(-1);
        }
        if (listen(sock, 1) == -1)
        {
            fprintf(stderr, "Listen failed.\n");
            exit(-1);
        }
        int conx = accept(sock, (struct sockaddr *restrict)&address, &addr_size);
        if (conx == -1)
        {
            fprintf(stderr, "Accept failed.\n");
            exit(-1);
        }

        printf(" ________  ________  _______   _______   ________          ________  ________   _______   ___  __       \n");
        printf("|\\   ____\\|\\   __  \\|\\  ___ \\ |\\  ___ \\ |\\   ___ \\        |\\   ____\\|\\   ___  \\|\\  ___ \\ |\\  \\|\\  \\     \n");
        printf("\\ \\  \\___|\\ \\  \\|\\  \\ \\   __/|\\ \\   __/|\\ \\  \\_|\\ \\       \\ \\  \\___|\\ \\  \\\\ \\  \\ \\   __/|\\ \\  \\/  /|_   \n");
        printf(" \\ \\_____  \\ \\   ____\\ \\  \\_|/_\\ \\  \\_|/_\\ \\  \\ \\\\ \\       \\ \\_____  \\ \\  \\\\ \\  \\ \\  \\_|/_\\ \\   ___  \\  \n");
        printf("  \\|____|\\  \\ \\  \\___|\\ \\  \\_|\\ \\ \\  \\_|\\ \\ \\  \\_\\\\ \\       \\|____|\\  \\ \\  \\\\ \\  \\ \\  \\_|\\ \\ \\  \\\\ \\  \\ \n");
        printf("    ____\\_\\  \\ \\__\\    \\ \\_______\\ \\_______\\ \\_______\\        ____\\_\\  \\ \\__\\\\ \\__\\ \\_______\\ \\__\\\\ \\__\\\n");
        printf("   |\\_________\\|__|     \\|_______|\\|_______|\\|_______|       |\\_________\\|__| \\|__|\\|_______|\\|__| \\|__|\n");
        printf("   \\|_________|                                              \\|_________|                               \n\n");

        printf("Eat ❦ to grow longer\n");
        printf("Avoid walls and ☠\n");
        printf("Eat 🜛 to speed up and turn ☠  into ❦\n");
        printf("Use 'w', 'a', 's', 'd' to move\n");
        printf("Use 'r' to restart\n");
        printf("Use 'q' to quit\n\n");

        printf("Press any key to start\n");

        while (TRUE)
        {
            char ch;
            int numRead = read(conx, &ch, 1);
            if (numRead > 0)
            {
                if (ch == 'q')
                {
                    printf("\nQUIT\n");
                    close(conx);
                    close(sock);
                    exit(0);
                }
                else
                {
                    break;
                }
            }
        }

        game_state state = malloc(sizeof(struct game_state));
        if (state == NULL)
        {
            perror("Failed to allocate memory for game state");
            exit(-1);
        }

        start_game(state);

        pthread_t tid;
        pthread_create(&tid, NULL, &server_loop, (void *)state);

        while (TRUE)
        {
            char ch;
            int numRead = read(conx, &ch, 1);
            switch (ch)
            {
            case FORE:
                enqueue(state->q, FORE);
                // state->dir = FORE;
                break;
            case BACK:
                enqueue(state->q, BACK);
                // state->dir = BACK;
                break;
            case LEFT:
                enqueue(state->q, LEFT);
                // state->dir = LEFT;
                break;
            case RITE:
                enqueue(state->q, RITE);
                // state->dir = RITE;
                break;
            case REDO:
                start_game(state);
                break;
            case QUIT:
                printf("\nQUIT\n");
                close(conx);
                close(sock);
                free(state);
                exit(0);
            }
        }
    }
}
