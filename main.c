// MIT License
//
// Copyright (c) 2024 Floréal Risso
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <inttypes.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define CMD_LEN 500
#define PAGE_SIZE 4096

const char *fun_name = "f1";
const char *file_name = "fun.o";
const char *cmd_template =
        "objdump --disassemble=%s %s |"
        "grep ' *[0-9a-f]:' | "
        "sed 's/ *[0-9a-f]*:\\t//' | sed 's/   *.*//' | tr -d '\\n' | tr -d ' '";


static int
f1(void)
{
	return 1;
}

static int (*hot_f1)(void) = f1;

static char*
get_code(const char* file, const char* fun)
{
	char cmd[CMD_LEN];
	snprintf(cmd, CMD_LEN, cmd_template,
	         fun, file);

	FILE* pipe = popen(cmd, "r");
	if (!pipe) {
		return NULL;
	}

	char* line = NULL;
	size_t len = 0;
	if (getline(&line, &len, pipe) == -1) {
		free(line);
		line = NULL;
	}

	pclose(pipe);
	return line;
}

static void
char_to_bytes(const char* hex, size_t h_len, uint8_t* bytes, size_t b_len)
{
	for (size_t i = 0; i < h_len && i / 2 < b_len; i += 2) {
		sscanf(&hex[i], "%2hhx", &bytes[i / 2]);
	}
}

static void
handle_sigint(int sig)
{
	(void)sig;  // Unused parameter
	puts("Caught SIGINT!");
	puts("Update!");

	void* new_func = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
	                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (new_func == MAP_FAILED) {
		perror("mmap failed");
		return;
	}

	char* hex = get_code(file_name, fun_name);
	if (!hex) {
		munmap(new_func, PAGE_SIZE);
		return;
	}

	size_t h_len = strlen(hex);
	char_to_bytes(hex, h_len, new_func, PAGE_SIZE);
	free(hex);

	hot_f1 = new_func;
}

int
main(void)
{
	struct sigaction sa = {
		.sa_handler = handle_sigint,
		.sa_flags = 0
	};
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction failed");
		return 1;
	}

	while (1) {
		printf("Version of f1 = %d\n", hot_f1());
		sleep(1);
	}

	return 0;
}
