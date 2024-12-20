#include <arpa/inet.h> // for add6
#include <netinet/in.h>
#include <pthread.h>    // for threads
#include <stdio.h>      // for printf()
#include <stdlib.h>     // for malloc()
#include <string.h>     // for strlen()
#include <sys/socket.h> // for socket()
#include <sys/types.h>
#include <termios.h> // for terminal input mode (only works on linux afaik)
#include <time.h>    // for time()
#include <unistd.h>  // for read()

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
#define SKUL "☠"
#define SPED "🜛"
#define SNEK "⯀"

#define FOOD 1
#define SPEED 2
#define DEATH 3

#define REDO 'r'
#define QUIT 'q'

#define FORE 'w'
#define BACK 's'
#define LEFT 'a'
#define RITE 'd'

#define STARTING_SPEED 150000 // in nanoseconds
#define QUEUE_SIZE 3          // move queue size
#define NUM_APPLES 5

struct sockaddr_in6 address;

typedef struct node {
    int x, y;
    struct node *next;
} *node;

typedef struct food {
    int x, y;
    int type;
} *food;

typedef struct game_state {
    node snek;
    int dir;
    food apples[NUM_APPLES];
    int q[QUEUE_SIZE];
    int speed;
} *game_state;

void enqueue(int *q, int val) {
    int index = QUEUE_SIZE - 1;
    for (int i = 0; i < QUEUE_SIZE - 1; i++) {
        if (q[i] == 0) {
            index = i;
            break;
        }
    }
    if (index <= QUEUE_SIZE - 1)
        q[index] = val;
}

int dequeue(int *q) {
    if (q[0] == 0) {
        return 0; // Queue is empty
    }

    q[QUEUE_SIZE - 1] = 0;
    int val = q[0];
    for (int i = 0; i < QUEUE_SIZE - 1; i++) {
        q[i] = q[i + 1];
    }
    q[QUEUE_SIZE - 1] = 0;
    return val;
}

void print_node(node n) {
    printf("ptr: %p\n", n);
    printf("x: %d\n", n->x);
    printf("y: %d\n", n->y);
    printf("next: %p\n", n->next);
}

void print_chain(node n) {
    node current = n;
    while (current != NULL) {
        print_node(current);
        current = current->next;
    }
}

node append_head(node head, int x, int y) {
    node new_head = malloc(sizeof(struct node));
    if (new_head == NULL) {
        perror("Failed to allocate memory for append");
        exit(-1);
    }
    new_head->x = x;
    new_head->y = y;
    new_head->next = head;
    return new_head;
}

void pop_tail(node head) {
    if (head == NULL || head->next == NULL) {
        return;
    }

    node current = head;
    node prev = NULL;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    if (prev != NULL) {
        prev->next = NULL;
    }

    free(current);
}

bool insnake(node snek, int x, int y) {
    node current = snek;
    while (current != NULL) {
        if (current->x == x && current->y == y) {
            return TRUE;
        }
        current = current->next;
    }
    return FALS;
}

void rand_apple(game_state state, int index);

bool infood(game_state state, int x, int y, bool eat) {
    for (int i = 0; i < NUM_APPLES; i++) {
        if (state->apples[i]->x == x && state->apples[i]->y == y) {
            int retval = state->apples[i]->type;
            if (eat) {
                rand_apple(state, i);
            }
            return retval;
        }
    }
    return FALS;
}

int randdir() {
    int randomNum = rand() % 4;
    switch (randomNum) {
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

void rand_apple(game_state state, int index) {
    int x, y;
    do {
        x = rand() % (WIDE - 2) + 1;
        y = rand() % (HIGH - 2) + 1;
    } while (insnake(state->snek, x, y) || infood(state, x, y, FALS));

    int r;
    switch (state->apples[index]->type) {
    case DEATH:
        r = rand() % 100;
        if (r < 25) {
            state->apples[index]->type = FOOD;
        }
        break;
    case FOOD:
        r = rand() % 100;
        if (r < 25) {
            state->apples[index]->type = DEATH;
        }
        break;
    }
    state->apples[index]->x = x;
    state->apples[index]->y = y;
}

void start_game(game_state state) {
    
    int x = rand() % ((int)(WIDE * 0.75) - (int)(WIDE * 0.25)) + (int)(WIDE * 0.25);
    int y = rand() % ((int)(HIGH * 0.75) - (int)(HIGH * 0.25)) + (int)(HIGH * 0.25);

    state->snek = malloc(sizeof(struct node));
    if (state->snek == NULL) {
        perror("Failed to allocate memory for snek");
        exit(-1);
    }

    state->snek->x = x;
    state->snek->y = y;
    state->snek->next = NULL;
    state->dir = randdir();

    for (int i = 0; i < NUM_APPLES; i++) {
        state->apples[i] = malloc(sizeof(struct food));
        if (state->apples[i] == NULL) {
            perror("Failed to allocate memory for food");
            exit(-1);
        }
        state->apples[i]->type = FOOD;
        state->apples[i]->x = 0;
        state->apples[i]->y = 0;
    }
    state->apples[0]->type = SPEED;
    for (int i = 0; i < NUM_APPLES; i++) {
        rand_apple(state, i);
    }
    state->speed = STARTING_SPEED;

    for (int i = 0; i < QUEUE_SIZE; i++) {
        state->q[i] = 0;
    }
}

void move(game_state state) {
    node head = state->snek;
    int last_move = dequeue(state->q);
    if (last_move != 0) {
        state->dir = last_move;
    }
    int x = head->x;
    int y = head->y;
    switch (state->dir) {
    case FORE:
        y--;
        break;
    case BACK:
        y++;
        break;
    case LEFT:
        x--;
        break;
    case RITE:
        x++;
        break;
    }

    if (x == 0 || x == WIDE - 1 || y == 0 || y == HIGH - 1 || insnake(head->next, x, y)) {
        printf("--------\nGAME OVER\n--------\n");
        printf("Press 'q' to quit\n\n");
        printf("Starting over in 3..");
        fflush(stdout);
        usleep(750000);
        printf("2..");
        fflush(stdout);
        usleep(750000);
        printf("1..");
        fflush(stdout);
        usleep(750000);
        start_game(state);
    } else {
        int type = infood(state, x, y, TRUE);
        switch (type) {
        case FOOD:
            state->snek = append_head(state->snek, x, y);
            break;
        case SPEED:
            state->snek = append_head(state->snek, x, y);
            state->speed = (int)(state->speed * 0.92);
            pop_tail(state->snek);
            for (int i = 0; i < NUM_APPLES; i++) {
                if (state->apples[i]->type == DEATH) {
                    state->apples[i]->type = FOOD;
                }
            }
            break;
        case DEATH:
            state->snek = append_head(state->snek, x, y);
            pop_tail(state->snek);
            pop_tail(state->snek);
            break;
        default:
            state->snek = append_head(state->snek, x, y);
            pop_tail(state->snek);
            break;
        }
    }
}

void render(game_state state) {
    char *screen[HIGH][WIDE];
    for (int i = 0; i < HIGH; i++) {
        for (int j = 0; j < WIDE; j++) {
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
    while (current != NULL) {
        screen[current->y][current->x] = SNEK;
        current = current->next;
    }

    for (int i = 0; i < NUM_APPLES; i++) {
        switch (state->apples[i]->type) {
        case FOOD:
            screen[state->apples[i]->y][state->apples[i]->x] = SNAK;
            break;
        case SPEED:
            screen[state->apples[i]->y][state->apples[i]->x] = SPED;
            break;
        case DEATH:
            screen[state->apples[i]->y][state->apples[i]->x] = SKUL;
            break;
        }
    }

    for (int i = 0; i < HIGH; i++) {
        for (int j = 0; j < WIDE; j++) {
            printf("%s", screen[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void *server_loop(void *gstate) {
    game_state state = (game_state)gstate;
    while (TRUE) {
        move(state);
        render(state);
        usleep(state->speed);
    }
}

void free_mem(game_state state) {
    node current = state->snek;
    while (current != NULL) {
        node temp = current;
        current = current->next;
        free(temp);
    }
    free(current);
    for (int i = 0; i < NUM_APPLES; i++) {
        free(state->apples[i]);
    }
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        printf("%s, expects (1) arg, %d provided", argv[0], argc - 1);
        return 1;
    }
    srand(time(NULL));
    char mode;
    switch (argv[1][1]) {
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
    address.sin6_flowinfo = 0; // valgrind
    address.sin6_scope_id = 0; // valgrind
    char buff[SIZE];
    memset(buff, 0, SIZE);

    if (mode == CLIENT) {
        printf("Starting client...\n");
        inet_pton(DOMAIN, "::1", &address.sin6_addr);
        if (connect(sock, (struct sockaddr *)&address, addr_size)) {
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

        while (TRUE) {
            int numRead = read(0, buff, SIZE);
            if (numRead > 0) {
                buff[numRead] = '\0';
                write(sock, buff, strlen(buff));
                if (buff[0] == 'q') {
                    tcsetattr(STDIN_FILENO, TCSANOW, &original_t);
                    close(sock);
                    printf("\nQUIT\n");
                    exit(0);
                }
                prev = buff[0];
            }
        }
    }

    if (mode == SERVER) {
        printf("Starting server...\n");
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            perror("Server - setsockopt addr failed.\n");
            exit(-1);
        }
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("Server - setsockopt port failed.\n");
            exit(-1);
        }
        address.sin6_addr = in6addr_any;
        if (bind(sock, (const struct sockaddr *)&address, addr_size) == -1) {
            fprintf(stderr, "Binding failed.\n");
            exit(-1);
        }
        if (listen(sock, 1) == -1) {
            fprintf(stderr, "Listen failed.\n");
            exit(-1);
        }
        int conx = accept(sock, (struct sockaddr *restrict)&address, &addr_size);
        if (conx == -1) {
            fprintf(stderr, "Accept failed.\n");
            exit(-1);
        }

        printf("      _____           _____          _____        _____          ____               _____        _____          ____     _____            ____        \n");
        printf("  ___|\\    \\     ____|\\    \\    ___|\\     \\   ___|\\    \\    ____|\\   \\          ___|\\    \\   ___|\\    \\    ____|\\   \\   |\\    \\   _____  |    |       \n");
        printf(" /    /\\    \\   /     /\\    \\  |    |\\     \\ |    |\\    \\  /    /\\    \\        /    /\\    \\ |    |\\    \\  /    /\\    \\  | |    | /    /| |    |       \n");
        printf("|    |  |    | /     /  \\    \\ |    | |     ||    | |    ||    |  |    |      |    |  |    ||    | |    ||    |  |    | \\/     / |    || |    |       \n");
        printf("|    |  |____||     |    |    ||    | /_ _ / |    |/____/ |    |__|    |      |    |  |____||    |/____/ |    |__|    | /     /_  \\   \\/ |    |  ____ \n");
        printf("|    |   ____ |     |    |    ||    |\\    \\  |    |\\    \\ |    .--.    |      |    |   ____ |    |\\    \\ |    .--.    ||     // \\  \\   \\ |    | |    |\n");
        printf("|    |  |    ||\\     \\  /    /||    | |    | |    | |    ||    |  |    |      |    |  |    ||    | |    ||    |  |    ||    |/   \\ |    ||    | |    |\n");
        printf("|\\ ___\\/    /|| \\_____\\/____/ ||____|/____/| |____| |____||____|  |____|      |\\ ___\\/    /||____| |____||____|  |____||\\ ___/\\   \\|   /||____|/____/|\n");
        printf("| |   /____/ | \\ |    ||    | /|    /     || |    | |    ||    |  |    |      | |   /____/ ||    | |    ||    |  |    || |   | \\______/ ||    |     ||\n");
        printf(" \\|___|    | /  \\|____||____|/ |____|_____|/ |____| |____||____|  |____|       \\|___|    | /|____| |____||____|  |____| \\|___|/\\ |    | ||____|_____|/\n");
        printf("      |____|/                                                                       |____|/                                     \\|____|/              \n\n");

        printf("Eat ❦ to grow longer\n");
        printf("Avoid walls and ☠\n");
        printf("Eat 🜛 to speed up and turn ☠  into ❦\n");
        printf("Use wasd to move\n");
        printf("Press r to restart\n");
        printf("Press q to quit\n\n");
        printf("Press any key to start\n");

        while (TRUE) {
            char ch;
            int numRead = read(conx, &ch, 1);
            if (numRead > 0) {
                if (ch != 'q')
                    break;
                printf("\nQUIT\n");
                close(conx);
                close(sock);
                exit(0);
            }
        }

        game_state state = malloc(sizeof(struct game_state));
        if (state == NULL) {
            perror("Failed to allocate memory for game state");
            exit(-1);
        }

        start_game(state);

        pthread_t tid;
        pthread_create(&tid, NULL, &server_loop, (void *)state);

        while (TRUE) {
            char ch;
            read(conx, &ch, 1);
            switch (ch) {
            case FORE:
                enqueue(state->q, FORE);
                break;
            case BACK:
                enqueue(state->q, BACK);
                break;
            case LEFT:
                enqueue(state->q, LEFT);
                break;
            case RITE:
                enqueue(state->q, RITE);
                break;
            case REDO:
                free_mem(state);
                start_game(state);
                break;
            case QUIT:
                printf("\nQUIT\n");
                close(conx);
                close(sock);
                free_mem(state);
                free(state);
                exit(0);
            }
        }
    }
}
