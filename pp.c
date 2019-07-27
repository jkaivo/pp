#define _XOPEN_SOURCE 700

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s source destinations\n", argv[0]);
		return 1;
	}

	int source = open(argv[1], O_RDONLY);
	if (source == -1) {
		fprintf(stderr, "%s:%s\n", argv[1], strerror(errno));
		return 1;
	}

	int dest = open(argv[2], O_WRONLY | O_CREAT, 0644);
	if (dest == -1) {
		fprintf(stderr, "%s:%s\n", argv[2], strerror(errno));
		return 1;
	}

	struct stat st;
	if (fstat(source, &st) == -1) {
		perror("stat");
		return 1;
	}

	setvbuf(stdout, NULL, _IONBF, 0);
	unsigned long long i = 0;
	do {
		char buf[BUFSIZ];

		ssize_t n = read(source, buf, sizeof(buf));
		if (n < 0) {
			perror("read");
			return 1;
		}

		if (write(dest, buf, n) < n) {
			perror("write");
			return 1;
		}

		
		i += n;
		printf("\r%llu/%llu bytes (%llu%%)", i, (unsigned long long)st.st_size, i * 100 / (unsigned long long)st.st_size);
	} while (i < (unsigned long long)st.st_size);

	printf("\n");
	return 0;
}
