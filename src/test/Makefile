CXX := g++
PYTHON := python

CXXFLAGS := -std=c++17 -O2

POINT_SRC := poisson_points.cpp
DATA_MAKER_SRC := ../data_maker.cpp
DATA_MAKER_HDR := ../../ind/data_maker.hpp ../../ind/global.hpp
POINT_BIN := graph.exe
POINT_IMG := poisson_preview.png
SMALL_POINT_IMG := poisson_preview_small.png
LEVELS := 5
POINT_ARGS := -n 12000 -e 15000 --levels $(LEVELS)
SMALL_POINT_ARGS := -n 2000 -e 4000 --levels $(LEVELS)

.PHONY: all points points_view points_view_small clean


all: points_view

points: $(POINT_BIN)

$(POINT_BIN): $(POINT_SRC) $(DATA_MAKER_SRC) $(DATA_MAKER_HDR)
	$(CXX) $(CXXFLAGS) -I../../ind $(POINT_SRC) $(DATA_MAKER_SRC) -o $@
$(POINT_BIN): $(POINT_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

points_view: points
	./$(POINT_BIN) $(POINT_ARGS) | $(PYTHON) poisson_preview.py --stdin --save $(POINT_IMG) --no-show

points_view_small: points
	./$(POINT_BIN) $(SMALL_POINT_ARGS) | $(PYTHON) poisson_preview.py --stdin --save $(SMALL_POINT_IMG) --no-show

clean:
	rm -f $(POINT_BIN) $(POINT_IMG) $(SMALL_POINT_IMG)
	rm -rf __pycache__
