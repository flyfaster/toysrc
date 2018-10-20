/* UVa 11450 - Wedding Shopping - Top Down */
// this code is similar to recursive backtracking code
// parts of the code specific to top-down DP are commented with: `TOP-DOWN'
// if these lines are commented, this top-down DP will become backtracking!
// https://github.com/luisfcofv/competitive-programming-book/blob/master/ch3/ch3_02_UVa11450_td.cpp

#include <algorithm>
#include <cstdio>
#include <cstring>
using namespace std;

int M, C, price[25][25]; // price[g (<= 20)][model (<= 20)]
int memo[210][25];       // TOP-DOWN: dp table memo[money (<= 200)][g (<= 20)]
// return money we've spent
int shop(int budget, int garment)
{
    if (budget < 0)
        return -1000000000; // fail, return a large -ve number
    if (garment == C)
        return M - budget; // we have bought last garment, done
    int& ans = memo[budget][garment]; // TOP-DOWN: memorization
    if (ans != -1)
        return ans;

    for (int model = 1; model <= price[garment][0]; model++) // try all models of specified garment
        ans = max(ans, shop(budget - price[garment][model], garment + 1));
    return ans; // TOP-DOWN: assign ans to table + return it
}

/*
int shop(int money, int g) {
  if (money < 0) return -1000000000;
  if (g == C) return M - money;
  int &ans = memo[money][g];                 // remember the memory address
  if (ans != -1) return ans;
  for (int model = 1; model <= price[g][0]; model++)
    ans = max(ans, shop(money - price[g][model], g + 1));
  return ans;                // ans (or memo[money][g]) is directly updated
}
void print_shop(int money, int g) { // this function does not return anything
  if (money < 0 || g == C) return;                      // similar base cases
  for (int model = 1; model <= price[g][0]; model++)   // which model is opt?
    if (shop(money - price[g][model], g + 1) == memo[money][g]) { // this one
      printf("%d - ", price[g][model]);
      print_shop(money - price[g][model], g + 1); // recurse to this one only
      break;
}   }
*/

int main()
{ // easy to code if you are already familiar with it
    int TC, score;

    scanf("%d", &TC);
    while (TC--)
    {
        scanf("%d %d", &M, &C);
        for (int i = 0; i < C; i++)
        {
            scanf("%d", &price[i][0]); // store K in price[i][0]
            for (int j = 1; j <= price[i][0]; j++)
                scanf("%d", &price[i][j]);
        }
        memset(memo, -1, sizeof memo); // TOP-DOWN: initialize DP memo table
        score = shop(M, 0);            // start the top-down DP
        if (score < 0)
            printf("no solution\n");
        else
            printf("%d\n", score);
    }
    return 0;
}
