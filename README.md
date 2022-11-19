# Ping
Reimplementation of the ping command in C

```bash
$ make && ./ft_ping 8.8.8.8
PING 8.8.8.8 (8.8.8.8) 56(84) bytes of data.
64 bytes from 8.8.8.8: icmp_seq=1, ttl=120 time=6.43 ms
64 bytes from 8.8.8.8: icmp_seq=2, ttl=120 time=6.46 ms
64 bytes from 8.8.8.8: icmp_seq=3, ttl=120 time=6.29 ms
^C
--- 8.8.8.8 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2013ms
rtt min/avg/max/mdev = 6.293/6.396/6.462/0.074 ms
```
