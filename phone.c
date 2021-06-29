#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sox.h>
#include <unistd.h>
#include <assert.h>
#define N 22050
void die(char *s){
    perror(s);
    exit(1);
}
void serv(int argc, char *argv[]){
    fprintf(stderr, "I'm SERVER.\n");
    int port = 0;
    char data_p[N];
    int ss = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(ss, (struct sockaddr *)&addr, sizeof(addr));
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int g = getsockname(ss, (struct sockaddr *)&addr, &addrlen);
    if(g == -1)die("getsockname");
    fprintf(stderr, "%d\n", ntohs(addr.sin_port));
    listen(ss, 10);
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int s = accept(ss, (struct sockaddr *)&client_addr, &len);
    if(s == -1)die("accept");
    close(ss);
    size_t n = 44100 * 0.5;
    sox_sample_t * buf_r = malloc(sizeof(sox_sample_t) * n);
    sox_init();
    sox_format_t * ft_r = sox_open_read("default", 0, 0, "pulseaudio");
    while(1){
        size_t m = sox_read(ft_r, buf_r, n);
        assert(m == n);
        size_t i;
        short * sbuf = malloc(sizeof(short) * n/2);
        for (i = 0; i < n/2; i++){
            sbuf[i] = (buf_r[2 * i] >> 16);
        }
        ssize_t se = send(s, sbuf, n, 0);
        if (se == -1)die("send");
        ssize_t play = recv(s, data_p, N, 0);
        if(play == -1) die("recv");
        for (int i = 0; i < play; i++){
            printf("%c", data_p[i]);
        }
    }
    sox_close(ft_r);
}
void client(int argc, char *argv[]){
    fprintf(stderr, "I'm CLIENT.\n");
    char * ip = argv[1];
    int port = atoi(argv[2]);
    char data_s[N];
    char data_r[N];
    int s = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_aton(ip, &addr.sin_addr);
    addr.sin_port = htons(port);
    int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    while(1){
        int n;
        while(n != N){
            n = read(0, data_s, 1);
        }
        if(n == -1) die("read");
        if(n == 0) break;
        ssize_t se = send(s, data_s, n, 0);
        ssize_t re = recv(s, data_r, N, 0);
        if(re == -1) die("recv");
        if(re == 0) break;
        for(int i = 0; i < re; i++){
            printf("%c", data_r[i]);
        }
    }
    shutdown(s, SHUT_WR);
}
int main(int argc, char *argv[]){
    if(argc == 1){
        serv(argc, argv);
    }
    else if(argc == 3){
        client(argc, argv);
    }
    else{
        printf("bad usage\n");
    }
    return 0;
}
