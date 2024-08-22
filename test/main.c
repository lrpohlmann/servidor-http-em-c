#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  glob_t test_files;
  int glob_status = glob("test/test_*.out", GLOB_ERR, NULL, &test_files);
  if (glob_status != 0) {
    switch (glob_status) {
    case GLOB_NOSPACE:
      printf("Glob: sem memoria.\n");
      return EXIT_FAILURE;
    case GLOB_ABORTED:
      printf("Glob: erro leitura.\n");
      return EXIT_FAILURE;
    case GLOB_NOMATCH:
      printf("Zero testes encontrados.\n");
      return EXIT_FAILURE;
    }
  }

  for (int i = 0; i < test_files.gl_pathc; i++) {
    printf("Teste %s\n", test_files.gl_pathv[i]);
    pid_t new_process_id = fork();
    if (new_process_id == -1) {
      printf("Erro: fork.\n");
      return EXIT_FAILURE;
    } else if (new_process_id == 0) {
      int status_exec =
          execl(test_files.gl_pathv[i], test_files.gl_pathv[i], (char *)NULL);
      if (status_exec == -1) {
        perror("exec");
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }
  }

  for (int i = 0; i < test_files.gl_pathc; i++) {
    pid_t finalized_test = wait(NULL);
    if (finalized_test == -1) {
      perror("wait");
      return EXIT_FAILURE;
    }
  }

  globfree(&test_files);
  return EXIT_SUCCESS;
}
