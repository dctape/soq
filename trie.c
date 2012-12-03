/* SO 13674617/adding-word-to-trie-structure-dictionary */

/*
** I'm trying to create a trie structure which can be inserted words,
** but the struct has to be exactly like this:
*/

typedef struct tNode_t
{
    struct tNode_t **l;
    char            *w;
} tNode;

/*
** The member l is a pointer to an array of 27 pointers to tNodes;
** that's the part I don't understand.
**
** If the array is of pointers to tNodes, how do I insert words into it?
** And since the size of the array is 27 (26 lower case letters a-z and
** the terminating character) how do you know where to input the word
** depending on what the beginning letter is?
*/

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

extern void trie_add_word(tNode *tree, char const *word);
extern tNode *trie_find_word(tNode *tree, char const *word);
extern void trie_print(tNode *tree);
extern tNode *trie_new(void);
extern void trie_free(tNode *tree);

static void db_print(char const *fmt, ...);

tNode *trie_new(void)
{
    tNode *tree = malloc(sizeof(tNode));
    assert(tree != 0);      // Abysmal way to validate memory allocation
    tree->w = 0;
    tree->l = (tNode **)calloc(27, sizeof(tNode *));
    assert(tree->l != 0);   // Abysmal way to validate memory allocation
    return(tree);
}

void trie_free(tNode *tree)
{
    assert(tree != 0);
    assert(tree->l != 0);
    for (size_t i = 0; i < 27; i++)
    {
        if (tree->l[i] != 0)
            trie_free(tree->l[i]);
    }
    free(tree->l);
    free(tree->w);
    free(tree);
}

static void add_word_suffix(tNode *tree, char const *word, char const *suffix)
{
    int c;
    assert(tree != 0);
    assert(tree->l != 0);
    db_print("-->> %s: word [%s], suffix [%s]\n", __func__, word, suffix);
    while ((c = *suffix++) != '\0')
    {
        if (isalpha(c))
        {
            db_print("---- %s: letter %c (index %d)\n", __func__, c, c - 'a' + 1);
            c = tolower(c) - 'a' + 1;
            assert(tree->l != 0);
            if (tree->l[c] == 0)
                tree->l[c] = trie_new();
            db_print("---- %s: recurse: [%s]/[%s]\n", __func__, word, suffix);
            add_word_suffix(tree->l[c], word, suffix);
            db_print("<<-- %s\n", __func__);
            return;
        }
    }
    if (tree->w != 0)
    {
        db_print("---- %s: tree already contains word [%s] at [%s]\n", __func__, word, tree->w);
        return;
    }
    tree->w = strdup(word);
    db_print("<<-- %s: inserted word [%s]\n", __func__, tree->w);
}

void trie_add_word(tNode *tree, char const *word)
{
    add_word_suffix(tree, word, word);
}

static tNode *find_word_suffix(tNode *tree, char const *word, char const *suffix)
{
    int c;
    db_print("-->> %s: word [%s] suffix[%s]\n", __func__, word, suffix);
    for ( ; (c = *suffix) != '\0'; suffix++)
    {
        if (isalpha(c))
        {
            db_print("---- %s: letter %c\n", __func__, c);
            c = tolower(c) - 'a' + 1;
            if (tree->l[c] == 0)
                return(0);
            tNode *rv = find_word_suffix(tree->l[c], word, suffix+1);
            if (rv == 0)
            {
                db_print("<<-- %s: missing [%s]/[%s]\n", __func__, word, suffix);
                return 0;
            }
            db_print("<<-- %s: found [%s] for [%s]/[%s]\n", __func__, rv->w, word, suffix);
            return rv;
        }
    }
    if (tree->w == 0)
    {
        db_print("<<-- %s: missing [%s]/[%s]\n", __func__, word, suffix);
        return 0;
    }
    db_print("<<-- %s: found [%s] for [%s]/[%s]\n", __func__, tree->w, word, suffix);
    return(tree);
}

tNode *trie_find_word(tNode *tree, char const *word)
{
    return find_word_suffix(tree, word, word);
}

void trie_print(tNode *tree)
{
    assert(tree != 0);
    assert(tree->l != 0);
    if (tree->w != 0)
        printf("%s\n", tree->w);
    for (size_t i = 0; i < 27; i++)
    {
        if (tree->l[i] != 0)
            trie_print(tree->l[i]);
    }
}

static int debug = 0;

static void db_print(char const *fmt, ...)
{
    if (debug > 0)
    {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}

int main(int argc, char **argv)
{
    tNode *root = trie_new();

    /* Set debugging - and use argv */
    if (argc > 1 && argv[argc] == 0)
        debug = 1;

    /* First test */
    char const *word = "cab";
    trie_add_word(root, word);
    tNode *leaf = trie_find_word(root, word);
    printf("Leaf word = %s\n", leaf->w);
    trie_free(root);

    /* Second, more comprehensive test */
    static char const *words[] =
    {
        "cabal",         "cabbie",
        "cab",           "centre",
        "cinema",        "cold",
        "culminate",     "culmination",
        "duck",          "cabs",
        "amniocentesis", "amniocentesis",
        "amniocentesis", "cam",
        "cab",           "cab",
        "zulu",          "alpha",
        "bravo",         "Charlie",
        "delta",         "echo",
        "foxtrot",       "golf",
        "hotel",         "India",
        "Juliet",        "kilo",
        "Lima",          "Mike",
        "November",      "Oscar",
        "Papa",          "Quebec",
        "Romeo",         "Sierra",
        "Tango",         "uMBRelLA",
        "Victor",        "Whisky",
        "X-ray",         "Yankee",
        "Zulu",          "Aquarius",
    };
    size_t num_words = sizeof(words) / sizeof(words[0]);
    size_t counter = 0;

    /* First time, add every other word; second time, every word */
    for (size_t mod = 2; mod > 0; mod--)
    {
        root = trie_new();
        printf("\nTest %zu\n", ++counter);
        for (size_t i = 0; i < num_words; i++)
        {
            if (i % mod == 0)
                trie_add_word(root, words[i]);
            tNode *leaf = trie_find_word(root, words[i]);
            if (leaf == 0)
                printf("Word [%s] is missing\n", words[i]);
            else
                printf("Word [%s] for [%s]\n", leaf->w, words[i]);
        }
        printf("\nTrie:\n");
        trie_print(root);
        trie_free(root);
    }

    return(0);
}
