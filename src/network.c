/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   network.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: novan-ve <novan-ve@student.codam.n>          +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/16 14:45:01 by novan-ve      #+#    #+#                 */
/*   Updated: 2022/04/04 09:32:37 by novan-ve      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

#include <netdb.h>
#include <netinet/ip_icmp.h>

uint16_t ft_htons(uint16_t hostshort) {
    int n = 1;

    if (*(char *)&n) {
        return (((hostshort & 0xFF) << 8) | ((hostshort & 0xFF00) >> 8));
    }

    return hostshort;
}

int	resolve_dns(char *dst, char *src, size_t dst_size) {
    struct addrinfo		hints;
    struct addrinfo		*servinfo;
    struct addrinfo		*result;
    struct sockaddr_in	*sockinfo;

    if (!ft_strcmp(src, "0.0.0.0")) {
        ft_memcpy(dst, "127.0.0.1", 10);
        return (0);
    }

    ft_memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(src, NULL, &hints, &servinfo)) {
        return (1);
    }

    result = servinfo;

    while (result) {
        sockinfo = (struct sockaddr_in *)result->ai_addr;

        if (inet_ntop(AF_INET, &sockinfo->sin_addr, dst, dst_size)) {
            freeaddrinfo(servinfo);
            return (0);
        }

        result = result->ai_next;
    }

    freeaddrinfo(servinfo);
    return (1);
}

unsigned short calc_checksum(void *b, size_t len) {
    long sum;
    unsigned short result;
    unsigned short *addr = (unsigned short*)b;

    sum = 0;

    while (len > 1) {
        sum += *addr++;
        len -= 2;
    }

    if (len > 0) {
        sum += *(unsigned char*)addr;
    }

    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    result = ~sum;

    return result;
}

int comp_checksum(void *b, size_t len) {
    struct icmp *packet = (struct icmp*)b;
    unsigned short cksum = packet->icmp_cksum;
    unsigned short result;

    packet->icmp_cksum = 0;
    result = calc_checksum(b, len);
    packet->icmp_cksum = cksum;

    return (result == cksum);
}
