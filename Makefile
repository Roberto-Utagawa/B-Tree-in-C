MAIN = src/main.c
ITEM = src/registros/registry.c
UTIL = src/util/util.c
BTREE = src/b-tree/b-tree.c src/b-tree/b-tree-aux.c

BINARY = registros_de_alunos

all:
	gcc -Wall -g $(MAIN) $(ITEM) $(UTIL) $(BTREE) -o $(BINARY)

run:
	./$(BINARY)

valgrind:
	make clean
	valgrind --tool=memcheck --leak-check=full  --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BINARY) < infos/inp.in

README:
	@echo "Alunos:Roberto Severo Utagawa - 12690712 // Felipe Alvarenga Carvalho - 10310526"

1test:
	make all
	make clean
	clear
	./$(BINARY) < infos/inp.in

2test:
	make all
	make clean
	clear
	./$(BINARY) < infos/inp1.inp

clean:
	rm -fr data/btree.data data/dataFile.data

zip:
	rm -fr registros.zip
	zip -r registros.zip Makefile src/ data/