char dotenv[256];

char* getdotenv(const char* varname)
{
    FILE* fenv = fopen(dotenv, "r");
    if (!fenv) return NULL;

    char buffer[256];

    while (fgets(buffer, sizeof(buffer), fenv))
    {
        buffer[strcspn(buffer, "\n")] = 0;

        char* eq = strchr(buffer, '=');
        if (!eq) continue;

        *eq = 0;
        char* key = buffer;
        char* value = eq + 1;

        rmlastspaces(key);
        while (*value == ' ')
            value++;

        if (strcmp(varname, key) == 0) {
            char* result = strdup(value);
            fclose(fenv);
            rmlastspaces(result);
            return result;
        }
    }

    fclose(fenv);
    return NULL;
}
