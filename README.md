# count_word

一个统计文本中单词有多少个的小脚本

更新了新的功能 - 计算每个单词一共出现多少次(并没有完全做对比，单词太多了。)

`gcc -o count count.c`

`./count a.txt`

# contacts

通讯录功能。

> gcc -o contacts contacts.c
>
> ./contacts
>
> 1. 输入用户 name & phone number
> 2. 打印刚刚所有插入的用户
> 3. delete 所输入的 用户
> 4. search 用户
> 5. save 用户至自定义文件
> 6. load 加载文件并读取里面的用户信息

# lock

基础的多线程编程，分别尝试使用了

> 互斥锁 mutex
>
> 自旋锁 spinlock
>
> 原子操作 \__asm__ volatile()
>
> gcc -o lock lock.c -lpthread
>
> ./lock

# pthread_pool

线程池的创建

注意mutex & pthread_cond_wait()  pthread_cond_broadcast() 等关键函数的调用

消化好本项目过程中接口之间的传递，线程之间的转换过程的写法和原理。

了解好 gdb 调试，学会如何使用 gdb

> gcc -o pthreadpool pthreadpool.c -lpthread -g
>
> ./pthreadpool
>
> `gdb 调试`
>
> ```powershell
>gcc -o pthreadpool pthreadpool.c -lpthread -g
> 
>gdb ./pthreadpool
> 
>b xxx(行) (breakpoint)
> 
>b 124
> 
>r (runing)
> 
>c (continue)
> ```

# MySQL

> 创建数据库
>
> 使用数据库
>
> 创建表格

`gcc -o Mysql Mysql.c -I /user/include/mysql/ -lmysqlclient`  `-I 系统头文件  -lmysqlclient: 引入我们编译中使用的库`

```
#include <mysql.h>
可能找不到
如果确实存在 /user/include/mysql/mysql.h
就改成
#include <mysql/mysql.h>

如果在上面的目录没有找到mysql.h文件，需要安装mysql-devel但是注意在ubuntu下安装mysql-devel命令：sudo apt-get install libmysqld-dev

如果你不想将#include<mysql.h>改成#include <mysql/mysql>的话，而且你是用的gcc编译的话，那么可以这样
gcc -I/user/include/mysql

# DELETE FROM TBL_USER WHERE U_NAME='Mayc'; # 提醒不安全，因为可能会同时有多条数据被删除



-- SET SQL_SAFE_UPDATES=0; # 设置成安全模式, 三条语句同时做
-- DELETE FROM TBL_USER WHERE U_NAME='Mayc';
-- SET SQL_SAFE_UPDATES=1;
sql -->
# 更好的方式

DELIMITER $$
CREATE PROCEDURE PROC_DELETE_USER(IN UNAME VARCHAR(32))
BEGIN
SET SQL_SAFE_UPDATES=0; # 设置成安全模式, 三条语句同时做
DELETE FROM TBL_USER WHERE U_NAME=UNAME;
SET SQL_SAFE_UPDATES=1;
END$$

CALL PROC_DELETE_USER('qiuxiang')
```

## 作业题：实现 mysql 数据库的连接池

> 主要的思路还是根据之前的pthread_pool来进行改写。
>
> 需要注意的地方是，既然是多连接池，那么就肯定会应用到多线程。技术能力不够，在这里就没有再去做一个线程池配合，单纯的就是为每一个数据库连接配了一个线程`threadid`。
>
> 我将整个过程分为如下几个部分：
>
> - 数据库连接结构体的创建
> - 用于管理数据库的相当于管理员的结构体，用来管理创建的数据库连接
> - 创建定量的数据库连接 --- 并且用链表连接管理，配合`管理员`结构体
> - 配合多线程进行并发处理，要注意临界资源的锁控制。
> - 将之前的 `mysql `的一些操作函数封装在 `struct tast` 结构体中，并作为任务分配给每一个数据库连接, 每有一个新的 `task` 需要唤醒一个数据库连接(等同于唤醒一个线程)，或者说是从数据库连接池中取出一个空闲的连接。完成 `task` 之后要释放，也就是将一个数据库连接回收到连接池



# DNS-UDP

![1](https://github.com/artintel/Learning_Project/blob/master/dns/1.png)

## Queries

> 查询名：
>
> 长度不固定，且不使用填充字节，一般该字段表示的就是需要查询的域名(如果是反向查询，则为IP，反向查询即由IP地址反查域名)，一般的格式如下图所示

![2](https://github.com/artintel/Learning_Project/blob/master/dns/2.png)

`0voice.com;  name:60voice3com`

`www.0voice.com; name:3www60voice3com`



在网络中，就是将 `dns_head` 和 `dns_question` 两个结构体内的信息发送给dns服务器

![3](https://github.com/artintel/Learning_Project/blob/master/dns/3.png)

在C/C++写网络程序的时候，往往会遇到字节的网络l顺序和主机顺序的问题。这是就可能用到htonl(), ntohl(), ntohs()，htons()这4个函数。
网络字节顺序与本地字节顺序之间的转换函数：

htonl()--"Host to Network Long"
ntohl()--"Network to Host Long"
htons()--"Host to Network Short"
ntohs()--"Network to Host Short"

## function()

字符串切分 `strtok(string, delim)` -- `strtok(www.0voice.com, ".")；` 

- string -- 需要切分的字符串， 
- delim -- 规则

在后续接着切分的时候变成 `strtok(nullptr, delim)`, 由此可发现，该函数是一个线程非安全函数

`strncpy(qname, token, len + 1)` 

和 `strcpy()` 不同，`strncpy()` 需要指定长度



## sendto() 发送

### dns_client_commit(const char* domain)

dns客户端比较固定的写法

```C
int dns_client_commit(const char* domain){
	
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) return -1;

	struct sockaddr_in servaddr = { 0 };
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(DNS_SERVER_PORT);
	servaddr.sin_addr._addr = inet_addr(DNS_SERVER_IP);


	struct dns_header header = { 0 };
	dns_create_header(&header);
	
	struct dns_question question = { 0 };
	dns_create_question(question, domain);


	char request[1024] = { 0 };
	int length = dns_build_requestion(&header, question, request);

	//request
	sendto(fd, request, length, 0, servaddr, addr_len);
	
}
```

### dns_build_requestion(struct dns_header* header, struct dns_question* question, char* request, int rlen)

将header和question的数据打包并返回包长度

```C
int dns_build_requestion(struct dns_header* header, struct dns_question* question, char* request, int rlen){

	if(header == nullptr || dns_question == nullptr || request == nullptr) return -1;
	memset(request, 0, rlen);
	// header --> request

	memcpy(request, header, sizeof(struct dns_header));
	int offset = sizeof(struct dns_header);
	

	
	// question --> request
	memcpy(request+offset, question->qname, question->length);
	offset += question->length;

	memcpy(requestion+offset, question->qtype, sizeof(question->qtype));
	offset += sizeof(question->qtype);

	memcpy(requestion+offset, question->qclass, sizeof(question->qclass));
	offset += sizeof(question->qclass);
	
	return offset;
	
}
```

## recvfrom() 接收

`send()` 和 `recvfrom()` 是一个流程的操作，属于同步操作，在recvfrom()和sendto()之间自动加锁，完成接收才结束



## UDP和TCP相比，TCP不具备的

1. UDP传输速度快 (下载)
2. UDP相应速度快 (游戏)



## 作业：DNS异步查询

引入`epoll`，以下是 `epoll` 的接口介绍

### epoll 接口

1. `int epoll_create(int size);`

   创建一个`epoll`的句柄，`size`用来告诉内核这个监听的数目一共有多大。这个参数不同于`select()`中的第一个参数，给出最大监听的`fd+1`的值。需要注意的是，当创建好`epoll`句柄后，它就是会占用一个`fd`值，在`linux`下如果查看`/proc/`进程`id/fd/`，是能够看到这个`fd`的，所以在使用完`epoll`后，必须调用`close()`关闭，否则可能导致fd被耗尽。(这里我就遇到了相反的麻烦，提前把 `epoll` 给关闭了)



2. `int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);`
   `epoll`的事件注册函数，它不同与`select()`是在监听事件时告诉内核要监听什么类型的事件，而是在这里先注册要监听的事件类型。第一个参数是`epoll_create()`的返回值，第二个参数表示动作，用三个宏来表示：
   
   - EPOLL_CTL_ADD：注册新的fd到epfd中；
   
   - EPOLL_CTL_MOD：修改已经注册的fd的监听事件；
   
   - EPOLL_CTL_DEL：从epfd中删除一个fd；
   
     第三个参数是需要监听的`fd`，第四个参数是告诉内核需要监听什么事，`struct epoll_event`结构如下：

```
typedef union epoll_data {
  void *ptr;
  int fd;
  __uint32_t u32;
  __uint64_t u64;
} epoll_data_t;

struct epoll_event {
  __uint32_t events; /* Epoll events */
  epoll_data_t data; /* User data variable */
};
```

 `events`可以是以下几个宏的集合：

- EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
- EPOLLOUT：表示对应的文件描述符可以写；
- EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
- EPOLLERR：表示对应的文件描述符发生错误；
- EPOLLHUP：表示对应的文件描述符被挂断；
- EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
- EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里



3. `int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);`

   `epoll`的事件注册函数，`epoll_ctl`向 `epoll`对象中添加、修改或者删除感兴趣的事件，返回0表示成功，否则返回 `–1`，此时需要根据 `errno` 错误码判断错误类型。等待事件的产生，类似于`select()`调用。参数`events`用来从内核得到事件的集合，`maxevents`告之内核这个`events`有多大，这个 `maxevents`的值不能大于创建`epoll_create()`时的`size`，参数`timeout`是超时时间（毫秒，0 会立即返回，-1 将不确定，也有说法说是永久阻塞）。该函数返回需要处理的事件数目，如返回 `0` 表示已超时。

- 第1个参数 epfd是 epoll的描述符。
- 第2个参数 events则是分配好的 epoll_event结构体数组，==epoll将会把发生的事件复制到 events数组中（events不可以是空指针，内核只负责把数据复制到这个 events数组中，不会去帮助我们在用户态中分配内存。内核这种做法效率很高）==。
- 第3个参数 maxevents表示本次可以返回的最大事件数目，通常 maxevents参数与预分配的events数组的大小是相等的。
- 第4个参数 timeout表示在没有检测到事件发生时最多等待的时间（单位为毫秒），如果 timeout为0，则表示 epoll_wait在 rdllist链表中为空，立刻返回，不会等待。

### fcntl(int fd, int cmd, .../* struct flock *flockptr */) -- 记录锁

`fcntl() `用来操作文件描述符(进程的文件描述符, 相关细节在 `《UNIX环境高级编程》p392`  中可以查询)的一些特性。

`cmd` 是 F_GETLK、F_SETLK 或 F_SETLKW。

### O_NONBLOCK
为文件的本次打开操作和后续的 I/O 操作设置非阻塞方式。 



和百度的服务器连接

1. 建立TCP连接
2. 在TCP连接，socket的基础上，向百度服务器发送http协议请求
3. 服务器在TCP连接socket，返回http协议 -- response



# http 请求连接

## 客户端请求消息格式

![image-20201210163839827](https://github.com/artintel/Learning_Project/blob/master/Http/image-20201210163839827.png)

```bash
GET /hello.txt HTTP/1.1
User-Agent: curl/7.16.3 libcurl/7.16.3
OpenSSL/0.9.1l zlib/1.2.3
Host: www.example.com
Accept-Language: en, mi
```

## 实现流程

1. www.baidu.com --> 翻译为 ip 地址。 DNS
2. TCP 连接这个 ip 地址： 端口
3. 发送 http 协议

>  `API -- gethostbyname()`定义：
>
> 返回对应于给定主机名的包含主机名字和地址信息的 `hostent` 结构的指定指针。
>
> ```C
> struct hostent{
>     char *h_name;
>     char ** h_aliases;
>     short h_addrtype;
>     short h_length;
>     char ** h_addr_list;
>     
> }
> struct hostent* gethostbyname(const char* name){
>     err_t err;
>     ip_addr_t addr;
>     /* buffer variables for lwip_gethostbyname */
>     HOSTENT_STORAGE struct hostent s_hostent;
>     HOSTENT_STORAGE char *s_aliases;
>     HOSTENT_STORAGE ip_addr_t s_hostent_addr;
>     HOSTENT_STORAGE ip_addr_t *s_phostent_addr[2];
>     /* query host IP address */
>     err = netconn_gethostbyname(name, &addr);
>     if (err != ERR_OK) {
>         LWIP_DEBUGF(DNS_DEBUG, ("lwip_gethostbyname(%s) failed, err=%d\n", name, err));
> h_errno = HOST_NOT_FOUND;
>     }
>     return NULL;
> }
> ```

>  `inet_ntoa()` 将 `unsigned int --> char*`

## 实现TCP连接服务器

> `http_create_socket(char* ip)` 创建`socket`用于连接相应的服务器，`ip `来自于 `gethostbyname()`
>
> 创建`socket` 和 `sockadd_in` 结构体
>
> 连接 connect(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in))
>
> 设置非阻塞I/O `fcntl(sockfd, F_SETFL, O_NONBLOCK);`

## send http 请求

> `int http_send_request(const char* hostname, const char* resource)`
>
> hostname: github.com
> resource: /artintel
>
> 1. 通过函数获取ip地址 `host_to_ip(const char* hostname);`
>
> 2. 建立连接 `http_create_socket()`
>
> 3. 发送 `socket` `send(sockfd, buffer, strlen(buffer), 0);`
>
> 4. 接收 `response`
>
>    `select` 检测，监听， 网络 `io` 里是否有可读的数据
>
>    `select(maxfd + 1, &rset, &wset, &eset, NULL);`
>
>    - `maxfd:` 可监听的最大 `sockfd` 数量
>    - `rset:` 哪些 I/O 可读
>    - `wset:` 哪些 I/O 可写
>    - `eset:` 哪些 I/O 出错
>    - `NULL:` 多长时间轮询一次



