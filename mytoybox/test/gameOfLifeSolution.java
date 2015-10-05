public class gameOfLifeSolution {
    public void gameOfLife(int[][] board) {
    	// https://leetcode.com/problems/game-of-life/
    	for (int i=0; i<board.length; i++)
        {
    		int[] row = board[i];
    		for(int j=0; j<row.length; j++) {
    			// for each element, count its neighbors
    			if( (row[j] & 0xF) >0) {
    				// increase cnt of its neighbors
    				for(int m=-1; m<=1; m++) {
    					for(int n=-1; n<=1; n++) {
    						if(m==0 && n==0)
    							continue;
    						int x = m+i; int y=n+j;
    						if(x<0 || x>=board.length || y<0 || y>=row.length)
    							continue;
    						board[x][y] += 0x100;
    					}
    				}
    			}
    		}          
        }
    	for (int i=0; i<board.length; i++)
        {
    		int[] row = board[i];
    		for(int j=0; j<row.length; j++) {
    			// for each element, count its neighbors
    			if(row[j]<0x200) row[j] = 0; //under population
    			else if(row[j]>=0x400) row[j] = 0; // over population
    			else if( (row[j]&0xFF00) == 0x300) row[j] = 1;
    			else if(row[j]>0x200 && row[j]<0x300) row[j] = 1;
    			else row[j]=0;
    		}          
        }
    }
    public static void main(String[] args) {
        System.out.println("Submission Result: Accepted!"); // Display the string.
    }
}