#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int create_connection();
int do_action(int sock, const char *tag, char *content);
int login(int sock, const char *uname, const char *passwd);
int change_board(int sock, const char *board_name);
int set_title(int sock, const char *title);
int post(int sock, const char *content);
void logout(int sock);

int create_connection() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket : ");
        exit(1);
    }

    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags=AI_PASSIVE
    };

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

int do_action(int sock, const char *tag, char* content) {
    static char username[32], password[32];
    if(!strncmp(tag, "ID", 2)) {
        strncpy(username, content, 32);
    } else if(!strncmp(tag, "PASS", 4)) {
        strncpy(password, content, 32);
        login(sock, username, password);
    } else if(!strncmp(tag, "BOARD", 5)) {
        change_board(sock, content);
    } else if(!strncmp(tag, "P", 1)) {
        set_title(sock, content);
    } else if(!strncmp(tag, "CONTENT", 7)) {
        char *p = content;
        while((p = strchr(p, '\n')))
            *p = '\r';
        post(sock, content);
    }
    sleep(1);
    return 0;
}

int login(int sock, const char *uname, const char *passwd) {
    dprintf(sock, "\b\b\b\b\b\b\b\b\b\b");
    // username -> , -> \r -> password -> \r
    dprintf(sock, "%s,\r%s\r", uname, passwd);
    dprintf(sock, "\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03");
    return 0;
}

int change_board(int sock, const char *board_name) {
    dprintf(sock, "\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03");
    // s -> board_name -> \r
    dprintf(sock, "s%s\r", board_name);
    dprintf(sock, "\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03");
    return 0;
}

int set_title(int sock, const char *title) {
    dprintf(sock, "\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03");
    // ^P -> \r -> title -> \r
    dprintf(sock, "\x10\r%s\r", title);
    return 0;
}

int post(int sock, const char *content) {
    dprintf(sock, "\b\b\b\b\b\b\b\b\b\b");
    // content -> ^x -> s -> enter -> etner
    dprintf(sock, "%s\x18s\r\r", content);
    return 0;
}

void logout(int sock) {
    // left key * 10
    dprintf(sock, "\033[D\033[D\033[D\033[D\033[D\033[D\033[D\033[D\033[D\033[D");
    sleep(1);
    // right key -> y -> \r
    dprintf(sock, "\033[Cy\r");
    sleep(1);
    exit(0);
}

int main(void) {
    int conn = create_connection();
    FILE *fp = fopen("P_input.txt", "r");
    if(fp == NULL) {
        fprintf(stderr, "P_input.txt open failed\n");
        exit(1);
    }
    
    char content[4096];
    char buf[8192];
    char tag[32], close_tag[32];
    char *buf_p;

    int closed = 1;
    for(;;) {
        if(closed) {
            fgets(buf, 4096, fp);
            if(!strncmp(buf, "<EXIT>", 6))
                logout(conn);

            sscanf(buf, "<%[^>]", tag);
            snprintf(close_tag, 32, "</%s>", tag);

            content[0] = '\0';
            buf_p = buf + 2 + strlen(tag);
            closed = 0;
        } else {
            char *end_tag_p;
            if((end_tag_p = strstr(buf_p, close_tag))) {
                strncat(content, buf_p, end_tag_p - buf_p);
                do_action(conn, tag, content);
                closed = 1;
            } else {
                strcat(content, buf_p);
                fgets(buf, 4096, fp);
                buf_p = buf;
            }
        }
    }
    return 0;
}
