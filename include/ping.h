/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ping.h                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: novan-ve <novan-ve@student.codam.n>          +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/14 13:30:13 by novan-ve      #+#    #+#                 */
/*   Updated: 2022/04/04 09:51:10 by novan-ve      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

# include <stdio.h>
# include <stdlib.h>
# include <arpa/inet.h>
# include <linux/in.h>

extern int g_interrupt[2];

typedef struct      s_statistics
{
    unsigned short  seq;
    int             packets_send;
    int             packets_received;
    double          send_time;
    double          total_time;
    double          min_time;
    double          max_time;
    double          time_sum;
    double          time_sum_square;
}                   t_statistics;

int             ping_loop(char *ip, char *host, int verbose);

uint16_t        ft_htons(uint16_t hostshort);
int             resolve_dns(char *dst, char *src, size_t dst_size);
unsigned short  calc_checksum(void *b, size_t len);
int             comp_checksum(void *b, size_t len);

void            *ft_memset(void *b, int c, size_t len);
void            *ft_memcpy(void *dst, const void *src, size_t n);
int             ft_strcmp(const char *s1, const char *s2);
int             ft_atoi(const char *str);
double          ft_sqrt(double t);
int             is_number(const char *s);
double          time_in_ms();

#endif