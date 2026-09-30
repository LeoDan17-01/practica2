#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <arpa/inet.h>

#define IFACE   "eth0"
#define ETYPE   0x88B5
#define MAX_MSG 1400

struct eth_hdr {
    unsigned char dst[6];
    unsigned char src[6];
    unsigned short type;
} __attribute__((packed));

static int enviar(int fd, int ifindex,
                  const unsigned char *src,
                  const char *mensaje, size_t mlen)
{
    unsigned char buf[ETH_FRAME_LEN];
    struct eth_hdr *h = (struct eth_hdr *) buf;
    struct sockaddr_ll sa;

    memset(h->dst, 0xff, 6);            /* broadcast */
    memcpy(h->src, src, 6);
    h->type = htons(ETYPE);

    memcpy(buf + sizeof(struct eth_hdr), mensaje, mlen);
    size_t total = sizeof(struct eth_hdr) + mlen;

    memset(&sa, 0, sizeof(sa));
    sa.sll_family  = AF_PACKET;
    sa.sll_ifindex = ifindex;
    sa.sll_halen   = ETH_ALEN;
    memcpy(sa.sll_addr, h->dst, ETH_ALEN);

    if (sendto(fd, buf, total, 0,
               (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("sendto");
        return -1;
    }
    return 0;
}

int main(void)
{
    int fd;
    struct ifreq ifr;
    unsigned char mi_mac[6];
    char linea[MAX_MSG + 2];

    fd = socket(AF_PACKET, SOCK_RAW, htons(ETYPE));
    if (fd < 0) { perror("socket"); return 1; }

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, IFACE, IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX"); close(fd); return 1;
    }
    int ifindex = ifr.ifr_ifindex;

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
        perror("SIOCGIFHWADDR"); close(fd); return 1;
    }
    memcpy(mi_mac, ifr.ifr_hwaddr.sa_data, 6);

    printf("[emisor] listo en %s (ethertype 0x%04X)\n", IFACE, ETYPE);
    printf("[emisor] escribe un mensaje y Enter. 'salir' o Ctrl+D para terminar.\n\n");
    fflush(stdout);

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            printf("\n[emisor] EOF, saliendo\n");
            break;
        }

        size_t len = strlen(linea);
        if (len > 0 && linea[len - 1] == '\n')
            linea[--len] = '\0';

        if (len == 0)
            continue;

        if (strcmp(linea, "salir") == 0)
            break;

        if (len > MAX_MSG) {
            fprintf(stderr, "[emisor] mensaje demasiado largo, ignorado\n");
            continue;
        }

        if (enviar(fd, ifindex, mi_mac, linea, len) == 0)
            printf("[emisor] enviado (%zu bytes)\n", len);
        fflush(stdout);
    }

    close(fd);
    return 0;
}