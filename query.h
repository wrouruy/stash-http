bool include_query(HTTPreq* req, char* queryname)
{
    const char* queries = req->queries;
    char q_eq[strlen(queryname) + 2];
    snprintf(q_eq, sizeof(q_eq), "%s=", queryname);

    size_t digit = strnindex(queries, sizeof(queries), q_eq, strlen(q_eq), true);

    return digit != -1 && queries[3] != '\0';
}

char* get_query(const HTTPreq* req, const char* queryname)
{
    if (!req || !req->queries || !queryname) {
        return NULL;
    }

    const char* queries = req->queries;
    size_t qlen = strlen(queries);
    if (qlen == 0) {
        return NULL;
    }

    char q_eq[strlen(queryname) + 2];
    snprintf(q_eq, sizeof(q_eq), "%s=", queryname);

    size_t pos = strnindex(queries, qlen, q_eq, strlen(q_eq), true);
    if (pos == (size_t)-1 || pos >= qlen)
        return NULL;

    char* res = malloc(qlen - pos + 1);
    if (!res) return NULL;

    size_t j = 0;
    while (queries[pos] != '&' && queries[pos] != '\0') {
        res[j++] = queries[pos++];
    }

    res[j] = '\0';
    return res;
}