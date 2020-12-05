# Learning_Project_Nodes

## count_word

一个统计文本中单词有多少个的小脚本

更新了新的功能 - 计算每个单词一共出现多少次(并没有完全做对比，单词太多了。)

`gcc -o count count.c`

`./count a.txt`

## contacts

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

## lock

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

## pthread_pool

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

## MySQL

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

### 作业题：实现 mysql 数据库的连接池

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



![image-20201205160356724](C:\Users\HS\AppData\Roaming\Typora\typora-user-images\image-20201205160356724.png)

# DNS-UDP

## Queries

**查询名：**长度不固定，且不使用填充字节，一般该字段表示的就是需要查询的域名(如果是反向查询，则为IP，反向查询即由IP地址反查域名)，一般的格式如下图所示

![image-20201205160814992](C:\Users\HS\AppData\Roaming\Typora\typora-user-images\image-20201205160814992.png)

0voice.com;  **name:**60voice3com

www.0voice.com; **name:**3www60voice3com



在网络中，就是将 `dns_head` 和 `dns_question` 两个结构体内的信息发送给dns服务器

![image-20201205163059197](C:\Users\HS\AppData\Roaming\Typora\typora-user-images\image-20201205163059197.png)

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