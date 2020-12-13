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

```bash
$ gcc -o http_request http_request.c -lpthread
$ ./http_request www.baidu.com /
```

 

# TCP服务器

1. 并发服务器
   -  一请求一线程 (已不被推崇)
   -  IO 多路复用, epoll
2. TCP服务器百万级连接

![image](https://github.com/artintel/Learning_Project/blob/master/TCP_EPOLL/image-20201210213520326.png)

# 一请求一线程

## main()

申请一个 `int sockfd = socket(AF_INET, SOCK_STREAM, 0);`

初始化一个实例 `sockaddr_in`

```
struct sockaddr_in addr;
memset(&addr, 0, sizeof(struct sockaddr_in));
add.sin_family = AF_INET;
addr.sin_port = htons(port);
addr.sin_addr.s_addr = INADDR_ANY;
```

`bind();`

> 将一本地地址与一套接字捆绑。本函数适用于未连接的数据报或流类套接字，在`connect()`或`listen()`调用前使用。当用`socket()`创建套接字后，它便存在于一个名字空间（地址族）中，但并未赋名。`bind()`函数通过给一个未命名套接字分配一个本地名字来为套接字建立本地捆绑（主机地址/端口号）。
>
> `bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))`

`listen();`

> 创建一个套接字并监听申请的连接
>
> ```c
> #include <sys/socket.h>
> int listen(int sockfd, int backlog);
> // sockfd: 用于标识一个已捆包未连接套接字的文件描述符
> // backlog: 等待连接队列的最大长度
> ```

`accept()`;

> `accept()`接收一个套接字中已建立的连接,来自客户端的连接。`accept()` 是c语言中网络编程的重要的函数，本函数从s的等待连接队列中抽取第一个连接，创建一个与s同类的新的套接字并返回句柄。
>
> **提取出所监听套接字的等待连接队列中第一个连接请求**，**创建一个新的套接字，并返回指向该套接字的文件描述符**。
>
> 新建立的套接字不再监听状态，原来所监听的套接字也不受该系统调用的影响。
>
> 新建立的套接字准备发送 `send()` 和接收数据 `recv()` 。
>
> ```C
> SOCKET accept(
> 	SOCKET s,
> 	struct sockaddr FAR　*addr,
> 	int FAR *addrlen
> 
> );
> ```
>
> `addr` 用于存放客户端的地址，`addrlen` 在调用函数时被设置为`addr` 指向区域的长度，在函数调用结束后被设置为实际地址信息的长度。本函数会阻塞等待知道有客户端请求到达。
>
> 返回值是一个新的套接字描述符，它代表的是和客户端的新的连接，可以把它理解成是一个客户端的`socket`,这个`socket`包含的是客户端的ip和port信息 。（当然这个 `new_socket` 会从 `sockfd` 中继承服务器的`ip`和`port`信息），而参数中的`SOCKET s`包含的是服务器的 `ip` 和 `port` 信息 。



单纯的 `sockfd` 无法解决多个客户端连接时，如何分辨的问题

可通过应用层协议来解决

# EPOLL I/O 多路复用

1. `int epoll_create(int size);`

   创建一个`epoll`的句柄，`size`用来告诉内核这个监听的数目一共有多大。这个参数不同于`select()`中的第一个参数，给出最大监听的`fd+1`的值。需要注意的是，当创建好`epoll`句柄后，它就是会占用一个`fd`值，在`linux`下如果查看`/proc/`进程`id/fd/`，是能够看到这个`fd`的，所以在使用完`epoll`后，必须调用`close()`关闭，否则可能导致fd被耗尽。

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
- ==EPOLLET==： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
- EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里

## 边沿触发EPOLLETvs水平触发

epoll事件有两种模型，边沿触发：edge-triggered (ET)， 水平触发：level-triggered (LT)

## 水平触发(level-triggered)

socket接收缓冲区不为空 有数据可读 读事件一直触发
socket发送缓冲区不满 可以继续写入数据 写事件一直触发
边沿触发(edge-triggered)

socket的接收缓冲区状态变化时触发读事件，即空的接收缓冲区刚接收到数据时触发读事件
socket的发送缓冲区状态变化时触发写事件，即满的缓冲区刚空出空间时触发读事件
边沿触发仅触发一次，水平触发会一直触发。






3. `int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);`

   `epoll`的事件注册函数，`epoll_ctl`向 `epoll`对象中添加、修改或者删除感兴趣的事件，返回0表示成功，否则返回 `–1`，此时需要根据 `errno` 错误码判断错误类型。等待事件的产生，类似于`select()`调用。参数`events`用来从内核得到事件的集合，`maxevents`告之内核这个`events`有多大，这个 `maxevents`的值不能大于创建`epoll_create()`时的`size`，参数`timeout`是超时时间（毫秒，0 会立即返回，-1 将不确定，也有说法说是永久阻塞）。该函数返回需要处理的事件数目，如返回 `0` 表示已超时。

- 第1个参数 epfd是 epoll的描述符。
- 第2个参数 events则是分配好的 epoll_event结构体数组，==epoll将会把发生的事件复制到 events数组中（events不可以是空指针，内核只负责把数据复制到这个 events数组中，不会去帮助我们在用户态中分配内存。内核这种做法效率很高）==。
- 第3个参数 maxevents表示本次可以返回的最大事件数目，通常 maxevents参数与预分配的events数组的大小是相等的。
- 第4个参数 timeout表示在没有检测到事件发生时最多等待的时间（单位为毫秒），如果 timeout为0，则表示 epoll_wait在 rdllist链表中为空，立刻返回，不会等待。

```bash
$ gcc -o tcp_server tcp_server.c
$ ./tcp_server 8888
```

# TCP_高并发

 

# TCP服务器

1. 并发服务器
   -  一请求一线程 (已不被推崇)
   -  IO 多路复用, epoll
2. TCP服务器百万级连接

<img src="C:\Users\HS\AppData\Roaming\Typora\typora-user-images\image-20201210213520326.png" alt="image-20201210213520326" style="zoom: 33%;" />

# 一请求一线程

## main()

申请一个 `int sockfd = socket(AF_INET, SOCK_STREAM, 0);`

初始化一个实例 `sockaddr_in`

```
struct sockaddr_in addr;
memset(&addr, 0, sizeof(struct sockaddr_in));
add.sin_family = AF_INET;
addr.sin_port = htons(port);
addr.sin_addr.s_addr = INADDR_ANY;
```

`bind();`

> 将一本地地址与一套接字捆绑。本函数适用于未连接的数据报或流类套接字，在`connect()`或`listen()`调用前使用。当用`socket()`创建套接字后，它便存在于一个名字空间（地址族）中，但并未赋名。`bind()`函数通过给一个未命名套接字分配一个本地名字来为套接字建立本地捆绑（主机地址/端口号）。
>
> `bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))`

`listen();`

> 创建一个套接字并监听申请的连接
>
> ```c
> #include <sys/socket.h>
> int listen(int sockfd, int backlog);
> // sockfd: 用于标识一个已捆包未连接套接字的文件描述符
> // backlog: 等待连接队列的最大长度
> ```

`accept()`;

> `accept()`接收一个套接字中已建立的连接,来自客户端的连接。`accept()` 是c语言中网络编程的重要的函数，本函数从s的等待连接队列中抽取第一个连接，创建一个与s同类的新的套接字并返回句柄。
>
> **提取出所监听套接字的等待连接队列中第一个连接请求**，**创建一个新的套接字，并返回指向该套接字的文件描述符**。
>
> 新建立的套接字不再监听状态，原来所监听的套接字也不受该系统调用的影响。
>
> 新建立的套接字准备发送 `send()` 和接收数据 `recv()` 。
>
> ```C
> SOCKET accept(
> 	SOCKET s,
> 	struct sockaddr FAR　*addr,
> 	int FAR *addrlen
> 
> );
> ```
>
> `addr` 用于存放客户端的地址，`addrlen` 在调用函数时被设置为`addr` 指向区域的长度，在函数调用结束后被设置为实际地址信息的长度。本函数会阻塞等待知道有客户端请求到达。
>
> 返回值是一个新的套接字描述符，它代表的是和客户端的新的连接，可以把它理解成是一个客户端的`socket`,这个`socket`包含的是客户端的ip和port信息 。（当然这个 `new_socket` 会从 `sockfd` 中继承服务器的`ip`和`port`信息），而参数中的`SOCKET s`包含的是服务器的 `ip` 和 `port` 信息 。



单纯的 `sockfd` 无法解决多个客户端连接时，如何分辨的问题

可通过应用层协议来解决

# EPOLL I/O 多路复用

1. `int epoll_create(int size);`

   创建一个`epoll`的句柄，`size`用来告诉内核这个监听的数目一共有多大。这个参数不同于`select()`中的第一个参数，给出最大监听的`fd+1`的值。需要注意的是，当创建好`epoll`句柄后，它就是会占用一个`fd`值，在`linux`下如果查看`/proc/`进程`id/fd/`，是能够看到这个`fd`的，所以在使用完`epoll`后，必须调用`close()`关闭，否则可能导致fd被耗尽。

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
- ==EPOLLET==： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
- EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里

## 边沿触发EPOLLETvs水平触发

epoll事件有两种模型，边沿触发：edge-triggered (ET)， 水平触发：level-triggered (LT)

## 水平触发(level-triggered)

socket接收缓冲区不为空 有数据可读 读事件一直触发
socket发送缓冲区不满 可以继续写入数据 写事件一直触发
边沿触发(edge-triggered)

socket的接收缓冲区状态变化时触发读事件，即空的接收缓冲区刚接收到数据时触发读事件
socket的发送缓冲区状态变化时触发写事件，即满的缓冲区刚空出空间时触发读事件
边沿触发仅触发一次，水平触发会一直触发。






3. `int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);`

   `epoll`的事件注册函数，`epoll_ctl`向 `epoll`对象中添加、修改或者删除感兴趣的事件，返回0表示成功，否则返回 `–1`，此时需要根据 `errno` 错误码判断错误类型。等待事件的产生，类似于`select()`调用。参数`events`用来从内核得到事件的集合，`maxevents`告之内核这个`events`有多大，这个 `maxevents`的值不能大于创建`epoll_create()`时的`size`，参数`timeout`是超时时间（毫秒，0 会立即返回，-1 将不确定，也有说法说是永久阻塞）。该函数返回需要处理的事件数目，如返回 `0` 表示已超时。

- 第1个参数 `epfd `是 `epoll` 的描述符。
- 第2个参数 `events` 则是分配好的 `epoll_event` 结构体数组，==epoll将会把发生的事件复制到 events数组中（events不可以是空指针，内核只负责把数据复制到这个 events数组中，不会去帮助我们在用户态中分配内存。内核这种做法效率很高）==。
- 第3个参数 `maxevents` 表示本次可以返回的最大事件数目，通常 `maxevents` 参数与预分配的 `events` 数组的大小是相等的。
- 第4个参数 `timeout` 表示在没有检测到事件发生时最多等待的时间（单位为毫秒），如果 `timeout` 为 `0`，则表示 `epoll_wait` 在 `rdllist` 链表中为空，立刻返回，不会等待。



# TCP_高并发

可能遇到的问题记录:

1. `connection refused` 服务器不允许连接 

   在服务器机如下设置

   文件系统会默认每个进程的 `fd` 个数为1024

   ​	修改： `/etc/security/limits.conf`

   ​	命令： `ulimit -a`

   ​				`ulimit -n 1048576` (sudo su) (临时修改)

   ​				`sudo vim /etc/security/limits.conf`

   ​				在最后一行之前添加 

   ​				`* hard nofile 1048576`

   ​				`* soft nofile 1048576`

2. `Cannot assign requested address`

   端口个数可能不够

   sockfd 与网络地址之间的关系 --> sockfd 与 ip 地址有什么关系

   sockfd --> (远程ip, 远程端口， 本机ip, 本机端口， proto)

   多开辟几个端口以供使用

3. `Connection timed out`

   fd 个数与 fd 的最大值影响

   在客户端机如下设置：

   `cat /proc/sys/fs/file-max   fs.file-max = 1048576   `

   `cat /proc/sys/net/netfilter/nf_conntrack_max  nf_conntrack_max = 1048576`

   如果 `nf_conntrack_max` 不显示 按照以下方式设置：

   ```bash
   # 若提示 sysctl: cannot stat /proc/sys/net/nf_conntrack_max: No such file or directory 执行如下：
   sudo su
   modprobe ip_conntrack
   lsmod |grep conntrack
   
   $ sudo sysctl -p
   ...
   fs.file-max = 1048576
   net.nf_conntrack_max = 1048576
   ```

   设置方法：

   ```
   sudo vim /etc/sysctl.conf 
   在 #net.ipv4.conf.all.log_martians = 1 之下输入
   fs.file-max = 1048576
   net.nf_conntrack_max = 1048576
   ```

4. `Cannot open /proc/meminfo: Too many open files in system`

   在服务器端执行第3步骤

5. 内存回收 大数量的TCP连接请求，很可能导致服务器内存的使用过载

   所以引入TCP的协议栈

   在刚刚的文件添加位置之下再添加

   ```
   net.ipv4.tcp_mem = 252114 524288 786432
   net.ipv4.tcp_wmem = 1024 1024 2048
   net.ipv4.tcp_rmem = 1024 1024 2048
   ```

   **tcp_mem **(3个`INTEGER`变量)：`low, pressure, high`
   **low**：当 `TCP` 使用了低于该值的内存页面数时，`TCP` 不会考虑释放内存。(理想情况下，这个值应与指定给 `tcp_wmem` 的第 2 个值相匹配 - 这第 2 个值表明，最大页面大小乘以最大并发请求数除以页大小 `(131072 * 300 / 4096)`。
   **pressure**：当 `TCP` 使用了超过该值的内存页面数量时，`TCP` 试图稳定其内存使用，进入 `pressure` 模式，当内存消耗低于 `low` 值时则退出 `pressure` 状态。(理想情况下这个值应该是 TCP 可以使用的总缓冲区大小的最大值` (204800 * 300 / 4096)`。
   **high**：允许所有 `tcp sockets` 用于排队缓冲数据报的页面量。(如果超过这个值，`TCP` 连接将被拒绝，这就是为什么不要令其过于保守` (512000 * 300 / 4096) `的原因了。 在这种情况下，提供的价值很大，它能处理很多连接，是所预期的 2.5 倍；或者使现有连接能够传输 2.5 倍的数据.

   

   **tcp_wmem**(3个INTEGER变量)： `min, default, max`
   **min**：为 `TCP socket` 预留用于发送缓冲的内存最小值。每个 `tcp socket` 创建后都可以使用它。默认值为 `4096(4K)`。
   **default**：为 `TCP socket` 预留用于发送缓冲 `buffer` 的内存数量，默认情况下该值会影响其它协议使用的`net.core.wmem_default` 值，一般要低于 `net.core.wmem_default` 的值。默认值为 `16384(16K)`。
   **max**: 用于 `TCP socket` 发送缓冲的内存最大值。该值不会影响 `net.core.wmem_max`，"静态"选择参数`SO_SNDBUF`则不受该值影响。默认值为 `131072(128K)`。（对于服务器而言，增加这个参数的值对于发送数据很有帮助）

   

   **tcp_rmem() 同理 tcp_wmem()**
   
   `/etc/sysctl.conf` 是一个允许你改变正在运行中的 Linux系统的接口。它包含一些 `TCP/IP` 堆栈和虚拟内存系统的高级选项，可用来控制 Linux 网络配置，由于 `/proc/sys/net` 目录内容的临时性，建议把`TCP/IP` 参数的修改添加到 `/etc/sysctl.conf` 文件, 然后保存文件，使用命令 `sudo sysctl –p` 使之立即生效。





```bash
# 服务器端
$ gcc -o tcp_server tcp_server.c
./tcp_server 8888

# 客户端 (可准备多个客户端机的虚拟机)
$ gcc -o tcp_client tcp_client.c
./tcp_client (ip of server) 8888
```

