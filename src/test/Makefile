CXX := g++
PYTHON := python

CXXFLAGS := -std=c++17 -O2

POINT_SRC := poisson_points.cpp
DATA_MAKER_SRC := ../data_maker.cpp
DATA_MAKER_HDR := ../../ind/data_maker.hpp ../../ind/global.hpp
POINT_BIN := graph.exe
POINT_IMG := poisson_preview.png
LEVELS := 5
POINT_ARGS := -n 1200 --levels $(LEVELS)

.PHONY: all points points_view clean

all: points_view

points: $(POINT_BIN)

$(POINT_BIN): $(POINT_SRC) $(DATA_MAKER_SRC) $(DATA_MAKER_HDR)
	$(CXX) $(CXXFLAGS) -I../../ind $(POINT_SRC) $(DATA_MAKER_SRC) -o $@

points_view: points
	./$(POINT_BIN) $(POINT_ARGS) | $(PYTHON) poisson_preview.py --stdin --save $(POINT_IMG) --no-show

clean:
	rm -f $(POINT_BIN) $(POINT_IMG)
	rm -rf __pycache__
