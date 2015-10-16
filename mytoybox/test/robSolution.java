public class robSolution {
	
    public int rob(int[] nums) {
        if(nums.length<1) return 0;
        if(nums.length==1) return nums[0];
        int grandparent = nums[0];
        int parent=Math.max(grandparent, nums[1]);
        if(nums.length==2) {
            return parent;
        }
        int ret=0;
        for(int i=2; i<nums.length; i++) {
            ret =Math.max(grandparent+nums[i],parent);
            grandparent=parent;
            parent=ret;
        }
        return ret;
    }
    public static void main(String[] args) {
    	robSolution sol=new robSolution();
    	int nums[]={2,1,1,2};
    	int ret = sol.rob(nums);
    	System.out.format("return %d Submission Result: Accepted\n", ret);
    }
}
