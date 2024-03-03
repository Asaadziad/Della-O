main: main.s
	cc main.s -o main
main.s: main.ssa
	./qbe/qbe main.ssa -o main.s
main.ssa: A
	./bin/A
A:
	mkdir -p bin
	clang++  src/*.cpp -o bin/A  -D DEBUG_FLAG

clean:
	rm -f main.s main.ssa
