// page 100 http://staff.ustc.edu.cn/~csli/graduate/datum/%cb%e3%b7%a8%b5%bc%c2%db%cf%b0%cc%e2%c2%d2%bd%e2%d6%d0%ce%c40410.pdf
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* D e f i n e c o m m a n d s t r i n g l i s t . */
#define INSERT "insert\n"
#define PRINT "print\n"
#define FIND "find\n"
#define DELETE "delete\n"
#define CLEAR "clear\n"
#define HELP "help\n"
#define QUIT "quit\n"
/* D e f i n e t h e m a x i m u m l i n e . */
#define MAXLINE 1024
typedef int Key;
typedef struct red_black_node *rbptr;
enum nodecolor {red, black};
typedef struct red_black_node
{
    Key key;
    rbptr rt;
    rbptr lft;
    rbptr pr;
    enum nodecolor color;
} rbnode;
static rbnode NIL = {0, NULL, NULL, NULL, black};
rbptr rb_build_tree(rbptr root);
rbptr minimum(rbptr x);
rbptr maximum(rbptr x);
rbptr predecessor(rbptr x);
rbptr successor(rbptr x);
rbptr rb_search(rbptr x, Key k);
rbptr left_rotate(rbptr root, rbptr x);
rbptr right_rotate(rbptr root, rbptr x);
rbptr rb_insert(rbptr root, rbptr z);
rbptr rb_insert_fixup(rbptr root, rbptr z);
rbptr rb_delete(rbptr root, rbptr z);
rbptr rb_delete_fixup(rbptr root, rbptr z);
rbptr rb_clear_tree(rbptr root);
void rb_print_node(rbptr p);
void rb_pre_visit_tree(rbptr root);
void usage(void);
int getline(char bfu[], int max);
rbptr rb_build_tree(rbptr root)
{
    int counter = 1;
    rbptr p;
    Key k;
    char arg[MAXLINE];
    printf(">(%d)Input key:", counter);
    while (getline(arg, MAXLINE) > 0)
    {
        if (sscanf(arg, "%d", &k) == 1)
        {
            if (p = (struct red_black_node*) malloc(sizeof(struct red_black_node)))
            {
                p->key = k;
                p->lft = &NIL;
                p->rt = &NIL;
                p->pr = &NIL;
                p->color = red;
                root = rb_insert(root, p);
                ++counter;
            }
            printf(">(%d)Input key:", counter);
        }
        else if (strcmp(arg, "\n") == 0)
            break;
        else
            printf("?Wrong key type.\n>(%d)Input key:", counter);
    }
    return root;
}
rbptr minimum(rbptr x)
{
    while (x->lft != &NIL)
        x = x->lft;
    return x;
}
rbptr maximum(rbptr x)
{
    while (x->rt != &NIL)
        x = x->rt;
    return x;
}
rbptr successor(rbptr x)
{
    if (x->rt != &NIL)
        return (minimum(x->rt));
    while (x->pr != &NIL && x == x->pr->rt)
        x = x->pr;
    return x;
}
rbptr predecessor(rbptr x)
{
    if (x->lft != &NIL)
        return (maximum(x));
    while (x->pr != &NIL && x == x->pr->lft)
        x = x->pr;
    return x;
}
rbptr rb_search(rbptr x, Key k)
{
    while (x != &NIL && x->key != k)
    {
        if (x->key < k)
            x = x->rt;
        else
            x = x->lft;
    }
    return x;
}
rbptr left_rotate(rbptr root, rbptr x)
{
    rbptr r = root;
    rbptr y;
    y = x->rt;
    x->rt = y->lft;
    y->lft->pr = x;
    y->pr = x->pr;
    if (x->pr == &NIL)
        r = y;
    else if (x == x->pr->lft)
        x->pr->lft = y;
    else
        x->pr->rt = y;
    y->lft = x;
    x->pr = y;
    return r;
}
rbptr right_rotate(rbptr root, rbptr x)
{
    rbptr r = root;
    rbptr y;
    y = x->lft;
    x->lft = y->rt;
    y->rt->pr = x;
    y->pr = x->pr;
    if (x->pr == &NIL)
        r = y;
    else
    {
        if (x->pr->lft == x)
            x->pr->lft = y;
        else
            x->pr->rt = y;
    }
    y->rt = x;
    x->pr = y;
    return r;
}
rbptr rb_insert(rbptr root, rbptr z)
{
    rbptr r = root;
    rbptr x, y;
    y = &NIL;
    x = root;
    while (x != &NIL)
    {
        y = x;
        if (z->key < x->key)
            x = x->lft;
        else
            x = x->rt;
    }
    z->pr = y;
    if (y == &NIL)
        r = z;
    else if (z->key < y->key)
        y->lft = z;
    else
        y->rt = z;
    z->lft = &NIL;
    z->rt = &NIL;
    z->color = red;
    r = rb_insert_fixup(r, z);
    return r;
}
rbptr rb_insert_fixup(rbptr root, rbptr z)
{
    rbptr r = root;
    rbptr y;
    while (z->pr->color == red)
    {
        if (z->pr == z->pr->pr->lft)
        { /* z ’ s p a r e n t is a l e f t c h i l d */
            y = z->pr->pr->rt;
            if (y->color == red)
            {                           /* C a s e 1 : z ’ s u n c l e y is r e d. */
                z->pr->color = black;   /* C a s e 1 */
                y->color = black;       /* C a s e 1 */
                z->pr->pr->color = red; /* C a s e 1 */
                z = z->pr->pr;          /* C a s e 1 */
            }
            else
            { /* C a s e 2 : z is a r i g h t c h i l d a n d z ’ s u n c l e y is b l a c
                 k . */
                if (z == z->pr->rt)
                {                          /* C a s e 2 */
                    z = z->pr;             /* C a s e 2 */
                    r = left_rotate(r, z); /* C a s e 2 */
                }                          /* C a s e 2 */
                z->pr->color = black; /* C a s e 3 : z is a l e f t c h i l d a n d z ’ s
                                         u n c l e y is r e d . */
                z->pr->pr->color = red;         /* C a s e 3 */
                r = right_rotate(r, z->pr->pr); /* C a s e 3 */
            }
        }
        else if (z->pr == z->pr->pr->rt)
        { /* z ’ s p a r e n t is a r i g h t c h i l d */
            y = z->pr->pr->lft;
            if (y->color == red)
            {                           /* C a s e 4 : z ’ s u n c l e is r e d */
                z->pr->color = black;   /* C a s e 4 */
                y->color = black;       /* C a s e 4 */
                z->pr->pr->color = red; /* C a s e 4 */
                z = z->pr->pr;          /* C a s e 4 */
            }
            else
            {
                if (z == z->pr->lft)
                { /* C a s e 5 : z ’ s u n c l e is b l a c k a n d z is a l e f t c h i l
                     d */
                    z = z->pr;              /* C a s e 5 */
                    r = right_rotate(r, z); /* C a s e 5 */
                }                           /* C a s e 5 */
                z->pr
                    ->color = black; /* C a s e 6 : z ’ s u n c l e is b l a c k a n d z
                                        is a r i g h t c h i l d */
                z->pr->pr->color = red;        /* C a s e 6 */
                r = left_rotate(r, z->pr->pr); /* C a s e 6 */
            }
        }
    }
    r->color = black;
    return r;
}

rbptr rb_delete(rbptr root, rbptr z)
{
    rbptr r = root;
    rbptr x, y;
    /* y is t h e n o d e to be d e l e t e d . */
    if (z->lft == &NIL || z->rt == &NIL)
        y = z;
    else
        y = successor(z);
    if (y->lft != &NIL)
        x = y->lft;
    else
        x = y->rt;
    x->pr = y->pr;
    if (y == r)
        r = x;
    else if (y == y->pr->lft)
        y->pr->lft = x;
    else
        y->pr->rt = x;
    if (y != z)
        z->key = y->key;
    if (y->color == black)
        r = rb_delete_fixup(r, x);
    free(y);
    return r;
}
rbptr rb_delete_fixup(rbptr root, rbptr z)
{
    rbptr r = root;
    rbptr w;
    if (z != r && z->color == black)
    {
        if (z == z->pr->lft)
        {                  /* z is a l e f t c h i l d . */
            w = z->pr->rt; /* w is z ’ s r i g h t s i b l i n g . */
            if (w->color == red)
            { /* C a s e 1 : w ’ s c o l o r is r e d . */
                w->color = black;
                z->pr->color = red;
                r = left_rotate(r, z->pr);
                w = z->pr->rt;
            }
            else if (w->lft->color == black && w->rt->color == black)
            { /* C a s e 2 : w ’ s c h i l d s a r e b l a c k . */
                w->color = red;
                z = z->pr;
            }
            else
            {
                if (w->rt->color == black)
                { /* C a s e 3 : w ’ s r i g h t c h i l d is b l a c k . */
                    w->lft->color = black;
                    w->color = red;
                    r = right_rotate(r, w);
                    w = z->pr->rt;
                }
                w->color =
                    z->pr->color; /* C a s e 4 : w ’ s r i g h t c h i l d is r e d . */
                w->rt->color = black;
                z->pr->color = black;
                r = left_rotate(r, z->pr);
                z = r;
            }
        }
        else
        {                   /* z is a r i g h t c h i l d . */
            w = z->pr->lft; /* w is z ’ s l e f t s i b l i n g . */
            if (w->color == red)
            { /* C a s e 5 : w ’ s c o l o r is r e d .*/
                w->color = black;
                z->pr->color = red;
                r = right_rotate(r, z->pr);
                w = z->pr->lft;
            }
            else if (w->lft->color == black && w->rt->color == black)
            { /* C a s e 6 : w ’ s c h i l d s a r e b l a c k . */
                w->color = red;
                z = z->pr;
            }
            else
            {
                if (w->lft->color == black)
                { /* C a s e 7 : w ’ s l e f t c h i l d is b l a c k . */
                    w->color = red;
                    w->rt->color = black;
                    r = left_rotate(r, w);
                    w = z->pr->lft;
                }
                w->color =
                    z->pr->color; /* C a s e 8 ; w ’ s l e f t c h i l d is r e d . */
                z->pr->color = black;
                w->lft->color = black;
                r = right_rotate(r, z->pr);
                z = r;
            }
        }
    }
    z->color = black;
    return r;
}
rbptr rb_clear_tree(rbptr root)
{
    if (root != &NIL)
    {
        root->lft = rb_clear_tree(root->lft);
        root->rt = rb_clear_tree(root->rt);
        free(root);
        root = &NIL;
    }
    return root;
}
int getline(char bfu[], int max)
{
    if (fgets(bfu, max, stdin) == NULL)
        return 0;
    else
        return (strlen(bfu));
}
void rb_print_node(rbptr p)
{
    if (p != &NIL)
    {
        if (p->color == red)
            printf("[%d (red)]", p->key);
        else
            printf("[%d (black)]", p->key);
    }
}
void rb_pre_visit_tree(rbptr root)
{
    if (root != &NIL)
    {
        rb_print_node(root);
        rb_pre_visit_tree(root->lft);
        rb_pre_visit_tree(root->rt);
    }
}

void usage(void)
{
    fputs(("Red−Black Tree test program:\n\
\n\
insert Insert a node.\n\
delete Delete a node.\n\
find Find node.\n\
print Print the tree in preorder.\n\
clear Clear entire tree.\n\
help Show this help message.\n\
quit Quit.\n\
\n\
"),
          stdout);
}

int main(void)
{
    /* S t o r e t h e k e y to f i n d . */
    Key fdk;
    /* T h e t r e e ’ s r o o t p o i n t e r a n d t h e f i n d e d k e y ’ s p o i n t
     * e r . */
    rbnode *rbt, *fd;
    /* S t o r e i n p u t l i n e s . */
    char cmdline[MAXLINE], arg[MAXLINE];
    rbt = &NIL;
    printf("Type \"help\" for usage.\n>");
    for (;;)
    {
        if (getline(cmdline, sizeof(cmdline)) > 0)
        {
            /* I n s e r t n o d e . */
            if (strcmp(cmdline, INSERT) == 0)
            {
                printf("Input the keys to insert,\
use \"^D\" to terminate input.\n");
                rbt = rb_build_tree(rbt);
                printf("\n>");
            }
            else if (strcmp(cmdline, FIND) == 0)
            {
                /* F i n d n o d e . */
                printf("Input the key to find:");
                if (getline(arg, MAXLINE) > 0)
                {
                    if (sscanf(arg, "%d", &fdk) == 1)
                    {
                        if ((fd = rb_search(rbt, fdk)) != &NIL)
                        {
                            printf("The node is finded: ");
                            rb_print_node(fd);
                            printf("\n>");
                        }
                        else
                            printf("No such node\n>");
                    }
                }
            }
            else if (strcmp(cmdline, DELETE) == 0)
            {
                /* D e l e t e n o d e . */
                printf("Input the key to delete:");
                if (getline(arg, MAXLINE) > 0)
                {
                    if (sscanf(arg, "%d", &fdk) == 1)
                    {
                        if ((fd = rb_search(rbt, fdk)) != &NIL)
                        {
                            rbt = rb_delete(rbt, fd);
                            printf("The node is deleted\n>");
                        }
                        else
                            printf("No such node\n>");
                    }
                }
            }
            else if (strcmp(cmdline, PRINT) == 0)
            {
                /* P r i n t n o d e . */
                printf("All nodes in preorder\n");
                rb_pre_visit_tree(rbt);
                printf("\n>");
            }
            else if (strcmp(cmdline, CLEAR) == 0)
            {
                /* C l e a r e n t i r e t r e e . */
                rbt = rb_clear_tree(rbt);
                printf("\n>");
            }
            else if (strcmp(cmdline, QUIT) == 0)
                /* Q u i t p r o g r a m . */
                exit(0);
            else if (strcmp(cmdline, HELP) == 0)
            {
                /* H e l p m e s s a g e */
                usage();
                printf(">");
            }
            else
                /* H a n d l e i n n v a l i d c o m m a n d s . */
                printf("Invalid command. Type \"help\"\
for more information.\n>");
        }
    }
}
