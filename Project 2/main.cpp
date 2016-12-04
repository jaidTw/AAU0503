#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int create_connection();
int login(int sock, const char *uname, const char *passwd);
void* DEBUG_recvall(void *arg);
int change_board();
int post();
int logout();
int parse();

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

    write(sock, (void *)uname, strlen(uname));
    write(sock, (void *)"\n", 1);
    write(sock, (void *)passwd, strlen(passwd));
    write(sock, (void *)"\n", 1);
    
    printf("\n結束\n");
    return 0;
}

void* DEBUG_recvall(void *arg) {
    int sock = *(int *)arg;
    //system("clear");
    char buf[4096];
    int recv_size;
    int size_cnt = 0;
    for(;;) {
        recv_size = read(sock, (void *)buf, sizeof(buf));
        printf("%s", buf);
        
        //for(int i = 0; i <= recv_size; ++i) {
        //    printf("%x ", (char)buf[i]);
        //}
        
        size_cnt += recv_size;
        //printf("\n\n\n\n\n\n\n\n*********************** %x **********************\n\n\n\n\n\n\n\n", buf[recv_size - 1]);
        //printf("SIZE : %d\n", size_cnt);
    }
    printf("return\n");
    return 0;
}

int main(void) {
    int conn = create_connection();
    pthread_t tid;
    pthread_create(&tid, NULL, DEBUG_recvall, (void *)&conn);
    //login(conn, "jaid,", "");
    write(conn, "\b", 1);
    write(conn, "\b", 1);
    write(conn, "\b", 1);
    write(conn, "\b", 1);
    write(conn, "\b", 1);
    write(conn, "\b", 1);
    write(conn, "\b", 1);
    write(conn, "j", 1);
    write(conn, "a", 1);
    write(conn, "i", 1);
    write(conn, "d", 1);
    write(conn, ",", 1);
    write(conn, "\r", 1);
    system("clear");
    write(conn, "1", 1);
    write(conn, "2", 1);
    write(conn, "3", 1);
    write(conn, "4", 1);
    write(conn, "5", 1);
    write(conn, "\r", 1);
    system("clear");
    write(conn, "\03", 1);
    write(conn, "\03", 1);
    write(conn, "\03", 1);
    write(conn, "\03", 1);
    write(conn, "\03", 1);
    write(conn, "\03", 1);
    write(conn, "\03", 1);
    write(conn, "\03", 1);
    system("clear");
    write(conn, "s", 1);
    write(conn, "t", 1);
    write(conn, "e", 1);
    write(conn, "s", 1);
    write(conn, "t", 1);
    write(conn, "\r", 1);
    system("clear");
    write(conn, "\03", 1);
    write(conn, "\03", 1);
    write(conn, "\03", 1);
    sleep(2);
    system("clear");
    write(conn, "\03", 1);
    for(;;) {
        //char buf[2048];
        //scanf("%s", buf);
        //for(int i = 0; i < strlen(buf); ++i) {
        //    write(conn, buf+i, 1);
        //}
    }
    return 0;
}
