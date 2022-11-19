/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.c                                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: novan-ve <novan-ve@student.codam.n>          +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/02/23 14:17:25 by novan-ve      #+#    #+#                 */
/*   Updated: 2022/04/04 09:53:47 by novan-ve      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

#include <signal.h>
#include <unistd.h>

void signal_handler(int sig) {
    if (sig == SIGINT) {
	    g_interrupt[0] = 1;
    }
    else if (sig == SIGALRM) {
        g_interrupt[1] = 1;
    }
}

int	main(int ac, char **av)
{
	char ip[64];
    int verbose = 0;
    int host = 0;

    if (ac == 1) {
        dprintf(2, "ping: usage error: Destination address required\n");
        exit(1);
    }
    for (int i = 1; i < ac; i++) {
        if (av[i][0] != '-') {
            if (host) {
                host = 0;
                break;
            }
            host = i;
        }
        if (!ft_strcmp(av[i], "-h") || !ft_strcmp(av[i], "-help")) {
            host = 0;
            break;
        }
        if (!ft_strcmp(av[i], "-v")) {
            verbose = 1;
        }
    }
    if (!host) {
        dprintf(2, "Usage: ping [-v verbose] [-h help] host\n");
        exit(2);
    }
    if (getuid()) {
        dprintf(2, "ping: usage error: command requires root privileges\n");
        exit(2);
    }
    if (is_number(av[host]) && ft_atoi(av[host]) > 0 && ft_atoi(av[host]) < 16777216) {
        dprintf(2, "ping: connect: Invalid argument\n");
        exit(2);
    }
	if (resolve_dns(ip, av[host], 64)) {
        dprintf(2, "ping: %s: Name or service not known\n", av[host]);
        exit(2);
    }
    if (signal(SIGINT, signal_handler) == SIG_ERR || 
        signal(SIGALRM, signal_handler) == SIG_ERR) {
        dprintf(2, "ping: unexpected error: Failed to initialize signal handler\n");
        exit(1);
    }
    exit(ping_loop(ip, av[host], verbose));
}
