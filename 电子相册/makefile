
#要生成的可执行文件名
TARGET :=exe

CC := arm-linux-gcc
#CC := gcc

#CSRC指定工程中所有的c源文件名列表
CSRCS := $(wildcard ./src/*.c) $(wildcard ./*.c) 

#OBJS指定工程中所有的.o文件列表
OBJS := $(patsubst %.c, %.o, $(CSRCS))

#INCS指定头文件的目录
INCS += -I./ -I./inc

#LIBS指定所需要依赖的库以及库的搜索路径
LIBS += -L../libs   -lm -lpthread

$(TARGET): $(OBJS)
	$(CC) $^ $(LIBS) -o $@
	cp ./$(TARGET) ~/tftpboot/$(TARGET)

	rm -rf ${OBJS}	

%.o: %.c
	$(CC) -c $<  $(INCS) -o $@
	
#"清除"：所有的中间文件和最终的可执行文件
clean:
	rm -rf $(OBJS)
	rm -rf $(TARGET)

