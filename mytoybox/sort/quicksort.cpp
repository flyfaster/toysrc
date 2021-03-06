#include <iostream>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <array>
#include <iterator>
#include <vector>
#include <boost/timer/timer.hpp>
using namespace std;

void quickSort(int arr[], int left, int right) {
      int i = left, j = right;
      int tmp;
      int pivot = arr[(left + right) / 2];
      /* partition */
      while (i <= j) {
            while (arr[i] < pivot)
                  i++;
            while (arr[j] > pivot)
                  j--;
            if (i <= j) {
                  tmp = arr[i];
                  arr[i] = arr[j];
                  arr[j] = tmp;
                  i++;
                  j--;
            }
      };
      /* recursion */
      if (left < j)
            quickSort(arr, left, j);
      if (i < right)
            quickSort(arr, i, right);
}

void quickSort2(int arr[], int left, int right) {
      int i = left, j = right;
      int pivot = arr[(left + right) / 2];
      /* partition */
      if (left==right)
      {
    	  std::cout<<__FUNCTION__<<" left "<< left<<", right "<<right<<std::endl;
    	  return;
      }
      while (i <= j) {
            while (arr[i] < pivot)
                  i++;
            while (arr[j] > pivot)
                  j--;
            if (i<j)
            	std::swap(arr[i], arr[j]);
            if (i <= j) {
                  i++;
                  j--;
            }
      };
      /* recursion */
      if (left < j)
            quickSort2(arr, left, j);
      if (i < right)
            quickSort2(arr, i, right);
}

void quickSort3(int arr[], int left, int right) {
      /* partition */
      if (left==right)
      {
    	  std::cout<<__FUNCTION__<<" left "<< left<<", right "<<right<<std::endl;
    	  return;
      }
      int i = left, j = right;
      int pivot = arr[(left + right) / 2];

      while (i <= j) {
            while (arr[i] < pivot)
                  i++;
            while (arr[j] > pivot)
                  j--;
            if (i<j)
            	std::swap(arr[i], arr[j]);
            if (i <= j) {
                  i++;
                  j--;
            }
      };
      /* recursion */
      if (left < j)
            quickSort3(arr, left, j);
      if (i < right)
            quickSort3(arr, i, right);
}

std::array<int, 1024> unsortarr, sortarr1, sortarr2, sortarr3, sortarr4;

int partition(int* nums, int beg, int end)
{
    if (end - beg <= 1)
        return beg;
    auto pivot = nums[end - 1];
    int i = beg - 1; // track elements less or equal than x
    for (int j = beg; j < end - 1; ++j)
    {
        if (nums[j] <= pivot)
        {
            ++i;
            swap(nums[i], nums[j]);
        }
    }
    swap(nums[++i], nums[end - 1]);
    return i;
}

void quicksort_mit(int* nums, int beg, int end)
{
    if (end - beg <= 1)
        return;
    int q = partition(nums, beg, end);
    quicksort_mit(nums, beg, q);
    quicksort_mit(nums, q + 1, end);
}

int main(int argc, char* argv[])
{
    boost::timer::auto_cpu_timer processtimer;
    cout << argv[0] << " start\n";
    //	for (int i=0; i<4; )
    //	{
    //		int data = 0;
    //		data = i++;
    //		std::cout << data <<std::endl;
    //	}
    //	for (int i=0; i<4; )
    //	{
    //		int data = 0;
    //		data = ++i;
    //		std::cout << data <<std::endl;
    //	}
    //	return 0;

    for (int i = 0; i < 1; i++)
    {
        for (auto&& i : unsortarr) // http://en.cppreference.com/w/cpp/language/range-for
            i = rand() % unsortarr.size();

        std::copy(unsortarr.begin(), unsortarr.end(), sortarr1.begin());
        std::copy(unsortarr.begin(), unsortarr.end(), sortarr2.begin());
        std::copy(unsortarr.begin(), unsortarr.end(), sortarr4.begin());
        memcpy(sortarr3.data(), unsortarr.data(), sizeof(unsortarr));
        {
            boost::timer::auto_cpu_timer t;
            quickSort(sortarr1.data(), 0, unsortarr.size() - 1);
        }
        cout << "quicksort done\n";
        {
            boost::timer::auto_cpu_timer t;
            quickSort2(sortarr2.data(), 0, unsortarr.size() - 1);
        }
        cout << "quickSort2 done\n";
        quickSort3(sortarr3.data(), 0, unsortarr.size() - 1);
        cout << "quickSort3 done\n";

        quicksort_mit(sortarr4.data(), 0, unsortarr.size());
        cout << "quicksort_mit done\n";

        for (size_t i = 0; i < unsortarr.size(); i++)
        {
            if (sortarr1[i] != sortarr2[i])
                std::cout << i << " diff12 " << sortarr1[i] << ", " << sortarr2[i]
                          << std::endl;
            if (sortarr1[i] != sortarr3[i])
                std::cout << i << " diff13 " << sortarr1[i] << ", " << sortarr3[i]
                          << std::endl;

            if (sortarr1[i] != sortarr4[i])
                std::cout << i << " diff14 " << sortarr1[i] << ", " << sortarr4[i]
                          << std::endl;
        }
    }
    cout << argv[0] << " done\n";
    return 0;
}
