#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PATH 260

struct source {
	char *name;
	FILE *file;
	struct source *next;
};

int main(int argc, char **argv)
{
	struct source *s = 0;
	FILE *out = 0;

	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case '\0':
				break;
			case 'n':
				break;
			case 'i':
				break;
			case 'o':
				out = fopen(argv[++i], "w");
				if (!out) printf("Cannot open %s, %s.\n", argv[i], strerror(errno));
				break;
			default:
				printf("Unknown option %s.\n", argv[i]);
				break;
			}
		} else {
			struct source *ns = malloc(sizeof(struct source));

			ns->name = argv[i];
			ns->file = fopen(ns->name, "rb");
			
			if (ns->file) {
				for (char *n = ns->name; *n; ++n) {
					switch (*n) {
					case '.':
						*n = '\0';
						*(n + 1) = '\0';
						break;
					case '\\':
					case '/':
						ns->name = n + 1;
						break;
					}
				}
				ns->next = s;
				s = ns;
			} else {
				printf("Cannot open %s, %s.\n", ns->name, strerror(errno));
				free(ns);
			}
		}
	}

	if (!s) {
		printf("No source specficied.\n");
		return -1;
	}

	if (!out) {
		char name_out[MAX_PATH];
		size_t l = strlen(s->name);
		memcpy(name_out, s->name, l);
		memcpy(name_out + l, ".h", 3);

		out = fopen(name_out, "w");

		if (!out) {
			printf("Cannot open %s, %s.\n", name_out, strerror(errno));
			return -1;
		}
	}

	fprintf(out, "%s", "#pragma once\n");

	char v;
	while (s) {
		fprintf(out, "%s", "const char g_");
		fprintf(out, "%s", s->name);
		fprintf(out, "%s", "[]={");
		while (fread(&v, 1, 1, s->file))
			fprintf(out, "%d,", v);
		fprintf(out, "%s", "};");
		s = s->next;
	}

    return 0;
}
