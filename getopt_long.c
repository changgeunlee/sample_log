#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

const char *program_name;
int verbose = 0;

static void print_usage(FILE *stream, int exit_code)
{
  fprintf(stream, "Usage : %s -c [ ... ]n", program_name);
  fprintf(stream,
    "?? -h????????????? --help?????? Display this usage information.n"
    "?? -p [passwords]? --password?? Passwords. (length: 8-16)n"
    "?? -v????????????? --verbose??? Print verbose message.n"
  );

  exit(exit_code);
}

int main(int argc, char **argv)
{
  int next_option;
  char *password;

  const char *const short_options = "hp:v";
  const struct option long_options[] = {
    { "help",0,NULL,'h' },
    { "password",1,NULL,'p' },
    { "verbose",0,NULL,'v' },
    { NULL,0,NULL,0 }
  };

  program_name = argv[0];

  do {
    next_option = getopt_long(argc, argv, short_options, long_options, NULL);

    switch(next_option)
    {
      case 'h':
        print_usage(stdout, 0);
        break;

      case 'p':
        password = strdup(optarg);
        break;

      case 'v':
        verbose = 1;
        break;

      case '?':    /* 잘못된 옵션 */
        print_usage(stderr, 1);
        break;

      case -1:    /* 더이상 옵션이 없음 정상 종료 */
        break;

      default:    /* 예상치 못한 오류 */
        abort();
    }
  } while (next_option != -1);

  return 0;
}
