# use this makefile to build with platformio 
#
.PHONY: all clean upload monitor lint test ci

# some of the examples use LED_BUILTIN which is not defined for ESP32
CIOPTSALL=--board=esp32dev --board=uno --board=esp01 --lib="src"

all:
	pio run

lint:
	cpplint --extensions=cpp,h,ino $(shell find .  \( ! -regex '.*/\..*' \) \
		       -type f -a \( -name "*\.cpp" -o -name "*\.h" -o -name "*\.ino" \) )

ci:
	platformio ci $(CIOPTSALL) examples/lamps27/lamps27.ino

envdump:
	-pio run --target envdump

clean:
	-pio run --target clean
	rm -f {test,src}/{*.o,*.gcno,*.gcda}

upload:
	pio run --target upload 

monitor:
	pio device monitor 

test:
	$(MAKE) -C test coverage OPT=-O0

tags:
	ctags -R