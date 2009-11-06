.SUFFIXES: .o .c .cpp 

all: bst btree
default: bst

OBJ =  filereader.o str.o main.o 
 
BTREE_OBJ = ${OBJ} btree.o
BST_OBJ = ${OBJ} bst.o
	
%.o: %.c 
	gcc -Wall -Wno-unused-function -Wno-deprecated -O3 -c  $(CFLAGS) $(DEFS) $< -o $@
	
%.o: %.cpp 
	gcc -Wall -Wno-unused-function -Wno-deprecated -O3 -c  $(CFLAGS) $(DEFS) $< -o $@

bst: ${BST_OBJ}
	gcc -o $@ ${BST_OBJ} $(DEFS) -lstdc++

btree: ${BTREE_OBJ}
	gcc -o $@ ${BTREE_OBJ} $(DEFS) -lstdc++
	
clean:
	-rm *.o
	-rm bst.exe btree.exe
	-rm bst btree
