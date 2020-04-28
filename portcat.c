#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

//colors
char wh_blue[30] = "\033[1;36m";
char purpure[30] = "\033[1;35m";
char blue[30]    = "\033[1;34m";
char green[30]   = "\033[1;32m";
char red[30]     = "\033[1;31m";
char st_end[30]  = "\033[0m";

int all_scan_ports[60000];
int port_error[60000];
int last_port;
char ip[100];

void logo();
void HelpMenu();
void write_statuses();
void *scan_port(void *port_ptr);
void create_tread();

int main(int argc, const char* argv[])
{
    int start = time(0);
    if (argc < 3)
    {
    	HelpMenu();
    	exit(1);
    }
    else
    {
        if (atoi(argv[2]) > 60000)
        	last_port = 60000;
        else
        	last_port = atoi(argv[2]);
        strcpy(ip, argv[1]);
    }
    
    logo();
    printf("server: %s\n", ip);
    pthread_t thread1[last_port];
    int port_num[last_port];
    printf("scanning ports 0 to %i, please wait\n\n", last_port);

    for (int i = 0; i < last_port; i++)
    {
        port_num[i] = i+1;
        pthread_create(&thread1[i], NULL, scan_port, &port_num[i]);
    }
    
    for (int j = 0; j < last_port; j++)
        pthread_join(thread1[j], NULL);
    
    write_statuses();
    int end = time(0);
    printf("%s%s%d%s\n", green, "\nscan finished of ", (end - start), " seconds\n");
    return 0;
}

void *scan_port(void *the_port)
{
	struct addrinfo hints, *new_address;
	
	int new_port = *((int *)the_port);
	char *port = malloc(10);
	sprintf(port, "%i", new_port);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, port, &hints, &new_address);
	
	int sock_fd = socket(new_address->ai_family, new_address->ai_socktype, new_address->ai_protocol);

	if (sock_fd < 0 )
	{
		perror("socket not created");
		exit(1);
	}

	int port_status = connect(sock_fd, new_address->ai_addr, new_address->ai_addrlen);
	if (port_status < 0)
		port_error[new_port - 1] = errno;
	close(sock_fd);
	all_scan_ports[new_port - 1] = port_status;
	return 0;
}

void write_statuses()
{
	printf("%s%s%s\n", red, "port     status", st_end);
	for (int i = 0; i < last_port; i++)
	{
		if (all_scan_ports[i] < 0)
		{
			if (port_error[i] == 60)
				printf("%s%-10i %s%s\n", st_end, i+1, wh_blue, "   filtered");
		}
		else
			printf("%s%-10i%sopen\n", st_end, i+1, green);
		}
		printf("%s", "\n");
}

void logo()
{
	system("clear");
	printf("%s", red);
	printf("%s\n", "     ######      ####      #####     ######## ");
	usleep(100000);
	printf("%s\n", "     ##   ##    ##  ##     ##  ##       ##    ");
	usleep(100000);
	printf("%s\n", "     ######     ##  ##     ####         ##    ");
	usleep(100000);
	printf("%s\n", "     ##         ##  ##     ## ##        ##    ");
	usleep(100000);
	printf("%s",   "     ##          ####      ##  ##       ##   ");
	usleep(100000);
	printf("%s%s%s%s", wh_blue, "၄ÁҬ ", blue, "v1.0\n");
	usleep(100000);
	printf("%s%s%s%s", green, "\n\t\t\t\t  coded by >> ", purpure, "𝖓𝖎𝖐𝖆𝖎𝖙\n");
	usleep(100000);
	printf("%s%s%s%s\n", green, "\t\t\t\t  telegtam >> ", purpure, "@aaanikit\n");
}

void HelpMenu()
{
	printf("%s\n", "Hello! PortCat - is simple port scaner\n\nusage:\n./portcat ip_or_domain max_port_to_scan");
}
