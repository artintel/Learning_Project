#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <fcntl.h>
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
 
#include <sys/epoll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#define nullptr NULL

//mark
#define DNS_SERVER_PORT 53
#define DNS_SERVER_IP 	"114.114.114.114"

#define DNS_HOST		0x01
#define DNS_CNAME		0x05
#define ASYNC_DNS_CLIENT_NUM	1024
/*
void err_sys(const char* x) 
{ 
    perror(x); 
    exit(1); 
}
*/

static int num = 1;


struct async_context{
	int epfd;
	//pthread_t threaid;
};

struct dns_header{

	unsigned short id;
	unsigned short flags;

	unsigned short questions;
	unsigned short answer;

	unsigned short authority;
	unsigned short additional;
	
};

struct dns_question{
	int length;
	unsigned short qtype;
	unsigned short qclass;
	unsigned char* name;
};


struct dns_item{

	char* domain;
	char* ip;
};


typedef void (*async_result_cb)(struct dns_item* domain_list, int count);

struct ep_arg{
	int sockfd;
	async_result_cb cb;
};


//client send to dns server

int dns_create_header(struct dns_header* header){

	if(header == nullptr) return -1;
	memset(header, 0, sizeof(struct dns_header));

	//random
	srandom(time(nullptr));
	header->id = random();

	header->flags = htons(0x0100);
	header->questions = htons(1);

	return 0;
	
}

// host: www.0voice.com
// name: 3www60voice3com0t
int dns_create_question(struct dns_question* question, const char* hostname){
	if(question == nullptr || hostname == nullptr) return -1;
	memset(question, 0, sizeof(struct dns_question));

	question->name = (char*)malloc(strlen(hostname) + 2);
	if(question->name == nullptr) {
		return -2;
	}

	question->length = strlen(hostname) + 2;
	
	question->qtype = htons(1);
	question->qclass = htons(1);

	// name
	const char delim[2] = ".";
	char* qname = question->name;


	
	char* hostname_dup = strdup(hostname);//strdump --> malloc
	char* token = strtok(hostname_dup, delim);

	while(token != nullptr){

		size_t len = strlen(token);

		*qname = len;
		qname++;
		
		strncpy(qname, token, len + 1);
		//strcpy needn't length, endof \0
		qname += len;

		token = strtok(nullptr, delim);
	}

	free(hostname_dup);
}

// 114.114.114.114    8.8.8.8




// struct dns_header* header
// struct dns_question* question
// char* request


int dns_build_requestion(struct dns_header* header, struct dns_question* question, char* request, int rlen){

	if(header == nullptr || question == nullptr || request == nullptr) return -1;
	memset(request, 0, rlen);
	// header --> request

	memcpy(request, header, sizeof(struct dns_header));
	int offset = sizeof(struct dns_header);
	

	
	// question --> request
	memcpy(request+offset, question->name, question->length);
	offset += question->length;

	memcpy(request+offset, &question->qtype, sizeof(question->qtype));
	offset += sizeof(question->qtype);

	memcpy(request+offset, &question->qclass, sizeof(question->qclass));
	offset += sizeof(question->qclass);
	
	return offset;
	
}

static int is_pointer(int in){
	return((in & 0xC0) == 0xC0);
}



void* dns_parse_name(unsigned char* chunk, unsigned char* ptr, char* out, int* len){

	int flag = 0, n = 0, alen = 0;
	char* pos = out + (*len);

	while(1){

		flag = (int)ptr[0];
		if ( flag == 0 ) break;		

		if( is_pointer(flag) ){

			n = (int)ptr[1];
			ptr = chunk + n;
			dns_parse_name(chunk, ptr, out, len);
			break;
		}
		else{

			ptr++;
			memcpy(pos, ptr, flag);
			pos += flag;
			ptr += flag;

			*len += flag;
			if((int)ptr[0] != 0){
				memcpy(pos, ".", 1);
				pos += 1;
				(*len) += 1;
			}
		}
	}
}


static int dns_parse_response(char* buffer, struct dns_item** domains){


	int i = 0;
	unsigned char* ptr = buffer;

	ptr += 4;
	int querys = ntohs(*(unsigned short*)ptr);

	ptr += 2;
	int answers = ntohs(*(unsigned short*)ptr);
	ptr += 6;
	for( i = 0; i < querys; i++ ){
		while(1){

			int flag = (int)ptr[0];
			ptr += (flag + 1);

			if(flag == 0) break;

		}
		ptr += 4;
	}

	char cname[128], aname[128], ip[20], netip[4];
	int len, type, ttl, datalen;

	int cnt = 0;
	struct dns_item* list = (struct dns_item*)calloc(answers, sizeof(struct dns_item));
	if(list == nullptr) return -1;

	for(i = 0; i < answers; i++){
		bzero(aname, sizeof(aname));
		len = 0;

		dns_parse_name(buffer, ptr, aname, &len);
		ptr += 2;

		type = htons(*(unsigned short*)ptr);
		ptr += 4;

		ttl = htons(*(unsigned short*)ptr);
		ptr += 4;

		datalen = ntohs(*(unsigned short*)ptr);
		ptr += 2;

		if(type == DNS_CNAME){

			bzero(cname, sizeof(cname));
			len = 0;
			dns_parse_name(buffer, ptr, cname, &len);
			ptr += datalen;

		}
		else if (type == DNS_HOST){
			bzero(ip, sizeof(ip));

			if(datalen == 4){
				memcpy(netip, ptr, datalen);
				inet_ntop(AF_INET, netip, ip, sizeof(struct sockaddr));

				//printf("%s has address %s\n", aname, ip);
				//printf("\tTime to live: %d minutes, %d seconds\n", ttl / 60, ttl % 60);

				list[cnt].domain = (char*)calloc(strlen(aname) + 1, 1);
				memcpy(list[cnt].domain, aname, strlen(aname));

				list[cnt].ip = (char*)calloc(strlen(ip) + 1, 1);
				memcpy(list[cnt].ip, ip, strlen(ip));

				cnt++;
			}

			ptr += datalen;

		}

	}

	*domains = list;
	ptr += 2;
	//printf("cnt: %d\n", cnt);

	return cnt;
}


void dns_async_client_destory_domains(struct dns_item* domain_list, int count){

	if(domain_list == nullptr) return;

	int i = 0;
	for(; i < count; i++){
		free(domain_list[i].domain);
		free(domain_list[i].ip);
	}
	free(domain_list);
}

static void* dns_async_client_callback(void* arg){
	struct async_context* ctx = (struct async_context*)arg;

	if(ctx == nullptr) return nullptr;

	int epfd = ctx->epfd;

	while(1){

		struct epoll_event events[ASYNC_DNS_CLIENT_NUM] = { 0 };

		int num_event = epoll_wait(epfd, events, ASYNC_DNS_CLIENT_NUM, -1);
		printf("num_event: %d\n", num_event);
		if (num_event < 0) {
			if (errno == EINTR || errno == EAGAIN) {
				continue;
			} else {
				break;
			}
		} else if (num_event == 0) {
			continue;
		}


		int i = 0;
		printf("here:\n");
		for(i = 0; i < num_event; i++){
			printf("num_event: %d\n", num_event);
			struct ep_arg* data = (struct ep_arg*)events[i].data.ptr;
			if(data == nullptr) {
				return nullptr;
			}

			int sockfd = data->sockfd;
			char response[1024] = { 0 };
			struct sockaddr_in addr;
			size_t addr_len = sizeof(struct sockaddr_in);

			
			int n = recvfrom(sockfd, response, sizeof(response), 0, (struct sockaddr*)&addr, (socklen_t*)&addr_len);

			// if(n < 0) continue;


			struct dns_item* dns_domain = nullptr;
			int count = dns_parse_response(response, &dns_domain);

			data->cb(dns_domain, count);

			int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, nullptr);
			close(sockfd);
			
			dns_async_client_destory_domains(dns_domain, count);
			free(data);
		}

	}
}

struct async_context* dns_async_client_init(void){
	int epfd = epoll_create(1);
	if(epfd < 0){
		printf("epfd_create_error: %d\n", epfd);
		return nullptr;
	}
	struct async_context* ctx = calloc(1, sizeof(struct async_context));
	if (ctx == NULL) {
		close(epfd);
		return NULL;
	}


	ctx->epfd = epfd;

	
	pthread_t threadid;
	int ret = pthread_create(&threadid, nullptr, &dns_async_client_callback, ctx);
	if(ret){
		perror("pthread_create");
		return nullptr;
	}
		
	//ctx->threaid = threadid;
	usleep(1);
	
	return ctx;
	
}


void set_nonblock(int fd, int flags)
{
	int val;

	if((val = fcntl(fd, F_GETFL, 0)) < 0)
	//err_sys("fcntl F_GETFL error");
	perror("fcntl F_GETFL error");

	val |= flags; /*turn on flags*/

	if(fcntl(fd, F_SETFL, val) < 0)
		//err_sys("fcntl F_SETFL error");
		perror("fcntl F_SETFL error");
}

static int set_block(int fd, int block) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) return flags;

	if (block) {        
		flags &= ~O_NONBLOCK;    
	} else {        
		flags |= O_NONBLOCK;    
	}

	if (fcntl(fd, F_SETFL, flags) < 0) return -1;

	return 0;
}



int dns_async_client_commit(struct async_context* ctx, const char* domain, async_result_cb cb){

	if (ctx == nullptr) return -1;
	if (domain == nullptr) return -2;
	
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) return -3;

	// nonblock
	//set_nonblock(sockfd, O_NONBLOCK);
	set_block(sockfd, 0);

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(DNS_SERVER_PORT);
	servaddr.sin_addr.s_addr = inet_addr(DNS_SERVER_IP);

	int ret = connect(sockfd, (struct sockaddr* )&servaddr, sizeof(servaddr));
	//printf("connect : %d\n", ret);

	struct dns_header header = { 0 };
	dns_create_header(&header);
	
	struct dns_question question = { 0 };
	dns_create_question(&question, domain);

	char request[1024] = { 0 };
	int length = dns_build_requestion(&header, &question, request, 1024);

	//request
	int slen = sendto(sockfd, request, length, 0, (struct sockaddr* )&servaddr, sizeof(struct sockaddr));

	struct ep_arg* eparg = (struct ep_arg*)calloc(1, sizeof(struct ep_arg));
	if(eparg == nullptr) {
		close(sockfd);
		return -1;
	}

	eparg->sockfd = sockfd;
	eparg->cb = cb;


	struct epoll_event ev;
	ev.data.ptr = eparg;
	ev.events = EPOLLIN;

	ret = epoll_ctl(ctx->epfd, EPOLL_CTL_ADD, sockfd, &ev);

	return ret;
}


char *domain[] = {
	"www.ntytcp.com",
	"bojing.wang",
	"www.baidu.com",
	"tieba.baidu.com",
	"news.baidu.com",
	"zhidao.baidu.com",
	"music.baidu.com",
	"image.baidu.com",
	"v.baidu.com",
	"map.baidu.com",
	"baijiahao.baidu.com",
	"xueshu.baidu.com",
	"cloud.baidu.com",
	"www.163.com",
	"open.163.com",
	"auto.163.com",
	"gov.163.com",
	"money.163.com",
	"sports.163.com",
	"tech.163.com",
	"edu.163.com",
	"www.taobao.com",
	"q.taobao.com",
	"sf.taobao.com",
	"yun.taobao.com",
	"baoxian.taobao.com",
	"www.tmall.com",
	"suning.tmall.com",
	"www.tencent.com",
	"www.qq.com",
	"www.aliyun.com",
	"www.ctrip.com",
	"hotels.ctrip.com",
	"hotels.ctrip.com",
	"vacations.ctrip.com",
	"flights.ctrip.com",
	"trains.ctrip.com",
	"bus.ctrip.com",
	"car.ctrip.com",
	"piao.ctrip.com",
	"tuan.ctrip.com",
	"you.ctrip.com",
	"g.ctrip.com",
	"lipin.ctrip.com",
	"ct.ctrip.com"
};


static void dns_async_client_result_callback(struct dns_item* list, int count){
	int i;
	for(i = 0; i < count; i++){
		printf("nams: %s\t ip:%s\n", list[i].domain, list[i].ip);
	}
}




int main(int argc, char* argv[]){
#if 0
	if(argc < 2) return -1;
	dns_client_commit(argv[1]);
#else

	struct async_context* ctx = dns_async_client_init();
	if(ctx == nullptr) return -1;

	int count = sizeof(domain) / sizeof(domain[0]);
	printf("count : %d\n", count);
	int i = 0;
	for(; i < count; i++ ){
		dns_async_client_commit(ctx, domain[i], dns_async_client_result_callback);
		//sleep(2);

	}
	getchar();

#endif


}








