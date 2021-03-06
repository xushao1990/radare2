/* radare2 - Copyleft 2011-2017 - pancake */

#include <r_util.h>
#include <r_socket.h>

static void fwd(int sig) {
	/* do nothing? send kill signal to remote process */
}

static void rarun2_tty() {
	/* TODO: Implement in native code */
	system ("tty");
	close(1);
	dup2(2, 1);
	signal (SIGINT, fwd);
	for (;;) {
		sleep (1);
	}
}

int main(int argc, char **argv) {
	char *file;
	RRunProfile *p;
	int i, ret;
	if (argc == 1 || !strcmp (argv[1], "-h")) {
		eprintf ("Usage: rarun2 -v|-t|script.rr2 [directive ..]\n");
		printf ("%s", r_run_help ());
		return 1;
	}
	if (!strcmp (argv[1], "-v")) {
		printf ("rarun2 "R2_VERSION"\n");
		return 0;
	}
	file = argv[1];
	if (!strcmp (file, "-t")) {
		rarun2_tty ();
		return 0;
	}
	if (*file && !strchr (file, '=')) {
		p = r_run_new (file);
	} else {
		bool noMoreDirectives = false;
		int directiveIndex = 0;
		p = r_run_new (NULL);
		for (i = *file ? 1 : 2; i < argc; i++) {
			if (!strcmp (argv[i], "--")) {
				noMoreDirectives = true;
				continue;
			}
			if (noMoreDirectives) {
				const char *word = argv[i];
				char *line = directiveIndex
					? r_str_newf ("arg%d=%s", directiveIndex, word)
					: r_str_newf ("program=%s", word);
				r_run_parseline (p, line);
				directiveIndex ++;
				free (line);
			} else {
				r_run_parseline (p, argv[i]);
			}
		}
	}
	if (!p) {
		return 1;
	}
	ret = r_run_config_env (p);
	if (ret) {
		printf("error while configuring the environment.\n");
		return 1;
	}
	ret = r_run_start (p);
	r_run_free (p);
	return ret;
}
