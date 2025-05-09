#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("please provide executable to run as argument: %s <path-to-executable>\n", argv[0]);
    return 1;
  }

  // set TERM
  putenv("TERM=vt100-patched");

  int fd = open("/dev/myuart", O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  // duplicate the fd onto stdin and stdout
  dup2(fd, STDIN_FILENO);
  dup2(fd, STDOUT_FILENO);
  dup2(fd, STDERR_FILENO);

  // exec the program
  execvp(argv[1], &argv[1]);
  perror("execvp failed");
  return 1;
}
