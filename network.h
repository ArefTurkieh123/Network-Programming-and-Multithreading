#ifndef NETWORK_H
#define NETWORK_H

int start_server(int port);
int connect_to_server(const char *ip, int port);

int send_move(int sock, const int move[4]);
int receive_move(int sock, int move[4]);

#endif