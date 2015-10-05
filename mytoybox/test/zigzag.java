public class zigzag {
	// Submission Result: Accepted 
    public String convert(String s, int numRows) {
        StringBuilder[] builder = new StringBuilder[numRows];
        for(int i=0; i<builder.length; i++)
            builder[i]=new StringBuilder();
        if(numRows==1) return s;
        int step = numRows*2-2;
        for(int i=0; i<s.length(); i++) {
            int sub = i%step;
            if(sub<numRows)
        	    builder[sub].append(s.charAt(i));
        	else {
        	    int back = (numRows-1)-(sub-numRows+1);
        	    builder[back].append(s.charAt(i));
        	}
        }
        StringBuilder sb=new StringBuilder();
        for(StringBuilder si: builder)
        	sb.append(si.toString());
        return sb.toString();
    }	
}