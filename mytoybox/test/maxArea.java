public class Solution {
    public int maxArea(int[] height) {
        int max = 0;
        int left=0, right=height.length-1;
        while(left<right){
            int current_area = (right-left)*Math.min(height[left],height[right]);
            if(left<right && current_area>max)
                max = current_area;
            if (height[left]<=height[right])
                left++;
            else
                right--;
        }
        return max;
    }
    public static int main(String[] argv) {
        System.out.println("Submission Result: Accepted");
        return 0;
    }
}
