typedef struct {
    char* data;
    size_t size;
} FileData;

FileData read_file_binary(const char* path) {
    FileData fd = {0};

    FILE* f = fopen(path, "rb");
    if (!f) return fd;

    fseek(f, 0, SEEK_END);
    fd.size = ftell(f);
    fseek(f, 0, SEEK_SET);

    fd.data = malloc(fd.size);
    fread(fd.data, 1, fd.size, f);

    fclose(f);
    return fd;
}

char* fvalue(char* filepath)
{
    FILE* fsrc = fopen(filepath, "rb");
    fseek(fsrc, 0, SEEK_END);
    long size = ftell(fsrc);
    rewind(fsrc);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, fsrc);
    buffer[size] = '\0';
    fclose(fsrc);

    return buffer;
}

bool isFile(char* filename){
    struct stat st;
    if(stat(filename, &st) == 0){
        if (S_ISREG(st.st_mode)) {
            return true;
        }
        return false;
    }
    return false;
}

char* fformat(char* filename)
{
    char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return "";
    }
    return dot + 1;
}