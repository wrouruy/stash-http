size_t strnindex(const char *hay, size_t hlen,
                 const char *needle, size_t nlen,
                 bool return_end)
{
    if (!hay || !needle) return (size_t)-1;
    if (nlen == 0) return 0;
    if (nlen > hlen) return (size_t)-1;

    for (size_t i = 0; i <= hlen - nlen; i++) {
        if (memcmp(hay + i, needle, nlen) == 0) {
            size_t idx = i;
            if (return_end)
                idx += nlen;
            return idx;
        }
    }
    return (size_t)-1;
}

void urldecode(char *dst, const char *src) {
    char a, b;
    char *start = dst;

    while (*src) {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a = a - 'A' + 10; else a -= '0';
            if (b >= 'a') b -= 'a' - 'A';
            if (b >= 'A') b = b - 'A' + 10; else b -= '0';
            *dst++ = 16 * a + b;
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';

    size_t len = strlen(start);
    while (len > 1 && start[len - 1] == '/') {
        start[--len] = '\0';
    }
}

char* strbefore(char* input, char c)
{
    char* res = malloc(512);

    for(int i = 0; i < strlen(input); i++){
        if(input[i] == c)
            break;
        res[i] = input[i];
    }

    return res;
}

void rmlastspaces(char* input)
{
    int len = strlen(input);

    for (int i = len - 1; i >= 0; i--) {
        if (input[i] != ' ')
            break;
        input[i] = '\0';
    }
}