APP=gen-expr



$(APP): gen-expr.c
	gcc -O2 -Wall -Werror -g -I../../include -I../../src/engine/interpreter -I../../src/monitor/debug -o $@ $< 

.PHONY: clean
clean:
	-rm $(APP) 2> /dev/null
