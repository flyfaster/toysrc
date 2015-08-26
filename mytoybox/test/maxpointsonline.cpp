/**
 * Definition for a point.
 * struct Point {
 *     int x;
 *     int y;
 *     Point() : x(0), y(0) {}
 *     Point(int a, int b) : x(a), y(b) {}
 * };
 */
class Solution {
public:
    bool sameLine(const Point& start, const Point& end, const Point& c ) {
        if (c.x>max(start.x, end.x) || c.y>max(start.y, end.y))
            return false; // quick check
        if (c.x<min(start.x, end.x) || c.y<min(start.y, end.y))
            return false; // quick check
        if (c.x==start.x && c.y == start.y) return true;
        if (c.x==end.x && c.y == end.y) return true;
        if (c.x==start.x && c.x == end.x) return true;
        if (c.y==start.y && c.y == end.y) return true;
        if ((c.x-start.x)*(end.y-start.y) == (c.y-start.y)*(end.x-start.x))
            return true;
        return false;
    }
    int maxPoints(vector<Point> &points) {
        int pointscnt = points.size();
        int maxcnt = 0;
        if (pointscnt<2)
            return pointscnt;
            
        for(int si=0; si<pointscnt; si++){
            for(int ei=si+1; ei<pointscnt; ei++) {
                int cnt=0;
                for(int ci=0; ci<pointscnt; ci++) {
                    if (sameLine(points[si], points[ei], points[ci]))
                        cnt++;
                }
                maxcnt = max(maxcnt, cnt);
            }
        }
        return maxcnt;
    }
};
