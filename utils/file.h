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

char* fvalue(char* filename)
{
    FILE* fsrc = fopen(filename, "rb");
    fseek(fsrc, 0, SEEK_END);
    long size = ftell(fsrc);
    rewind(fsrc);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, fsrc);
    buffer[size] = '\0';
    fclose(fsrc);

    return buffer;
}

char* fformat(char* filename)
{
    char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return "";
    }
    return dot + 1;
}