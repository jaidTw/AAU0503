#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* DEBUG_recvall(void *arg);
int create_connection();
int login(int sock, const char *uname, const char *passwd);
int change_board(int sock, char *board_name);
int set_title(int sock, char *title);
int post(int sock, char *post);
int logout(int sock);

int create_connection() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket : ");
        exit(1);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *srvinf;
    if(getaddrinfo("ptt.cc", "telnet", &hints, &srvinf) < 0) {
        perror("getaddrinfo : ");
        exit(1);
    }

    if(connect(sock, srvinf->ai_addr, sizeof(*(srvinf->ai_addr))) < 0) {
        perror("connect : ");
        exit(1);
    }

    freeaddrinfo(srvinf);
    return sock;
}

int login(int sock, const char *uname, const char *passwd) {
    printf("login with uname='%s', passwd='%s'\n", uname, passwd);
    write(sock, "\b\b\b\b\b\b\b\b\b\b", 10);
    write(sock, (void *)uname, strlen(uname));
    write(sock, (void *)",\r", 2);
    write(sock, (void *)passwd, strlen(passwd));
    write(sock, (void *)"\r", 1);
    sleep(1);
    write(sock, (void *)"\03\03\03\03\03\03\03\03\03\03", 10);
    sleep(1);
    
    return 0;
}

void* DEBUG_recvall(void *arg) {
    int sock = *(int *)arg;
    char buf[4096];
    for(;;) {
        read(sock, (void *)buf, sizeof(buf));
        printf("%s", buf);
        
    }
    return 0;
}

int change_board(int sock, char *board_name) {
    write(sock, (void *)"\03\03\03\03\03\03\03\03\03\03", 10);
    write(sock, (void *)"s", 1);
    write(sock, (void *)board_name, strlen(board_name));
    write(sock, (void *)"\r", 1);
    sleep(1);
    write(sock, (void *)"\03\03\03\03\03\03\03\03\03\03", 10);
    sleep(1);
    return 0;
}

int set_title(int sock, char *title) {
    return 0;
}

int set_content(int sock, char *post) {
    return 0;
}

int logout(int sock) {
    write(sock, "", 0);
    sleep(1);
    write(sock, "", 0);
    sleep(1);
    return 0;
}

int main(void) {
    int conn = create_connection();
    FILE *fp = fopen("P_input.txt", "r");
    if(fp == NULL) {
        fprintf(stderr, "P_input.txt open failed\n");
        exit(-1);
    }
    pthread_t tid;
    pthread_create(&tid, NULL, DEBUG_recvall, (void *)&conn);
    
    char tag[1024];
    char content[4096];
    
    char username[32];
    char password[32];
    fscanf(fp, "<%[^>]>%[^<]</%*[^>]>\n", tag, username);
    fscanf(fp, "<%[^>]>%[^<]</%*[^>]>\n", tag, password);

    login(conn, username, password);
    


    for(;;) {
        fscanf(fp, "<%[^>]>%[^<]</%*[^>]>\n", tag, content);
        if(!strncmp(tag, "EXIT", 4)) {
            return 0;
            logout(conn);
        }

        if(!strncmp(tag, "BOARD", 5)) {
            change_board(conn, content);
            fscanf(fp, "<%[^>]>%[^<]</%*[^>]>\n", tag, content);
            set_title(conn, content);
            fscanf(fp, "<%[^>]>%[^<]</%*[^>]>\n", tag, content);
            set_content(conn, content);
        }
    }

    return 0;
}
