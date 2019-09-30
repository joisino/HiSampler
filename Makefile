CXX = g++
CXXFLAGS = -std=gnu++11 -O2
.PHONY: all clean

all: solvers/clique solvers/coloring solvers/vertex_cover solvers/vertex_cover_approx

solvers/clique: solvers/src/clique.cpp
	$(CXX) $(CXXFLAGS) solvers/src/clique.cpp -o solvers/clique

solvers/coloring: solvers/src/coloring.cpp
	$(CXX) $(CXXFLAGS) solvers/src/coloring.cpp -o solvers/coloring

solvers/vertex_cover: solvers/src/vertex_cover.cpp
	$(CXX) $(CXXFLAGS) solvers/src/vertex_cover.cpp -o solvers/vertex_cover

solvers/vertex_cover_approx: solvers/src/vertex_cover_approx.cpp
	$(CXX) $(CXXFLAGS) solvers/src/vertex_cover_approx.cpp -o solvers/vertex_cover_approx

clean:
	rm -r solvers
	mkdir solvers
	git checkout solvers
