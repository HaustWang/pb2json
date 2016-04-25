CXX = g++ -std=c++11

INCLUDE = -I./ -I../

BIN = test_main

# define obj
BIN_OBJ = $(patsubst %.cpp, $(DEP_DIR)/%.o, $(wildcard *.cpp))

# define dep dirs
DEP_DIR := ./objdep
$(shell mkdir -p $(DEP_DIR))

RM = rm -rf
# define targets

# main targets
all: $(BIN)

# default rules
$(DEP_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $(INC_FLAGS) -c -o $@ $<

$(DEP_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c -o $@ $<

$(DEP_DIR)/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c -o $@ $<

$(DEP_DIR)/%.d : %.c
	@$(CC) -MM $(CFLAGS) $(INC_FLAGS) $<    | sed 's,\($*\)\.o[ :]*,$(DEP_DIR)/\1.o $@ : ,g' >$@

$(DEP_DIR)/%.d : %.cpp
	@$(CXX) -MM $(CXXFLAGS) $(INC_FLAGS) $< | sed 's,\($*\)\.o[ :]*,$(DEP_DIR)/\1.o $@ : ,g' >$@

$(DEP_DIR)/%.d : %.cc
	@$(CXX) -MM $(CXXFLAGS) $(INC_FLAGS) $< | sed 's,\($*\)\.o[ :]*,$(DEP_DIR)/\1.o $@ : ,g' >$@

$(BIN): $(BIN_OBJ) $(DEP_LIB)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	$(RM) $(BIN) $(DEP_DIR)

.PHONY: all clean

# default dependency rule



