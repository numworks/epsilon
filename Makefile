OBJS = tree_pool.o tree_node.o tree_reference.o test.o
CXXFLAGS = -std=c++11 -g -O0

test: $(OBJS)
	clang++ $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(OBJS)

%.o: %.cpp
	clang++ $(CXXFLAGS) -c $< -o $@

