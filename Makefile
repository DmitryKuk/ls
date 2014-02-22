# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Исходники C
SRCS_C=main.c arguments.c error.c ls.c usage.c


TARGET=ls
PREFIX=/urs/local/bin/

# Объектные файлы
OBJS=$(SRCS_C:.c=.o)

# Компиляторы
GCC=gcc -Wall


# Цели

.PHONY: all clear #install uninstall

all: $(TARGET)

clear:
	rm -f "$(TARGET)" *.o

#install:
#	install $(TARGET) $(PREFIX)

#uninstall:
#	rm -f $(PREFIX)/$(TARGET)

# Конечная цель
$(TARGET): $(OBJS)
	$(GCC) -o $@ $^

# Неявные преобразования
%.o: %.c
	$(GCC) -o $@ -c $<
