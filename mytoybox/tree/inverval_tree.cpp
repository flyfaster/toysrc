
struct interval_segment {
	int start;
	int end;
};

// https://walkccc.github.io/CLRS/Chap14/Problems/14-1/
struct interval_end_point
{
    int pos;   // start or end of interval
    int val;   // p(x) = +1 if it is left endpoint; p(x) = -1 if it is right endpoint
    int v;     // the sum of the values of all nodes in the subtree rooted at x
    int m = 0; // the maximum value obtained by the expression
    int o = 0; // the value of i for which x.m achieves its maximum.
    interval_end_point* left{nullptr};
    interval_end_point* right{nullptr};
};
// x.v = x.left.v + p(x) + x.right.v
// x.m = max(x.left.m, x.left.v + p(x), x.left.v + p(x) + x.right.m
