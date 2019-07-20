#编译器
CC=gcc
#预处理函数
CPPFLAGS=-I /usr/include/fastdfs \
				 -I /usr/include/fastcommon 

CFLAGS=-Wall

#动态库
LIBS=-lfdfsclient

#子目标
upload_fdfs=./upload

#最终目标
target=$(upload_fdfs)
ALL:$(target)

#所有.c生成.o
%.o:%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS)

#生成每一个子目标
$(upload_fdfs): ./fdfs_api.o ./make_log.o ./main_test.o
	$(CC) $^ $(LIBS) -o $@

#清理
clean:
	-rm -rf *.o $(target)

#伪目标
.PHONY:clean ALL
