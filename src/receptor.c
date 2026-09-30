/* receptor.c - practica 2 capa 2
 * escucha tramas 0x88B5 en eth0 y las imprime
 * ctrl+c para salir
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <arpa/inet.h>

#define IFACE "eth0"
#define ETYPE 0x88B5

struct eth_hdr {
    unsigned char dst[6];
    unsigned char src[6];
    unsigned short type;
} __attribute__((packed));

int corriendo = 1;

void ctrl_c(int s)
{
    (void) s;
    corriendo = 0;
}

void print_mac(const char *tag, unsigned char *m)
{
    printf("%s %02X:%02X:%02X:%02X:%02X:%02X\n", tag,
           m[0], m[1], m[2], m[3], m[4], m[5]);
}

int main(void)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_ll sa;
    unsigned char buf[ETH_FRAME_LEN];
    struct pollfd pfd;

    signal(SIGINT, ctrl_c);

    fd = socket(AF_PACKET, SOCK_RAW, htons(ETYPE));
    if (fd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, IFACE, IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX");
        close(fd);
        exit(1);
    }

    memset(&sa, 0, sizeof(sa));
    sa.sll_family   = AF_PACKET;
    sa.sll_protocol = htons(ETYPE);
    sa.sll_ifindex  = ifr.ifr_ifindex;

    if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("bind");
        close(fd);
        exit(1);
    }

    printf("escuchando en %s, ethertype 0x%04X\n", IFACE, ETYPE);
    printf("ctrl+c para salir\n\n");

    pfd.fd = fd;
    pfd.events = POLLIN;

    while (corriendo) {
        int r = poll(&pfd, 1, 1000);
        if (r < 0) {
            perror("poll");
            break;
        }
        if (r == 0)
            continue;   // nada, seguimos

        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if (n < 0) {
            perror("recv");
            continue;
        }

        if ((size_t) n < sizeof(struct eth_hdr)) {
            fprintf(stderr, "trama corta: %zd bytes\n", n);
            continue;
        }

        struct eth_hdr *h = (struct eth_hdr *) buf;
        unsigned char *payload = buf + sizeof(struct eth_hdr);
        size_t plen = (size_t) n - sizeof(struct eth_hdr);

        printf("\n--- trama recibida ---\n");
        print_mac("dst:", h->dst);
        print_mac("src:", h->src);
        printf("tipo: 0x%04X\n", ntohs(h->type));
        printf("payload: ");

        for (size_t i = 0; i < plen; i++) {
            unsigned char c = payload[i];
            putchar((c >= 32 && c < 127) ? c : '.');
        }
        printf("\n----------------------\n");
    }

    printf("\ncerrando...\n");
    close(fd);
    return 0;
}