if g++ src/drone.cpp main.cpp -o main.out functions/functions.cpp -lsfml-graphics -lsfml-window -lsfml-system; then
	./main.out "$@"
fi
