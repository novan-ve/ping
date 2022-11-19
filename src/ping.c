/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ping.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: novan-ve <novan-ve@student.codam.n>          +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/14 13:36:32 by novan-ve      #+#    #+#                 */
/*   Updated: 2022/04/04 09:31:47 by novan-ve      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>

int g_interrupt[2] = {0, 0};

int create_socket(struct sockaddr_in *addr, char *ip) {
    struct timeval  tv;
    int             sockfd;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1) {
        dprintf(2, "ping: failed to create socket\n");
	    exit(1);
    }

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv,
                   sizeof(struct timeval)) < 0) {
        dprintf(2, "ping: failed to set socket timeout\n");
        exit(1);
    }

    ft_memset(addr, 0, sizeof(*addr));

    addr->sin_family = AF_INET;

    if (!inet_pton(AF_INET, ip, &(addr->sin_addr))) {
        dprintf(2, "ping: invalid address\n");
        exit(1);
    }

    return (sockfd);
}

int send_packet(int sockfd, struct sockaddr_in *addr, unsigned short seq) {
    char packet_str[64];
    struct icmp *packet;

    ft_memset(packet_str, 0, 64);

    packet = (struct icmp *)packet_str;
    packet->icmp_type = ICMP_ECHO;
    packet->icmp_code = 0;
    packet->icmp_cksum = 0;
    packet->icmp_seq = ft_htons(seq);
    packet->icmp_id = ft_htons(getpid());
    packet->icmp_cksum = calc_checksum(packet, 64);

    if (sendto(sockfd, packet_str, 64, 0, (struct sockaddr*)addr, sizeof(*addr)) < 0) {
        dprintf(2, "ping: failed to send packet\n");
        return (0);
    }

    return (1);
}

int receive_packet(int sockfd, char *dst, size_t len) {
    char            buf[128];
    struct msghdr   msg;
    struct iovec    iov;

    ft_memset(&msg, 0, sizeof(msg));

    iov.iov_base = dst;
    iov.iov_len = len;
    msg.msg_name = buf;
    msg.msg_namelen = sizeof(buf);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    return (recvmsg(sockfd, &msg, MSG_WAITALL));
}

int unpack_packet(char *packet_str, int size, t_statistics *stats, char *host, char *ip, int verbose) {
    char            sender[128];
    int             iphdr_len;
    double          time_diff;
    struct ip       *ip_packet;
    struct icmp     *icmp_packet;

    ip_packet = (struct ip*)packet_str;
    iphdr_len = ip_packet->ip_hl * 4;
    icmp_packet = (struct icmp*)(packet_str + iphdr_len);

    if (!inet_ntop(AF_INET, &ip_packet->ip_src, sender, sizeof(sender))) {
        if (verbose) {
            dprintf(2, "ping: invalid packet received\n");
        }
        return (1);
    }

    if (icmp_packet->icmp_id != ft_htons(getpid())) {
        return (0);
    }
    if (!comp_checksum(icmp_packet, 64)) {
        if (verbose) {
            dprintf(2, "ping: Packet with incorrect checksum received\n");
        }
        return (1);
    }
    else if (!ip_packet->ip_ttl) {
        if (verbose) {
            dprintf(2, "ping: TTL Expired in Transit\n");
        }
        return (1);
    }
    else if (icmp_packet->icmp_type == ICMP_ECHOREPLY) {
        time_diff = time_in_ms() - stats->send_time;

        if (!ft_strcmp(host, "127.0.0.1") || !ft_strcmp(host, "0.0.0.0")) {
            printf("%d bytes from 127.0.0.1: ", size - iphdr_len);
        }
        else if (!ft_strcmp(host, ip)) {
            printf("%d bytes from %s: ", size - iphdr_len, host);
        }
        else {
            printf("%d bytes from %s (%s): ", size - iphdr_len, host, sender);
        }
        printf("icmp_seq=%u, ", ft_htons(icmp_packet->icmp_seq));
        printf("ttl=%u ", ip_packet->ip_ttl);
        
        if (time_diff < 1.0) {
            printf("time=%.3f ms\n", time_diff);
        }
        else if (time_diff < 10.0) {
            printf("time=%.2f ms\n", time_diff);
        }
        else if (time_diff < 100.0) {
            printf("time=%.1f ms\n", time_diff);
        }
        else {
            printf("time=%.f ms\n", time_diff);
        }

        if (time_diff > stats->max_time) {
            stats->max_time = time_diff;
        }
        if (time_diff < stats->min_time || !stats->min_time) {
            stats->min_time = time_diff;
        }

        stats->time_sum += time_diff;
        stats->time_sum_square += time_diff * time_diff;
    }
    else if (icmp_packet->icmp_type == ICMP_ECHO && !ft_strcmp(sender, "127.0.0.1")) {
        return (0);
    }
    else if (verbose) {
        printf("%d bytes from %s: ", size - iphdr_len, sender);
        printf("type = %d, code = %d\n", icmp_packet->icmp_type, icmp_packet->icmp_code);
    }

    stats->packets_received++;
    return (1);
}

void print_stats(t_statistics *stats, char *host) {
    double          mean;
    double          smean;

    printf("\n--- %s ping statistics ---\n", host);
    printf("%d packets transmitted, ", stats->packets_send);
    printf("%d received, ", stats->packets_received);
    if (stats->packets_send) {
        printf("%.f%% packet loss, ", 
            (1.0 - (stats->packets_received / (double)stats->packets_send)) * 100.0);
    }
    printf("time %.fms\n", stats->total_time);

    if (stats->packets_received) {
        printf("rtt min/avg/max/mdev = %.3f/", stats->min_time);
        printf("%.3f/", stats->time_sum / (double)stats->packets_received);
        printf("%.3f/", stats->max_time);

        if (stats->packets_received == 1) {
            printf("0.000 ms\n");
        }
        else {
            mean = stats->time_sum / (double)stats->packets_received;
            smean = stats->time_sum_square / (double)stats->packets_received;
            printf("%.3f ms\n", ft_sqrt(smean - mean * mean));
        }
    }
}

int	ping_loop(char *ip, char *host, int verbose) {
    int                 sockfd;
    struct sockaddr_in  addr;
    char                recv_buf[84];
    int                 bytes_received = 0;
    t_statistics        stats = {1, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    sockfd = create_socket(&addr, ip);

    printf("PING %s (%s) 56(84) bytes of data.\n", host, ip);

    while(!g_interrupt[0]) {
        stats.send_time = time_in_ms();
        if (send_packet(sockfd, &addr, stats.seq))
        {
            stats.packets_send++;
            while (!g_interrupt[0]) {
                bytes_received = receive_packet(sockfd, recv_buf, sizeof(recv_buf));
                if (bytes_received == -1) {
                    break;
                }
                if (unpack_packet(recv_buf, bytes_received, &stats, host, ip, verbose)) {
                    alarm(1);
                    while (!g_interrupt[1] && !g_interrupt[0]) {}
                    g_interrupt[1] = 0;
                    break;
                }
            }
            if (!g_interrupt[0]) {
                stats.total_time += time_in_ms() - stats.send_time;
            }
        }
        else {
            alarm(1);
            while (!g_interrupt[1] && !g_interrupt[0]) {}
            g_interrupt[1] = 0;
        }
        stats.seq++;
    }
    print_stats(&stats, host);

    return (0);
}
