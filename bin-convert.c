#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[]) {
    const char *in_path;
    FILE *in_file;
    FILE *out_file;
    const char *nmspace = "GL";
    const char *filename;
    char variable[1024];
    size_t i;
    int num_bytes = 0;
    int column = 0;
    const int bytes_per_column = 12;
    
	if (argc != 3) {
		fprintf(stderr, "Invalid args\n");
		return 1;
	}
	
    in_path = argv[1];
	in_file = fopen(in_path, "rb");
	if (!in_file) {
		fprintf(stderr, "Can't open input file %s\n", in_path);
		return 1;
	}
	
	out_file = fopen(argv[2], "wb");
	if (!out_file) {
		fprintf(stderr, "Can't open output file %s\n", argv[2]);
		fclose(in_file);
		return 1;
	}
	
	filename = strrchr(in_path, '/');
	if (filename) {
        memcpy(variable, filename + 1, strlen(filename + 1) + 1);
	} else {
        memcpy(variable, in_path, strlen(in_path) + 1);
	}
	for (i = 0; i < strlen(variable); ++i) {
		if (variable[i] == '.') {
			variable[i] = '_';
		}
	}
	
	fprintf(out_file, "#include \"GLResources.h\"\n");
	fprintf(out_file, "unsigned char %s::%s[] = {\n", nmspace, variable);
	
	for (;;) {
		int ch = fgetc(in_file);
		if (ch == EOF) {
			if (feof(in_file)) {
				fprintf(out_file, "\n");
			}
			break;
		}
		if (column == 0) {
			fprintf(out_file, "  ");
		}
		fprintf(out_file, "0x%02x, ", ch);
		++column;
		if (column == bytes_per_column) {
			column = 0;
			fprintf(out_file, "\n");
		}
		++num_bytes;
	}
	
	fprintf(out_file, "};\n");
	fprintf(out_file, "unsigned int %s::%s_len = %d;\n", nmspace, variable, num_bytes);
	
	fclose(out_file);
	fclose(in_file);
	
	return 0;
}
