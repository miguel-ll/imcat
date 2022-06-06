default:
	gcc -O3 -DNDEBUG -Wall -Wextra -Os -std=c99 -Wno-unused-result -rdynamic -lm -o imcat imcat.c sh_image.c

install:
	gcc -O3 -DNDEBUG -Wall -Wextra -Os -std=c99 -Wno-unused-result -rdynamic -lm -o /usr/local/bin/imcat imcat.c sh_image.c

uninstall:
	rm /usr/local/bin/imcat
