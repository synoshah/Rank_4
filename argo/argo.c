/* ------------------------------------------------------------------------- */
/*                     SIMPLEST VERSION OF YOUR CODE                         */
/* ------------------------------------------------------------------------- */

int parse_string(char **dst, FILE *stream)
{
    int     c;
    int     len;
    char    *str;
    char    *tmp;

    if (!expect(stream, '"'))
        return (-1);
    
    len = 0;
    str = malloc(1); /* Start with 1 byte */
    str[0] = '\0';
    
    while (1)
    {
        c = getc(stream);
        if (c == EOF)
        {
            free(str);
            unexpected(stream);
            return (-1);
        }
        if (c == '"')
            break ;
        if (c == '\\') /* Handle escape characters */
        {
            c = getc(stream);
            if (c == EOF)
            {
                free(str);
                unexpected(stream);
                return (-1);
            }
        }
        
        /* Expand the string by exactly 1 extra character */
        tmp = realloc(str, len + 2);
        if (!tmp)
        {
            free(str);
            return (-1);
        }
        str = tmp;
        
        str[len] = c;
        len++;
        str[len] = '\0';
    }
    *dst = str;
    return (1);
}

int parse_integer(int *dst, FILE *stream)
{
    int c;

    c = peek(stream);
    /* Make sure it's actually a number or negative sign */
    if (c != '-' && !isdigit(c))
    {
        unexpected(stream);
        return (-1);
    }
    if (fscanf(stream, "%d", dst) != 1)
        return (-1);
    return (1);
}

int parse_map(json *dst, FILE *stream)
{
    pair    *tmp;
    pair    *p; /* Shortcut pointer to save typing! */

    if (!expect(stream, '{'))
        return (-1);
        
    if (accept(stream, '}'))
        return (1);
        
    while (1)
    {
        /* Expand the array for one more key-value pair */
        tmp = realloc(dst->map.data, sizeof(pair) * (dst->map.size + 1));
        if (!tmp)
            return (-1);
        dst->map.data = tmp;

        /* Use the shortcut 'p' instead of writing out the massive array path */
        p = &dst->map.data[dst->map.size];
        
        p->key = NULL;
        p->value.type = INTEGER;

        /* 1. Parse the Key */
        if (parse_string(&p->key, stream) != 1)
            return (-1);

        /* 2. Expect the Colon */
        if (!expect(stream, ':'))
        {
            dst->map.size++;
            return (-1);
        }

        /* Update size before parsing value for leak-free cleanup */
        dst->map.size++;

        /* 3. Parse the Value */
        if (argo(&p->value, stream) != 1)
            return (-1);

        /* 4. Check if map is done or if there are more pairs */
        if (accept(stream, '}'))
            break ;
            
        if (!expect(stream, ','))
            return (-1);
    }
    return (1);
}

int argo(json *dst, FILE *stream)
{
    int c;

    dst->type = INTEGER; 
    c = peek(stream);
    
    if (c == '{')
    {
        dst->type = MAP;
        dst->map.data = NULL;
        dst->map.size = 0;
        return (parse_map(dst, stream));
    }
    if (c == '"')
    {
        dst->type = STRING;
        dst->string = NULL;
        return (parse_string(&dst->string, stream));
    }
    if (c == '-' || isdigit(c))
    {
        dst->type = INTEGER;
        return (parse_integer(&dst->integer, stream));
    }
    
    unexpected(stream);
    return (-1);
}

/* ------------------------------------------------------------------------- */
/*                         END OF YOUR PRACTICE CODE                         */
/* ------------------------------------------------------------------------- */