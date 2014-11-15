
all:
	g++ -g src/*.cxx -std=c++11  -fsanitize=address -fno-omit-frame-pointer -lpthread -lasound -lsndfile
