src = 2.c
obj = 2.o 
opt = -g -c
all:	2
2: $(obj)
	gcc $(obj) -l pthread -o 2 -lm
2.o: $(src)
	gcc $(opt) $(src)
clean:
	rm 2 *.o
