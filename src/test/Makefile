CXX := g++
PYTHON := python

CXXFLAGS := -std=c++17 -O2

POINT_SRC := poisson_points.cpp
POINT_BIN := graph.exe
POINT_IMG := poisson_preview.png
POINT_ARGS := -n 120 -r 70 --seed 7

.PHONY: all points points_view clean

all: points_view

points: $(POINT_BIN)

$(POINT_BIN): $(POINT_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

points_view: points
	./$(POINT_BIN) $(POINT_ARGS) | $(PYTHON) poisson_preview.py --stdin --radius 70 --save $(POINT_IMG) --no-show

clean:
	rm -f $(POINT_BIN) $(POINT_IMG)
	rm -rf __pycache__
