#include <inttypes.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int f1();
int (*hot_f1)(void) = f1;

#define CMD_LEN 500
#define PAGE_SIZE 4096

char *get_code(char *file, char *fun) {
  char cmd[CMD_LEN];
  snprintf(cmd, CMD_LEN,
           "objdump --disassemble=%s %s | grep ' *[0-9a-f]:' | sed 's/ "
           "*[0-9a-f]*:\\t//' | sed 's/   *.*//' | tr -d '\\n' | tr -d ' '",
           fun, file);

  FILE *pipe = popen(cmd, "r");
  if (!pipe) {
    return NULL;
  }

  char *line = NULL;
  size_t len = 0;
  getline(&line, &len, pipe);
  pclose(pipe);

  return line;
}

void char_to_bytes(size_t h_len, char hex[h_len], size_t b_len,
                   uint8_t bytes[b_len]) {
  int i;
  // Loop through the string 2 chars at a time
  for (i = 0; i < h_len; i += 2) {
    // Convert 2 hex chars to int and store in array
    sscanf(&hex[i], "%2hhx", &bytes[i / 2]);
  }
}

// Signal handler function
void handle_sigint(int sig) {
  puts("Caught SIGINT!");
  hot_f1 = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  char *hex = get_code("fun.o", "f1");
  size_t h_len = strlen(hex);
  char_to_bytes(h_len, hex, PAGE_SIZE, (uint8_t *)hot_f1);
  // exit(0);
}

int main() {
  struct sigaction sa;

  sa.sa_handler = handle_sigint;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);

  while (1) {
    printf("Version of f1 = %d\n", hot_f1());
    sleep(1);
  }

  return 0;
}
