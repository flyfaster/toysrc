class Solution {
public:
struct myclass {
    bool cmp(int i,int j) { 
      char left[32], right[32];
      sprintf(left, "%d%d", i,j);
      sprintf(right, "%d%d", j,i);
      return (strcmp(left, right)>0);
    }
  bool operator() (int i,int j) { 
      return cmp(i,j);
      
      char left[16], right[16];
      sprintf(left, "%d", i);
      sprintf(right, "%d", j);
      if(strstr(right, left)==right && strlen(right) > strlen(left)) {
          return left[strlen(left)-1]>right[strlen(left)];
      }
      if(strstr(left, right )==left && strlen(left) > strlen(right)) {
          return right[strlen(right)-1]<left[strlen(right)];
      }
      return (strcmp(left, right)>0);}
} myobject;

    string largestNumber(vector<int> &num) {
        std::sort (num.begin(), num.end(), myobject); 
        std::stringstream ss;
        bool skip0 = true;
        for (int i=0; i<num.size(); i++) {
            if(skip0 && num[i]==0)
                continue;
            if(num[i])
                skip0 = false;
        ss << num[i];
        }
        if (num.size() && ss.str().size()==0)
            ss <<"0";
        return ss.str();
    }
};
