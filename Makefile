all:
	em++ puzzle_drop.cpp -o puzzle_drop.js -lembind
clean:
	rm puzzle_drop.wasm puzzle_drop.js
