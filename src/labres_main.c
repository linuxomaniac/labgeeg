int yyerror(const char *mess) {}

int main(int argc, char *argv[]) {
  FILE *f = NULL;
  int r;

  if(argc == 2) {
    if(strcmp(argv[1], "-") != 0) {
      f = fopen(argv[1], "r");
      if(f == NULL) {
        fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
        exit(1);
      }
      yyin = f;
    }
  } else if(argc > 2) {
    fprintf(stderr, "Usage: %s [<if>]\n", argv[0]);
    exit(1);
  }

  r = yyparse();

  if(f) {
    fclose(f);
  }

  if(!r) {
    fputs("gagné\n", stdout);
  } else {
    fputs("perdu\n", stderr);
  }

	return r;
}
