CXX := g++
PYTHON := python

CXXFLAGS := -std=c++17 -O2

POINT_SRC := poisson_points.cpp
<<<<<<< HEAD
POINT_BIN := graph.exe
POINT_IMG := poisson_preview.png
POINT_ARGS := -n 120 -r 70 --seed 7

.PHONY: all points points_view clean
=======
DATA_MAKER_SRC := ../data_maker.cpp
DATA_MAKER_HDR := ../../ind/data_maker.hpp ../../ind/global.hpp
POINT_BIN := graph.exe
POINT_IMG := poisson_preview.png
SMALL_POINT_IMG := poisson_preview_small.png
LEVELS := 5
POINT_ARGS := -n 12000 -e 15000 --levels $(LEVELS)
SMALL_POINT_ARGS := -n 2000 -e 4000 --levels $(LEVELS)

.PHONY: all points points_view points_view_small clean
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d

all: points_view

points: $(POINT_BIN)

<<<<<<< HEAD
$(POINT_BIN): $(POINT_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

points_view: points
	./$(POINT_BIN) $(POINT_ARGS) | $(PYTHON) poisson_preview.py --stdin --radius 70 --save $(POINT_IMG) --no-show

clean:
	rm -f $(POINT_BIN) $(POINT_IMG)
=======
$(POINT_BIN): $(POINT_SRC) $(DATA_MAKER_SRC) $(DATA_MAKER_HDR)
	$(CXX) $(CXXFLAGS) -I../../ind $(POINT_SRC) $(DATA_MAKER_SRC) -o $@

points_view: points
	./$(POINT_BIN) $(POINT_ARGS) | $(PYTHON) poisson_preview.py --stdin --save $(POINT_IMG) --no-show

points_view_small: points
	./$(POINT_BIN) $(SMALL_POINT_ARGS) | $(PYTHON) poisson_preview.py --stdin --save $(SMALL_POINT_IMG) --no-show

clean:
	rm -f $(POINT_BIN) $(POINT_IMG) $(SMALL_POINT_IMG)
>>>>>>> c7c9cb91529b0d0f11b58dacd4347cf0784c893d
	rm -rf __pycache__
