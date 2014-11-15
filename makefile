
all:
	g++ -g src/*.cxx   -fsanitize=address -fno-omit-frame-pointer -lpthread -lasound -lsndfile
