/**
 * Definition for singly-linked list.
 * public class ListNode {
 *     int val;
 *     ListNode next;
 *     ListNode(int x) { val = x; }
 * }
 */

 class ListNode {
     int val;
      ListNode next;
      ListNode(int x) { val = x; }
  }
 
public class reverseKGroup {
    public ListNode reverseKGroup(ListNode head, int k) {
        ListNode start=head;
        ListNode start_parent=null;
        ListNode end_child=null;
        ListNode pre=null;
        ListNode seghead=null;
        ListNode ret=null;
        int len=0;
        while(start!=null) { len++; start=start.next;}
        if(k<=1 || k>len) return head;
        
        start=head;
        int segments=len/k;
        for(int i=0; i<segments; i++) {
            seghead=start;
            for(int j=0; j<k; j++) {
            	end_child = start.next;
                start.next=pre; 
                pre=start; 
                start=end_child; 
            }
            if(ret==null)
            ret = pre;            
            
            if(start_parent!=null) {
            	start_parent.next = pre;
            }
            start_parent=seghead;
            seghead.next=end_child;
        }
        if (seghead!=null)
        	seghead.next=end_child;
        if(ret==null) return head;
        return ret;
    }
    public static void main(String[] args) {
    	ListNode head=null; ListNode a=null;
    	int cnt=2;
    	if(args.length>0)
    		cnt = Integer.parseInt(args[0]);
    	for(int i=0; i<cnt; i++) {
    		if(head==null) {
    			a = new ListNode(i);
    			head=a;
    		} else {
    			a.next = new ListNode(i);
    			a = a.next;
    		}    		
    	}
    	reverseKGroup mysol=new reverseKGroup();
    	ListNode ret = mysol.reverseKGroup(head, 2);
    	System.out.format("input length %d result\n", cnt);
    	while(ret!=null) {
    		System.out.format("%d ", ret.val);
    		ret=ret.next;
    	}
    }
}
// javac reverseKGroup.java & java -cp . reverseKGroup 3

/*
public class Solution {
    public boolean canWinNim(int n) {
        if(n<4) return true;
        if(n%4 <1) return false;
        return true;
    }
}

public class Solution { // combination-sum-iii
    private void combination(List<List<Integer>> ret, List<Integer> a, int k, int n) {
        int lastelement = 0;
        if( a.isEmpty()==false)
            lastelement = a.get(a.size()-1);
        if(k==1 && n<=9 && n>lastelement) {
            a.add(n);
            ret.add(a);
            return;
        }
        if(n<=lastelement) return;
        for(int i=lastelement+1; i<=9; i++) {
            List<Integer> dup = new LinkedList<Integer>(a);
            dup.add(i);
            combination(ret, dup, k-1, n-i);
        }
    }
    public List<List<Integer>> combinationSum3(int k, int n) {
        List<List<Integer>> ret= new LinkedList<List<Integer>>();
        List<Integer> dup = new LinkedList<Integer>( );
        combination(ret, dup, k, n);
        return ret;
    }
}

*/


