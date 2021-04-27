OBJS_DIR = .objs
OBJS =  $(OBJS_DIR)/address_t.o $(OBJS_DIR)/reader_t.o $(OBJS_DIR)/validate_t.o $(OBJS_DIR)/tlb_t.o $(OBJS_DIR)/mmu_t.o

COMPILER = clang++
CFLAGS = -Wall -g -Wc++17-extensions -std=c++17
OUTPUT = vroom

all: $(OBJS) main.cpp
	$(COMPILER) $(CFLAGS) main.cpp $(OBJS) -o $(OUTPUT)
	./$(OUTPUT)

$(OBJS_DIR)/address_t.o: src/address_t.cpp includes/address_t.hpp
	$(COMPILER) $(CFLAGS) -c src/address_t.cpp -o $(OBJS_DIR)/address_t.o

$(OBJS_DIR)/mmu_t.o: src/mmu_t.cpp includes/mmu_t.hpp
	$(COMPILER) $(CFLAGS) -c src/mmu_t.cpp -o $(OBJS_DIR)/mmu_t.o

$(OBJS_DIR)/reader_t.o: src/reader_t.cpp includes/reader_t.hpp
	$(COMPILER) $(CFLAGS) -c src/reader_t.cpp -o $(OBJS_DIR)/reader_t.o

$(OBJS_DIR)/tlb_t.o: src/tlb_t.cpp includes/tlb_t.hpp
	$(COMPILER) $(CFLAGS) -c src/tlb_t.cpp -o $(OBJS_DIR)/tlb_t.o

$(OBJS_DIR)/validate_t.o: src/validate_t.cpp includes/validate_t.hpp
	$(COMPILER) $(CFLAGS) -c src/validate_t.cpp -o $(OBJS_DIR)/validate_t.o

clean:
	rm -rf $(OBJS_DIR)/* $(OUTPUT)

