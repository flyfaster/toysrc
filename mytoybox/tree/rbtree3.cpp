#include <stdlib.h>
#include <iostream>

using namespace std;

struct RBT
{
    int data, color;
    RBT *lchild, *rchild, *parent;
} * root;

using node = RBT;

node* minimum(node* ptr)
{
    while (ptr->lchild)
        ptr = ptr->lchild;
    return ptr;
}

node* maximum(node* ptr)
{
    while (ptr->rchild)
        ptr = ptr->rchild;
    return ptr;
}

void setparent(node* r, node* r1)
{
    if (r)
        r->parent = r1;
}

void rrrotate(node* x)
{
    auto y = x->lchild;
    x->lchild = y->rchild;
    setparent(y->rchild, x);
    setparent(y, x->parent);
    if (x->parent == nullptr)
        root = y;
    else if (x == x->parent->rchild)
        x->parent->rchild = y;
    else
        x->parent->lchild = y;
    y->rchild = x;
    setparent(x, y);
}

void llrotate(node* x)
{
    auto y = x->rchild;
    x->rchild = y->lchild;
    setparent(y->lchild, x);
    setparent(y, x->parent);
    if (x->parent == nullptr)
        root = y;
    else if (x == x->parent->lchild)
        x->parent->lchild = y;
    else
        x->parent->rchild = y;
    y->lchild = x;
    setparent(x, y);
}

node* successor(node* ptr)
{
    if (ptr->rchild)
        return (minimum(ptr->rchild));

    auto y = ptr->parent;
    while (y != nullptr && ptr == y->rchild)
    {
        ptr = y;
        y = y->parent;
    }

    return y ? y : ptr;
}

node* predecessor(node* ptr)
{
    if (ptr->parent)
        if (ptr->parent->rchild == ptr)
            return ptr->parent;

    if (ptr->lchild)
        return maximum(ptr->lchild);
    return ptr;
}

node* binarysearch(int i, node* r)
{
    if (r == nullptr || r->data == i)
        return r;
    if (r->data > i)
        r = r->lchild;
    else
        r = r->rchild;
    return binarysearch(i, r);
}

void RBDeleteFixup(node* x)
{
    while (x != nullptr && x->color == 1)
    {
        if (x == x->parent->lchild)
        {
            if (auto w = x->parent->rchild)
            {
                if (w->color == 0)
                {
                    w->color = 1;
                    x->parent->color = 0;
                    llrotate(x->parent);
                    w = x->parent->rchild;
                }
                if (w->lchild->color == 1 && w->rchild->color == 1)
                {
                    w->color = 0;
                    x = x->parent;
                }
                else if (w->rchild->color == 1)
                {
                    w->lchild->color = 1;
                    w->color = 0;
                    rrrotate(w);
                    w = x->parent->rchild;
                }
                w->color = x->parent->color;
                x->parent->color = 1;
                w->rchild->color = 1;
            }
            llrotate(x->parent);
            x = root;
        }
        else
        {
            if (auto w = x->parent->lchild)
            {
                if (w->color == 0)
                {
                    w->color = 1;
                    x->parent->color = 0;
                    llrotate((x->parent));
                    w = x->parent->lchild;
                }
                if (w->rchild->color == 1 && w->lchild->color == 1)
                {
                    w->color = 0;
                    x = x->parent;
                }
                else if (w->lchild->color == 1)
                {
                    w->rchild->color = 1;
                    w->color = 0;
                    rrrotate(w);
                    w = x->parent->lchild;
                }
                w->color = x->parent->color;
                x->parent->color = 1;
                w->lchild->color = 1;
            }
            llrotate(x->parent);
            x = root;
        }
        x->color = 1;
    }
}

void delet(node* z)
{
    if (z == nullptr)
        return;

    auto y = (z->lchild == nullptr || z->rchild == nullptr) ? z : successor(z);
    auto x = (y->lchild == nullptr) ? y->lchild : y->rchild;

	setparent(x, y->parent);
	if (y->parent == nullptr)
		root = x;
	else if (y == y->parent->lchild)
		y->parent->lchild = x;
	else
		y->parent->rchild = x;
	if (y != z)
		z->data = y->data;
	if (y->color == 1)
		RBDeleteFixup(x);
}

void RBInsertFixup(node* x)
{
    while (x->parent != nullptr && x->parent->color == 0)
    {
        if (x->parent == x->parent->parent->lchild)
        {
            auto y = x->parent->parent->rchild;
            if (y != nullptr)
            {
                if (y->color == 0)
                {
                    x->parent->color = 1;
                    y->color = 1;
                    x->parent->parent->color = 0;
                    x = x->parent->parent;
                }
            }
            else
            {
                if (x == x->parent->rchild)
                {
                    x = x->parent;
                    llrotate(x);
                }
                else
                {
                    x->parent->color = 1;
                    x->parent->parent->color = 0;
                    rrrotate(x->parent->parent);
                }
            }
        }
        else
        {
            auto y = x->parent->parent->lchild;
            if (y != nullptr)
            {
                if (y->color == 0)
                {
                    x->parent->color = 1;
                    y->color = 1;
                    x->parent->parent->color = 0;
                    x = x->parent->parent;
                }
            }
            else
            {
                if (x == x->parent->lchild)
                {
                    x = x->parent;
                    llrotate(x);
                }
                else
                {
                    x->parent->color = 1;
                    x->parent->parent->color = 0;
                    rrrotate(x->parent->parent);
                }
            }
        }
    }
    root->color = 1;
}

void insert(node* z)
{
    node *x, *y;
    y = nullptr;
    x = root;
    while (x != nullptr)
    {
        y = x;
        if (z->data < x->data)
            x = x->lchild;
        else
            x = x->rchild;
    }
    z->parent = y;
    if (y == nullptr)
        root = z;
    else
    {
        z->parent = y;
        if (z->data < y->data)
            y->lchild = z;
        else
            y->rchild = z;
    }
    RBInsertFixup(z);
}

int height(node* ptr)
{
    if (ptr != nullptr)
        return ((height(ptr->rchild) > height(ptr->lchild) ? height(ptr->rchild) + 1 :
                                                             height(ptr->lchild) + 1));
    else
        return -1;
}

void inorder(node* ptr)
{
	if (!ptr)
		return;

	inorder(ptr->lchild);

    cout << endl << ptr->data <<" ";
    cout <<"Height :"<< height(ptr) <<" ";
    auto z = successor(ptr);
    cout <<"Successor :"<< z->data <<" ";
    z = predecessor(ptr);
    cout <<"Predecessor :"<< z->data <<" "<<"Color :";
    if (ptr->color == 0)
        cout <<"Red";
    else
        cout <<"Black";

    if (ptr->parent)
        cout <<"Parent :"<< ptr->parent->data;

	inorder(ptr->rchild);
}

int search(int key, node* ptr)
{
	int search_result = 0;
    if (ptr != nullptr)
    {
        if (ptr->data == key)
            search_result = 1;
        else
        {
            search_result |= search(key, ptr->lchild);
            search_result |= search(key, ptr->rchild);
        }
    }
    return search_result;
}

int main()
{
    node* z = nullptr;
    int op = 0, item = 0;
    int s = 0, k = 0;
    root = nullptr;
    while (true)
    {
        cout <<"\nRed Black Tree Operations";
        cout <<"\n1.Insertion\n2.Deletion\n3.Searching\n4.Traversing\n5.Exit";
        cout <<"\nEnter your option :";
        cin >> op;
        switch (op)
        {
        case 1:
            z = new node;
            cout <<"Enter Item To Be Inserted ";
            cin >> item;
            z->data = item;
            z->color = 0;
            z->rchild = nullptr;
            z->lchild = nullptr;
            z->parent = nullptr;
            insert(z);
            break;
        case 2:
            if (root == nullptr)
                cout <<"Tree Is Empty\n";
            else
            {
                cout <<"Enter Item To Be Deleted ";
                cin >> item;
                delet(binarysearch(item, root));
            }
            break;
        case 3:
            s = k = 0;
            cout <<"Enter Element To Search : ";
            cin >> item;
            s = search(item, root);
            cout <<"Search Is " << (s!=1?"NOT ":"") << "Found\n";
            break;
        case 4:
            cout <<"Inorder Traversal :\n";
            inorder(root);
            break;
        case 5:
            exit(0);
        }
    }
    return 0;
}
