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

  printf("%lu Testes Encontrados\n", test_files.gl_pathc);
  int ok_tests = 0;
  int fail_tests = 0;

  for (int i = 0; i < test_files.gl_pathc; i++) {
    printf("%s ... \n", test_files.gl_pathv[i]);
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
    } else {
      int test_exit_status = 0;
      int wait_return = waitpid(new_process_id, &test_exit_status, 0);
      if (wait_return == -1) {
        perror("wait");
        return EXIT_FAILURE;
      }

      if (WIFEXITED(test_exit_status)) {
        printf("\e[1;32mOK\e[0m\n");
        ok_tests++;
      } else {
        printf("\e[1;31mFAIL\e[0m\n");
        fail_tests++;
      }

      printf("\n");
    }
  }

  printf("Ok: %d, Failed: %d\n", ok_tests, fail_tests);
  globfree(&test_files);
  return EXIT_SUCCESS;
}
