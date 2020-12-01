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

# pthread

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

>  作业题：实现 mysql 数据库的连接池