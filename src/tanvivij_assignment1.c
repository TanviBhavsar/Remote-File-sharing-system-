/**
 * @tanvivij_assignment1
 * @author  Tanvi Vijay Bhavsar <tanvivij@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#define STDIN 0
#define PACKET_SIZE 1000
#include "../include/global.h"

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */

struct stats
{
	int upload_ctr,download_ctr;
	long double upload_speed, download_speed;
	char hostname[150];
};

struct stats_server
{
	int upload_ctr,download_ctr;
	char upload_speed[200], download_speed[200];
	char hostname1[150],hostname2[150];
};
struct stats list_stats[20];
struct stats_server list_stats_server[20];
int list_stats_sctr=0;


void calc_stats_server(char buf[2000])
{
	int i;
	char s[2] = "#";
	char *token;
	int argc1=0,index=-1;
	token=(char *)malloc(2000*sizeof(char));

	token = strtok(buf, s);
	for(i=0;i<list_stats_sctr;i++)
	{
		if(strcmp(token,list_stats_server[i].hostname1)==0)
			index=i;
	}
	if(index==-1)
	{
		index=list_stats_sctr;
		list_stats_sctr++;
	}



	while( token != NULL )
	{

		fflush(stdout);
		switch(argc1)
		{
		case 0: strcpy(list_stats_server[index].hostname1,token);
		break;
		case 1: strcpy(list_stats_server[index].hostname2,token);
		break;
		case 2:list_stats_server[index].upload_ctr=atoi(token);
		break ;
		case 3 :strcpy(list_stats_server[index].upload_speed,token);
		break;
		case 4:list_stats_server[index].download_ctr=atoi(token);
		break ;
		case 5 :strcpy(list_stats_server[index].download_speed   ,token);
		break;


		}
		argc1++;
		token = strtok(NULL, s);

	}

}
void stats_server_print()
{
	int i;
	printf("\nHostname 1		  Hostname 2 		    Total Uploads  Average upload Speed  Total Downloads   Average Download Speed");
	for(i=0;i<list_stats_sctr;i++)
		printf("\n%s  %s %d              %s              %d               %s", list_stats_server[i].hostname1, list_stats_server[i].hostname2,list_stats_server[i].upload_ctr,list_stats_server[i].upload_speed,list_stats_server[i].download_ctr,list_stats_server[i].download_speed );

}



void statistics()
{
	int i;
	printf("\nHostname 		 Total Uploads  Average upload Speed   Total Downloads   Average Download Speed");
	for(i=0;i<20;i++)
	{
		if((list_stats[i].upload_ctr>0) || (list_stats[i].download_ctr >0) )


			printf("\n%s %d           %llf                %d              %llf \n",list_stats[i].hostname,list_stats[i].upload_ctr,list_stats[i].upload_speed,list_stats[i].download_ctr,list_stats[i].download_speed);
	}
}

struct list_connection
{
	char id[2];
	char hostname[150];
	char ip_address[150];
	char port_no[50];
	int socket_descriptor;
	int connected;//tells client whether it is connected to a peer
};
char *my_ip,my_hostname[100];
void server_help()
{
	printf("\nAvailable User Interfaces for Server -  \n");
	printf("1.creator- Displays information about the author of the program and department integrity policy\n");
	printf("2.myip- Displays the IP address of the machine on which Program is running\n");
	printf("3.myport- Displays the listening port no\n");
	printf("4.statistics-Displays statistics of upload/download operations in the past for clients\n");
	printf("5.exit- Program ends\n");

}

void client_help()
{
	printf("\nAvailable User Interfaces for Client -  \n");
	printf("1.creator- Displays information about the author of the program and department integrity policy\n");
	printf("2.myip- Displays the IP address of the machine on which Program is running\n");
	printf("3.myport- Displays the listening port no\n");
	printf("4.register <server IP> <port>- registers this client with <server IP> which is listening on <port>.\n");
	printf("5.connect <destination> <port>- connects this client to a <destination> which is listening on <port>.\n");
	printf("6.list -lists all connection this process is part of\n");
	printf("7.terminate <ID> - terminates the connection listed under the specified <ID> when LIST is used to display all connections\n");
	printf("8.upload <connection ID> <file name> - uploads the file <file name> to the peer which has ID equal to <connection ID> in its List\n");
	printf("9.download <connection ID1> <file name1> <connection ID2> <file name2>  - Downloads the file <file name1> from peer which has ID equal to <connection ID1> in its List\n");
	printf("10.statistics-Displays statistics of upload/download operations in the past\n");
	printf("11.exit - closes all connection and terminates this process\n");


}


void download_file(char tokens_array[100][100],int argc,struct list_connection client_list[5],int client_list_ctr)
{

	int i,sock_des_send,j,found_id=0;
	char download_string[100];
	for(i=1;i<argc;)
	{
		int sprintf_ret=sprintf (download_string, "DOWNLOAD#%s",tokens_array[i+1]);
		found_id=0;

		//FIND SOCKEt descriptor of destination id
		for(j=1;j<client_list_ctr;j++)
		{
			if((strcmp(client_list[j].id,tokens_array[i])==0))
			{
				sock_des_send=client_list[j].socket_descriptor;
				found_id=1;
				break;
			}

		}
		if(found_id==0)
			printf("Could not find id %s",tokens_array[i]);
		else
		{
			int sent_size=send(sock_des_send, download_string,sprintf_ret, 0);
			if ( sent_size == -1)
				perror("send");
		}
		i=i+2;
	}
}

int  upload_file(char destination_id[10],char file_name[100],struct list_connection client_list[5],int client_list_ctr)
{

	FILE *fp;
	printf("\nStarted upload");

	struct timeval start_time, end_time;
	gettimeofday(&start_time, NULL);


	char buffer[PACKET_SIZE],packet_send[PACKET_SIZE],first_packet[PACKET_SIZE],send_file_name[100],buf[100],destination_hostname[100],stats_string[1000];
	int file_size, sock_des_send,i,no_read;



	memset(packet_send,'\0',PACKET_SIZE);
	memset(first_packet,'\0',PACKET_SIZE);

	// extract just filename from path
	strcpy(buf,file_name);
	strcpy(send_file_name, "NULL");
	char s[2] = "/";
	char *token;
	int argc1=0;
	token=(char *)malloc(100*sizeof(char));

	token = strtok(buf, s);


	while( token != NULL )
	{

		fflush(stdout);
		strcpy(send_file_name,token);
		token = strtok(NULL, s);

	}

	int ctr=0,to_read=0;
	fp = fopen(file_name, "rb");

	if(!fp)
	{
		printf("\n fopenn error");
		return;
	}
	int found_id=0;
	//FIND SOCKEt descriptor of destination id
	for(i=1;i<client_list_ctr;i++)
	{
		if((strcmp(client_list[i].id,destination_id)==0))
		{
			sock_des_send=client_list[i].socket_descriptor;
			strcpy(destination_hostname,client_list[i].hostname);
			found_id=1;
			break;
		}

	}
	if(found_id==0)
		return -1;


	fseek(fp, 0, SEEK_END); // seek to end of file
	file_size = ftell(fp); // get current file pointer
	fseek(fp, 0, SEEK_SET);
	int sprintf_ret;
	if ((strcmp(send_file_name,"NULL"))!=0)//only file name is given as input
		sprintf_ret=sprintf (packet_send, "filename-%s;filesize-%d",send_file_name,file_size );
	else
		sprintf_ret=sprintf (packet_send, "filename-%s;filesize-%d",file_name,file_size );

	to_read=PACKET_SIZE-sprintf_ret-1;
	no_read=fread(buffer,1, to_read, fp);
	sprintf (first_packet, "%s;%s",packet_send,buffer );
	int first_send=no_read+sprintf_ret+1;
	int sent_size=send(sock_des_send, first_packet,first_send, 0);

	if ( sent_size== -1)
		perror("send");

	while(no_read)
	{

		memset(packet_send,'\0',PACKET_SIZE);
		/* Read and display data */
		no_read=fread(packet_send,1, PACKET_SIZE, fp);
		if(no_read==0)
			break;

		sent_size=send(sock_des_send, packet_send,no_read, 0);

		if ( sent_size == -1)
			perror("send");


	}
	fclose(fp);
	gettimeofday(&end_time, NULL);

	long diff=((end_time.tv_sec - start_time.tv_sec)*1000000) +(end_time.tv_usec - start_time.tv_usec);
	long double time_taken=(long double)diff/1000000;


	long double rate=(long double)(file_size*8)/time_taken;


	printf("\nUpload Complete");
	printf("\n Tx: %s -> %s, File Size :%d bytes, Time Taken: %llf seconds, Tx Rate:%llf bits/second\n", my_hostname,client_list[i].hostname,file_size,time_taken,rate);
	int id=atoi(destination_id);
	list_stats[id].upload_ctr+=1;
	if(list_stats[id].upload_speed>0)
		list_stats[id].upload_speed=(list_stats[id].upload_speed+rate)/2;
	else
		list_stats[id].upload_speed=rate;
	strcpy(list_stats[id].hostname,destination_hostname);
	
	sprintf_ret=sprintf (stats_string, "%s#%s#%d#%llf#%d#%llf",my_hostname,list_stats[id].hostname,list_stats[id].upload_ctr,list_stats[id].upload_speed,list_stats[id].download_ctr,list_stats[id].download_speed);

	if ( send(client_list[0].socket_descriptor, stats_string,sprintf_ret, 0) == -1)
		perror("send");
	return 1;

}

void  parse_download_string(char peer_send[PACKET_SIZE],char source_id[10],struct list_connection client_list[5],int client_list_ctr)
{
	char s[2] = " #";
	char *token,buff[PACKET_SIZE],file_name[100];
	token=(char *)malloc(strlen(peer_send)*sizeof(char));
	strcpy(buff,peer_send);
	token = strtok(buff, s);
	int argc1=0;

	while( token != NULL )
	{

		argc1+=1;
		if (argc1==2)
			strcpy(file_name,token );
		token = strtok(NULL, s);

	}
	int ret =upload_file(source_id,file_name,client_list,client_list_ctr);
	if(ret!=1)
		printf("\nUpload file failed");
}

int receive_file(char peer_send[PACKET_SIZE],char file_name[100], int *file_size, int *bytes_written,int noofbytes)
{

	char packet_write[PACKET_SIZE];
	FILE *fp=NULL;


	memset(packet_write,'\0',PACKET_SIZE);


	if((strstr(peer_send,"filename-")!=NULL))
	{
		//extract filename n filesize

		//new file so initialize
		*file_size=0;
		*bytes_written=0;
		char s[2] = ";";
		char s2[2]="-",token2[PACKET_SIZE][PACKET_SIZE];
		char *token,*token3;
		int ctr=0,token2_ctr=0;
		token=(char *)malloc(noofbytes*sizeof(char));
		token3= (char *)malloc(noofbytes*sizeof(char));
		token = strtok(peer_send, s);

		while( token != NULL )
		{
			//printf( "token is %s\n", token );
			fflush(stdout);
			strcpy(token2[token2_ctr], token);
			token2_ctr++;

			//last string will be blank
			if((strcmp(token, " ")==0))
				break;
			token = strtok(NULL, s);
		}
		int x=noofbytes-strlen(token2[0])-strlen(token2[1])-2;//no of bytes to write(2 delimitators)

		token3 = strtok(token2[0], s2);
		token3=strtok(NULL, s2);
		if(token3)
		{
			strcpy(file_name,token3);
			token3=strtok(token2[1],s2);
			token3=strtok(NULL, s2);;
			*file_size=atoi(token3);
		}
		strcpy(packet_write, token2[2]);

		fp= fopen(file_name,"wb");
		if(fp)
			fwrite(packet_write , 1 , x, fp );
		else
		{
			printf("\nCould not open file");
			return -1;
		}
		*bytes_written+=x;

	}
	else
	{
		//write in file

		fp= fopen(file_name,"ab");//open in append mode as we want chnges to stay
		if(!fp)
		{
			printf("\nCould not open file");
			return -1;
		}

		if(*bytes_written<*file_size)
		{
			*bytes_written+=noofbytes;
			if(*bytes_written>*file_size)
			{
				*bytes_written-=noofbytes;
				int write_last=*file_size-*bytes_written;
				peer_send[write_last]='\0';// remove extra characters if any
				fwrite(peer_send , 1 , write_last , fp );
				*bytes_written+=write_last;
			}
			else
				fwrite(peer_send , 1 , noofbytes , fp );

		}



	}


	fclose(fp);
	return 1;
}
int delete_array(struct list_connection delete_list[5], int list_ctr, int delete_index)
{
	int c;

	for ( c = delete_index ; c < list_ctr - 1 ; c++ )
	{


		strcpy(delete_list[c].id,delete_list[c+1].id);
		strcpy(delete_list[c].hostname,delete_list[c+1].hostname);
		strcpy(delete_list[c].port_no,delete_list[c+1].port_no);
		strcpy(delete_list[c].ip_address ,delete_list[c+1].ip_address);
		delete_list[c].socket_descriptor=delete_list[c+1].socket_descriptor;
		delete_list[c].connected=delete_list[c+1].connected;




	}
	list_ctr--;
	return list_ctr;

}

int client_terminate(char id_terminate[10],struct list_connection client_list[5],int client_list_ctr)
{
	int found_id=-1,i;
	for(i=1;i<client_list_ctr;i++)
	{
		if((strcmp(client_list[i].id,id_terminate)==0))
		{


			client_list[i].connected=0;
			found_id=1;
			break;
		}
	}
	if (found_id==-1)
		printf("\n ID does not exist");
	return found_id;
}

//when client prints server list it should not print 0th entry which is dokken
void list_print(struct list_connection given_list[5],int given_list_ctr)
{
	int i;
	if (given_list_ctr == 0)
	{
		printf ("List is empty");
		return;
	}
	for(i=0;i<given_list_ctr;i++)
	{


		printf("%-5d%-35s%-20s%-8d\n",atoi(given_list[i].id),given_list[i].hostname,given_list[i].ip_address,atoi(given_list[i].port_no));



	}
	fflush(stdout);

}

//This function is coming from http://jhshi.me/2013/11/02/how-to-get-hosts-ip-address/
char* find_my_ip()
																																																																																{
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) < 0) {
		perror("gethostname");
		return "NULL";
	}

	// Google's DNS server IP
	char* target_name = "8.8.8.8";
	// DNS port
	char* target_port = "53";

	/* get peer server */
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* info;
	int ret = 0;
	if ((ret = getaddrinfo(target_name, target_port, &hints, &info)) != 0) {
		printf("[ERROR]: getaddrinfo error: %s\n", gai_strerror(ret));
		return "NULL";
	}

	if (info->ai_family == AF_INET6) {
		printf("[ERROR]: do not support IPv6 yet.\n");
		return "NULL";
	}

	/* create socket */
	int sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (sock <= 0) {
		perror("socket");
		return "NULL";
	}

	/* connect to server */
	if (connect(sock, info->ai_addr, info->ai_addrlen) < 0) {
		perror("connect");
		close(sock);
		return "NULL";
	}

	/* get local socket info */
	struct sockaddr_in local_addr;
	socklen_t addr_len = sizeof(local_addr);
	if (getsockname(sock, (struct sockaddr*)&local_addr, &addr_len) < 0) {
		perror("getsockname");
		close(sock);
		return "NULL";
	}

	/* get peer ip addr */
	char *myip, myip1[INET_ADDRSTRLEN];
	myip=(char *)malloc(INET_ADDRSTRLEN*sizeof(char));
	if (inet_ntop(local_addr.sin_family, &(local_addr.sin_addr), myip1, sizeof(myip1)) == NULL) {
		perror("inet_ntop");
		return "NULL";
	}
	strcpy(myip,myip1);
	//printf("My IP is %s %s \n", myip1, myip1);
	fflush(stdout);

	return myip;

																																																																																}

void initialize_list( struct list_connection list_init[5])
{
	//Initially no client is connected and no socket
	int i;
	for(i=0;i<5;i++)
	{
		list_init[i].connected=0;
		list_init[i].socket_descriptor=-1;

	}

}
int server_register(char buf[40], struct list_connection server_list[5], int server_list_ctr)
{
	if((strstr(buf,"REGISTER")!=NULL)|| (strstr(buf,"register")!=NULL))
	{
		//extract port mo of string
		char s[2] = "#";
		char *token;
		int argc1=0,new_fd;
		token=(char *)malloc(100*sizeof(char));
		//for strtok everywhere took help from recitation slides
		token = strtok(buf, s);


		while( token != NULL )
		{

			fflush(stdout);
			if(argc1==1)
				strcpy(server_list[server_list_ctr].port_no,token);
			argc1+=1;
			token = strtok(NULL, s);

		}
		int sprintf_ret=sprintf (server_list[server_list_ctr].id, "%d",(server_list_ctr+1) );


		//Took from Beej

		socklen_t len;
		struct sockaddr_storage addr;

		int port;

		len = sizeof addr;
		getpeername(server_list[server_list_ctr].socket_descriptor, (struct sockaddr*)&addr, &len);

		if (addr.ss_family == AF_INET) {
			struct sockaddr_in *s = (struct sockaddr_in *)&addr;
			port = ntohs(s->sin_port);
			inet_ntop(AF_INET, &s->sin_addr, server_list[server_list_ctr].ip_address, sizeof server_list[server_list_ctr].ip_address);
		}


		//Used beej
		struct in_addr ipv4addr;
		struct hostent *he;

		inet_pton(AF_INET, server_list[server_list_ctr].ip_address, &ipv4addr);
		he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
		if(he)
			strcpy(server_list[server_list_ctr].hostname,he->h_name);





		int i,j,n;
		char clients_send[500];
		char previous_string[500];
		strcpy(previous_string, "NULL");
		//do for host name
		server_list_ctr++;

		//Send server list to all clients
		for(i=0;i<server_list_ctr;i++)
		{

			int sprintf_ret=sprintf (clients_send, "%s#%s#%s#%s:",server_list[i].id,server_list[i].hostname,server_list[i].ip_address,server_list[i].port_no);

			if(strcmp(previous_string,"NULL")==0)
				strcpy(previous_string,clients_send);
			else
				strcat(previous_string, clients_send);
		}

		for(j=0;j<server_list_ctr;j++)
		{

			if (send(server_list[j].socket_descriptor,previous_string, strlen(previous_string), 0) == -1)
				perror("send");
		}
	}
	return server_list_ctr;
}
int client_connect(char tokens_array[100][100],struct list_connection client_list[5], int client_list_ctr,struct list_connection server_list_onclient[5],int server_list_ctr)
{
	struct addrinfo hints, *servinfo, *p;
	int rv,sockfd,i,can_connect=0,found_iphost=0, found_port=0,server_index,is_ip=1;

	char cmd[100],server_send[100];
	if (client_list_ctr> 3)
	{
		printf("\nCannot connect to more than 3 clients");
		return -1;
	}

	if((strcmp(tokens_array[1],my_ip)==0))
	{
		printf("\nCannot connect to own IP. Enter Another IP");
		return -1;
	}
	//Check whether given details are present in server ip list
	for(i=0;i<server_list_ctr;i++)
	{


		is_ip=strcmp(tokens_array[1],server_list_onclient[i].ip_address);
		if((strcmp(tokens_array[1],server_list_onclient[i].ip_address)==0)|| (strcmp(tokens_array[1],server_list_onclient[i].hostname)==0))
		{
			found_iphost=1;
			if((strcmp(tokens_array[2],server_list_onclient[i].port_no)==0))
			{
				found_port=1;
				server_index=i;
				break;
			}
		}
		
	}


	if(found_iphost==0)
	{
		printf("\n This client has not registered with Server \n");
		return -1;
	}
	if(found_port==0)
	{
		printf("\nGiven host is not listening at input port\n");
		return -1;
	}
	//Check whether connection exists already
	for(i=1;i<client_list_ctr;i++)
	{
		if((strcmp(tokens_array[1],client_list[i].ip_address)==0)|| (strcmp(tokens_array[1],server_list_onclient[i].hostname)==0))
		{

			can_connect=1;
			break;

		}
	}
	if(can_connect==1)
	{
		printf("\nA connection to destination already exists\n");
		return -1;
	}

	//Obtain IP address
	//Used beej
	if(is_ip!=0)
	{
		struct hostent *he;
		struct in_addr **addr_list;



		if ((he = gethostbyname(tokens_array[1])) == NULL) {  // get the host info
			herror("gethostbyname");
			return 2;
		}



		addr_list = (struct in_addr **)he->h_addr_list;
		strcpy(tokens_array[1],inet_ntoa(*addr_list[0]));
		printf("converted Ip address is %s",tokens_array[1]);
	}
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;



	if ((rv = getaddrinfo(tokens_array[1], tokens_array[2], &hints, &servinfo)) != 0) {//put url of servers instead of 127.0.0.1
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}



	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(1);
	}
	else if (sockfd)
	{
		printf("\nConnect to peer success\n");



		int numbytes,sprintf_ret;
		char buf[20];




		strcpy(client_list[client_list_ctr].hostname,server_list_onclient[server_index].hostname);
		strcpy(client_list[client_list_ctr].ip_address,server_list_onclient[server_index].ip_address);
		strcpy(client_list[client_list_ctr].port_no,server_list_onclient[server_index].port_no);

		sprintf_ret=sprintf (client_list[client_list_ctr].id, "%d",(client_list_ctr+1)) ;

		client_list[client_list_ctr].connected=1;
		client_list[client_list_ctr].socket_descriptor=sockfd;


		return i;

	}

}



void is_server(char listening_port[20])
{
	//create listening socket
	//Used Beej
	struct list_connection server_list[5];
	initialize_list(server_list);
	int server_list_ctr=0;
	int listening_socket, new_fd=-1;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;

	struct sockaddr_in their_addr;
	socklen_t sin_size;

	int yes=1;

	char buf[3000],*myip_server;
	int rv,sock_index,c,i;

	int numbytes;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, listening_port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((listening_socket = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(listening_socket, p->ai_addr, p->ai_addrlen) == -1) {
			close(listening_socket);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(listening_socket, 4) == -1) {
		perror("listen");
		exit(1);
	}

	fd_set masterlist,watchlist;

	// clear the set ahead of time

	FD_ZERO(&watchlist);
	FD_ZERO(&masterlist);

	// add our descriptors to the set

	FD_SET(STDIN, &masterlist);
	FD_SET(listening_socket, &masterlist);


	int head_socket = listening_socket, argc1;
	char cmd[100];

	while(1)
	{


		fflush(stdout);
		printf("\n [PA1]$ ");
		fflush(stdout);
		argc1=0;


		watchlist=masterlist;
		rv = select(head_socket+1, &watchlist, NULL, NULL, NULL);

		if (rv == -1) {
			perror("select"); // error occurred in select()
		}
		else {
			for(sock_index=0; sock_index<=head_socket;sock_index+=1)
			{

				fflush(stdout);
				// one or both of the descriptors have data
				if (FD_ISSET(sock_index, &watchlist)) {
					if(sock_index==STDIN)
					{

						fgets (cmd, 100, stdin);
						fflush(stdout);
						char tokens_array[100][100];
						char s[2] = " ";
						char *token;
						token=(char *)malloc(strlen(cmd)*sizeof(char));

						token = strtok(cmd, s);


						while( token != NULL )
						{
							//printf( "token is %s\n", token );
							fflush(stdout);
							strcpy(tokens_array[argc1],token);
							argc1+=1;
							token = strtok(NULL, s);
						}
						//Last character of last string has \n
						c=strlen(tokens_array[argc1-1])-1;
						tokens_array[argc1-1][c]='\0';


						if((strcasecmp(tokens_array[0],"EXIT"))==0)
							exit(0);

						else if((strcasecmp(tokens_array[0],"help"))==0)
							server_help();
						else if((strcasecmp(tokens_array[0],"statistics"))==0)
							stats_server_print();

						else if((strcasecmp(tokens_array[0],"MYIP"))==0)
						{
							myip_server=find_my_ip();
							if((strcmp(myip_server,"NULL")!=0))
							{
								printf("IP address:%s", myip_server);
								fflush(stdout);
							}
						}
						else if((strcasecmp(tokens_array[0],"MYPORT"))==0)
							printf("Port number:%d",atoi(listening_port));
						else if((strcasecmp(tokens_array[0],"CREATOR"))==0)
						{
							printf("\n Tanvi Vijay Bhavsar tanvivij tanvivij@buffalo.edu\n");
							printf("I have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity");
						}

						else if((strcasecmp(tokens_array[0],"LIST"))==0)
							list_print(server_list,server_list_ctr);
						else
							printf("\nInvalid input\n");


					}

					else
					{
						if (sock_index==listening_socket)
						{

							new_fd = accept(listening_socket, (struct sockaddr *)&their_addr, &sin_size);

							server_list[server_list_ctr].socket_descriptor=new_fd;
							if(new_fd > head_socket)
								head_socket=new_fd;
							FD_SET(new_fd, &masterlist);


						}
						else
						{

							if (new_fd>0)
							{
								numbytes = recv(new_fd, buf, 20, 0);

								if(numbytes==-1)
								{
									perror("recv");
									exit(1);

								}
								else if(numbytes > 0)
								{
									buf[numbytes] = '\0';
									server_list_ctr=server_register( buf, server_list,server_list_ctr);
								}
								new_fd=-1;
							}
							else
							{
								for(i=0;i<server_list_ctr;i++)
								{
									if (server_list[i].socket_descriptor==sock_index)
									{
										numbytes = recv(server_list[i].socket_descriptor, buf, 1000, 0);

										if(numbytes==-1)
										{
											perror("recv");
											exit(1);

										}
										else if(numbytes==0)
										{
											printf("Server received 0 bytes from another client so disconnecting\n");
											close(server_list[i].socket_descriptor);
											FD_CLR(server_list[i].socket_descriptor, &masterlist);

											server_list[i].socket_descriptor=-1;
											server_list[i].connected=0;


											printf("\nAfter terminate Server list is\n");
											//delete entry from array
											server_list_ctr=delete_array(server_list,server_list_ctr,i);

											list_print(server_list,server_list_ctr);


										}
										else if(numbytes>0)
										{
											buf[numbytes]='\0';
											calc_stats_server(buf);
										}



									}

								}
							}
						}
					}

				}
			}


		}
	}


}
void client_register(char tokens_array[100][100], struct list_connection client_list[5],char listening_port[20])
{

	struct addrinfo hints, *servinfo, *p;
	int rv,sockfd,i;

	char cmd[100],server_send[100];

	//used beej
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(tokens_array[1], tokens_array[2], &hints, &servinfo)) != 0) {//put url of servers instead of 127.0.0.1
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}



	int sprintf_ret=sprintf (client_list[0].id, "%d",1 );
	strcpy(client_list[0].ip_address,tokens_array[1]);
	strcpy(client_list[0].port_no,tokens_array[2]);
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(1);
	}


	//Populate client list with server entry
	sprintf_ret=sprintf (client_list[0].id, "%d",1 );
	strcpy(client_list[0].ip_address,tokens_array[1]);
	strcpy(client_list[0].port_no,tokens_array[2]);
	client_list[0].socket_descriptor=sockfd;
	client_list[0].connected=1;
	struct in_addr ipv4addr;
	struct hostent *he;

	inet_pton(AF_INET, client_list[0].ip_address, &ipv4addr);
	he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
	if(he)
		strcpy(client_list[0].hostname,he->h_name);

	sprintf_ret=sprintf (server_send, "%s#%s", tokens_array[0],listening_port);

	if (send(sockfd,server_send, strlen(server_send), 0) == -1)
		perror("send");


}

int main(int argc, char **argv)
{
	/*Start Here*/
	//int listening_port;
	char listening_port[20];
	int sockfd,n,ret_val;
	int numbytes;
	char buf[100];
	strcpy(listening_port,argv[2]);

	if(strcmp(argv[1],"s")==0)

		is_server(listening_port);
	else
	{
		//Client part



		if(strcmp(argv[1],"c")!=0)
		{
			printf("\nInvalid first argument.First  argument can be s or c. ");
			exit(0);
		}
		int k;
		for(k=0;k<5;k++)
		{
			list_stats[k].upload_ctr=0;
			list_stats[k].download_ctr=0;
			list_stats[k].upload_speed=0;
			list_stats[k].download_speed=0;
		}
		struct in_addr ipv4addr;
		struct hostent *he;
		my_ip=find_my_ip();
		inet_pton(AF_INET, my_ip, &ipv4addr);
		he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
		if(he)
			strcpy(my_hostname,he->h_name);

		fd_set masterlist,watchlist;
		int head_socket =0,rv,sock_index,port, listening_socket,new_fd;
		FD_ZERO(&watchlist);
		FD_ZERO(&masterlist);

		// add our descriptors to the set

		FD_SET(STDIN, &masterlist);
		struct timeval start_time, end_time;
		struct list_connection client_list[5],server_list_onclient[5];

		initialize_list(client_list);
		initialize_list(server_list_onclient);
		int client_list_ctr=1,server_list_ctr=0,peer_id;//0th is server details which we fill in register
		argc=0;
		char cmd[100], *arg,*myip_client,file_name[100];
		int i,c,file_size,bytes_written;



		struct addrinfo hints, *servinfo, *p;
		struct sockaddr_in their_addr;
		socklen_t sin_size;
		char buf[3000],*myip_server;

		int numbytes;
		int yes=1,sprintf_ret;
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE; // use my IP

		if ((rv = getaddrinfo(NULL, listening_port, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			exit(1);
		}

		// loop through all the results and bind to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((listening_socket = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
				perror("server: socket");
				continue;
			}

			if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
					sizeof(int)) == -1) {
				perror("setsockopt");
				exit(1);
			}

			if (bind(listening_socket, p->ai_addr, p->ai_addrlen) == -1) {
				close(listening_socket);
				perror("server: bind");
				continue;
			}

			break;
		}

		if (p == NULL)  {
			fprintf(stderr, "server: failed to bind\n");
			exit(1);
		}

		freeaddrinfo(servinfo); // all done with this structure

		if (listen(listening_socket, 4) == -1) {
			perror("listen");
			exit(1);
		}
		FD_SET(listening_socket, &masterlist);
		head_socket=listening_socket;
		while(1)
		{
			fflush(stdout);
			printf("\n [PA1]$ ");
			fflush(stdout);

			//create listening socket for client




			watchlist=masterlist;
			rv = select(head_socket+1, &watchlist, NULL, NULL, NULL);

			if (rv == -1) {
				perror("select"); // error occurred in select()
			}
			else {
				for(sock_index=0; sock_index<=head_socket;sock_index+=1)
				{

					fflush(stdout);
					if (FD_ISSET(sock_index, &watchlist)) {
						if(sock_index==STDIN)
						{
							argc=0;
							fgets (cmd, 100, stdin);

							fflush(stdout);
							char tokens_array[100][100];
							char s[2] = " ";
							char *token;
							token=(char *)malloc(strlen(cmd)*sizeof(char));

							token = strtok(cmd, s);


							while( token != NULL )
							{

								strcpy(tokens_array[argc],token);
								argc+=1;
								token = strtok(NULL, s);
							}
							//Last character of last string has \n
							c=strlen(tokens_array[argc-1])-1;
							tokens_array[argc-1][c]='\0';


							if((strcasecmp(tokens_array[0],"MYIP"))==0)
							{
								myip_client=find_my_ip();
								if((strcmp(myip_client,"NULL")!=0))
								{

									printf("IP address:%s", myip_client);
									fflush(stdout);
								}
							}
							else if((strcasecmp(tokens_array[0],"TERMINATE"))==0)
							{
								//sockfd
								if((strcmp(tokens_array[1], "1")==0))
									printf("\nCannot terminate connection with server\n");
								else
								{
									int close_sockfd_index=client_terminate(tokens_array[1],client_list,client_list_ctr);
									//printf("\nin terminate initiator , closing, clearing and deleting");

									if(close_sockfd_index!=-1)
									{
										close(client_list[close_sockfd_index].socket_descriptor);
										FD_CLR(client_list[close_sockfd_index].socket_descriptor, &masterlist);
										//remove from array
										client_list_ctr=delete_array(client_list,client_list_ctr,close_sockfd_index);
									}
								}

							}
							else if((strcasecmp(tokens_array[0],"statistics"))==0)
								statistics();

							else if((strcasecmp(tokens_array[0],"exit"))==0)
							{
								//sockfd


								for(i=0;i<client_list_ctr;i++)

									close(i);
								printf("\nClosed all connections and exit");
								exit(0);



							}

							else if((strcasecmp(tokens_array[0],"REGISTER"))==0)
							{

								client_register(tokens_array,client_list,listening_port);
								if(client_list[0].socket_descriptor > head_socket)
									head_socket=client_list[0].socket_descriptor;
								FD_SET(client_list[0].socket_descriptor, &masterlist);

							}//for register if
							else if((strcasecmp(tokens_array[0],"CONNECT"))==0)
							{

								peer_id=client_connect(tokens_array,client_list,client_list_ctr,server_list_onclient,server_list_ctr);
								if(peer_id!=-1)
								{
									if(client_list[peer_id].socket_descriptor > head_socket)
										head_socket=client_list[peer_id].socket_descriptor;
									FD_SET(client_list[peer_id].socket_descriptor, &masterlist);
									client_list_ctr++;
								}

							}//
							//more stdin
							else if((strcasecmp(tokens_array[0],"MYPORT"))==0)
								printf("Port number:%d",atoi(listening_port));
							else if((strcasecmp(tokens_array[0],"CREATOR"))==0)
							{
								printf("\n Tanvi Vijay Bhavsar tanvivij tanvivij@buffalo.edu\n");
								printf("I have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity");
							}
							else if((strcasecmp(tokens_array[0],"LIST"))==0)
								list_print(client_list,client_list_ctr);
							else if((strcasecmp(tokens_array[0],"help"))==0)
								client_help();
							else if((strcasecmp(tokens_array[0],"UPLOAD"))==0)
							{
								int ret_upload=upload_file(tokens_array[1],tokens_array[2],client_list,client_list_ctr);
								if(ret_upload  !=1)
									printf("\nUpload file failed");
							}
							else if((strcasecmp(tokens_array[0],"DOWNLOAD"))==0)
								download_file(tokens_array,argc,client_list,client_list_ctr);
							else
								printf("\nInvalid input\n");

						}
						//If server has sent
						else if(client_list[0].socket_descriptor==sock_index)
						{
							char list_input[1000];
							server_list_ctr=0;

							if ((numbytes = recv(client_list[0].socket_descriptor, list_input, 1000, 0)) == -1) {
								perror("recv");
								exit(1);
							}

							list_input[numbytes] = '\0';



							char s[2] = ":";
							char s2[2]="#",token2[100][100];
							char *token,*token3;
							int ctr=0,token2_ctr=0,sprintf_ret;
							token=(char *)malloc(strlen(list_input)*sizeof(char));

							token = strtok(list_input, s);


							while( token != NULL )
							{

								fflush(stdout);
								strcpy(token2[token2_ctr], token);
								token2_ctr++;


								if((strcmp(token, " ")==0))
									break;
								token = strtok(NULL, s);
							}

							for(i=0;i<token2_ctr;i++)
							{
								token3 = strtok(token2[i], s2);
								ctr=0;
								while(token3!=NULL)
								{

									switch(ctr)
									{
									case 0:	sprintf_ret=sprintf (server_list_onclient[server_list_ctr].id, "%d",(server_list_ctr+1));

									break;
									case 1:	strcpy(server_list_onclient[server_list_ctr].hostname,token3);
									break;
									case 2:	strcpy(server_list_onclient[server_list_ctr].ip_address,token3);
									break;
									case 3:	strcpy(server_list_onclient[server_list_ctr].port_no,token3);
									break;
									}
									ctr++;
									token3 = strtok(NULL, s2);
								}
								server_list_ctr++;

							}


							//print parsed output
							printf("\n");
							printf("\nServer IP list received by client\n");
							list_print(server_list_onclient,server_list_ctr);


						}
						else if (sock_index==listening_socket)
						{

							new_fd = accept(listening_socket, (struct sockaddr *)&their_addr, &sin_size);

							if(new_fd > head_socket)
								head_socket=new_fd;

							FD_SET(new_fd, &masterlist);

							fflush(stdout);
							socklen_t len;
							struct sockaddr_storage addr;
							char iclient_ip[30];
							len = sizeof addr;
							getpeername(new_fd, (struct sockaddr*)&addr, &len);

							if (addr.ss_family == AF_INET) {
								struct sockaddr_in *s = (struct sockaddr_in *)&addr;
								port = ntohs(s->sin_port);
								inet_ntop(AF_INET, &s->sin_addr, iclient_ip, sizeof iclient_ip);
							}
							//update client list


							client_list[client_list_ctr].connected=1;
							client_list[client_list_ctr].socket_descriptor=new_fd;
							strcpy(client_list[client_list_ctr].ip_address,iclient_ip);
							for(i=0;i<server_list_ctr;i++)
							{
								if((strcmp(server_list_onclient[i].ip_address, iclient_ip)==0))
								{
									strcpy(client_list[client_list_ctr].ip_address,iclient_ip);
									strcpy(client_list[client_list_ctr].hostname,server_list_onclient[i].hostname);
									strcpy(client_list[client_list_ctr].port_no,server_list_onclient[i].port_no);
								}
							}

							sprintf_ret=sprintf (client_list[client_list_ctr].id, "%d",(client_list_ctr+1)) ;


							client_list_ctr++;


							printf("Connect success\n");

						}
						//check all connections for input data by comparing socket descriptor of client list with sock_index
						else
						{
							char peer_send[PACKET_SIZE],source_hostname[100];
							int receive_id;
							for(i=1;i<client_list_ctr;i++)
							{

								if (sock_index==client_list[i].socket_descriptor)
								{


									memset(peer_send,'\0',PACKET_SIZE);
									numbytes = recv(client_list[i].socket_descriptor, peer_send, PACKET_SIZE, 0);
									receive_id=atoi(client_list[i].id);
									strcpy(source_hostname,client_list[i].hostname);

									if(numbytes==-1)
									{
										perror("recv");
										exit(1);

									}
									else if (numbytes==0)
									{

										printf("Client received 0 bytes from another client so disconnecting\n");


										//close connection for that socket

										close(client_list[i].socket_descriptor);
										FD_CLR(client_list[i].socket_descriptor, &masterlist);

										client_list[i].socket_descriptor=-1;
										client_list[i].connected=0;


										printf("\nAfter terminate client list is\n");
										//delete entry from array
										client_list_ctr=delete_array(client_list,client_list_ctr,i);

										list_print(client_list,client_list_ctr);

									}
									else//received file
									{

										peer_send[numbytes] = '\0';


										if((strstr(peer_send,"DOWNLOAD#")!=NULL))
											parse_download_string(peer_send,client_list[i].id,client_list,client_list_ctr);
										else
										{
											//refered http://stackoverflow.com/questions/12722904/how-to-use-struct-timeval-to-get-the-execution-time at both places
											if((strstr(peer_send,"filename-")!=NULL))
											{
												//start timer
												printf("\nStarted receiving file");
												gettimeofday(&start_time, NULL);
												bytes_written=0;
												file_size=0;
											}
											//printf("\npeer send is %s",peer_send);
											ret_val=receive_file(peer_send,file_name,&file_size,&bytes_written,numbytes);
											if (ret_val != 1)
											{
												printf("\nCould not receive file");
											}

											if(bytes_written!=0 && bytes_written==file_size)
											{
												gettimeofday(&end_time, NULL);
												long diff=((end_time.tv_sec - start_time.tv_sec)*1000000) +(end_time.tv_usec - start_time.tv_usec);
												long double time_taken=(long double)diff/1000000;


												long double rate=(long double)(file_size*8)/time_taken;

												printf("\nFile %s is downloaded from %s ",file_name,client_list[i].hostname);
												printf("\n Rx: %s -> %s, File Size :%d bytes, Time Taken: %llf seconds, Rx Rate:%llf bits/second\n", client_list[i].hostname,my_hostname,file_size,time_taken,rate);

												list_stats[receive_id].download_ctr+=1;
												if(list_stats[receive_id].download_speed>0)
													list_stats[receive_id].download_speed=(list_stats[receive_id].download_speed+rate)/2;
												else
													list_stats[receive_id].download_speed=rate;
												strcpy(list_stats[receive_id].hostname,source_hostname);
												//send to server
												char stats_string[1000];
												int sprintf_ret=sprintf (stats_string, "%s#%s#%d#%llf#%d#%llf",my_hostname,list_stats[receive_id].hostname,list_stats[receive_id].upload_ctr,list_stats[receive_id].upload_speed,list_stats[receive_id].download_ctr,list_stats[receive_id].download_speed);

												if ( send(client_list[0].socket_descriptor, stats_string,sprintf_ret, 0) == -1)
													perror("send");
												
											}

										}
									}
								}


							}

						}
					}


				}
			}
		}
		

	}


	return 0;
}











