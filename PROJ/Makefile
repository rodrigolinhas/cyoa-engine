CC = gcc
CFLAGS = -Wall -Wextra
DEBUG_FLAG = -DDEBUG_ON
CUNIT_LIB = -lcunit

SRC_DIR = main
TEST_DIR = tests

.PHONY: all run test debug clean

all: $(SRC_DIR)/TDfinal.exe $(TEST_DIR)/Test.exe

# Compila o programa principal
$(SRC_DIR)/TDfinal.exe: $(SRC_DIR)/TDfinal.c
	$(CC) $< $(CFLAGS) -o $@

# Compila os testes unitários
$(TEST_DIR)/Test.exe: $(TEST_DIR)/test.c
	$(CC) $< $(CUNIT_LIB) $(CFLAGS) -o $@

# Executa o programa principal
run: $(SRC_DIR)/TDfinal.exe
	@echo "Digite o nome do arquivo de input (ex: input.txt):"; \
	read filename; \
	./$< < $$filename

# Executa os testes unitários
test: $(TEST_DIR)/Test.exe
	@./$<

# Compila em modo debug
debug: $(SRC_DIR)/TDfinal.c
	$(CC) $< $(CFLAGS) $(DEBUG_FLAG) -o $(SRC_DIR)/TDfinal.exe
	@echo "Debug ativado."
	$(SRC_DIR)/TDfinal.exe
		@echo "Digite o nome do arquivo de input (ex: input.txt):"; \
		read filename; \
		./$< < $$filename


# Remove executáveis
clean:
	rm -f $(SRC_DIR)/*.exe $(TEST_DIR)/*.exe $(TEST_DIR)/tmp_*.txt