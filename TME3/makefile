.PHONY: clean

TME = tme2
DIR_SRC = src
DIR_OBJ = bin
TARGET = ${DIR_OBJ}/bin_c

All: $(DIR_OBJ) $(TARGET)

$(DIR_OBJ):
	mkdir bin

$(TARGET): ${DIR_OBJ}/$(TME).o 
	gcc -o $@ $^
	
$(DIR_OBJ)/$(TME).o: ${DIR_SRC}/$(TME).c
	gcc -o $@ -c $^

runSandmark: $(TARGET)
	./$(TARGET) data/sandmark.umz > outSandmark;
	more outSandmark
runCodex: $(TARGET)
	./$(TARGET) data/codex.umz > outCodex;
	more outCodex
clean:
	rm -rf $(DIR_OBJ)