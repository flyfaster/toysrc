package oz.html;

import javax.imageio.ImageIO;
import javax.swing.text.html.*;
import javax.swing.text.BadLocationException;
import javax.swing.text.Element;
import javax.swing.text.ElementIterator;

import org.eclipse.swt.widgets.Display;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.URL;
import java.net.URLConnection;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Enumeration;
import java.util.GregorianCalendar;
import java.util.Properties;
import java.util.concurrent.locks.ReentrantLock;
import java.awt.image.BufferedImage;
import java.io.BufferedInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.FileOutputStream;
// http://nadeausoftware.com/node/73
public class Application {
	static MainWnd mw;
	static int min_img_width = 400;
	static int min_img_height = 400;
	private final static ReentrantLock lock = new ReentrantLock();	
	static StringBuffer sb=new StringBuffer();
	public static void main(String[] args) throws InstantiationException, IllegalAccessException, ClassNotFoundException, SQLException {
		if (!ImageDB.checkDerbyClasspath()) return;
		Properties props = new Properties(); 
		props.put("user", "user1"); props.put("password", "user1"); 
		//create and connect the database named helloDB 
		ImageDB.conn=DriverManager.getConnection("jdbc:derby:helloDB;create=true", props); 
		System.out.println("create and connect to helloDB"); 
		ImageDB.conn.setAutoCommit(false); 
		ImageDB.createTables();
		mw = new MainWnd();
		mw.open();
		try {			
			Enumeration netInterfaces=NetworkInterface.getNetworkInterfaces();
            while(netInterfaces.hasMoreElements()){
                    NetworkInterface ni=(NetworkInterface)netInterfaces.nextElement();
                    System.out.println(ni.getName());
                    InetAddress ip = (InetAddress) ni.getInetAddresses().nextElement();
                    if( !ip.isSiteLocalAddress() && !ip.isLoopbackAddress() && ip.getHostAddress().indexOf(":")==-1){
                            System.out.println("Interface "+ni.getName()+" seems to be InternetInterface. I'll take it...");
                            break;
                    }else{
                          ip=null;
                    }
            }
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Done.");		
	}

	static void println(String msg)
	{
		System.out.println(msg);
	     lock.lock();  // block until condition holds
	     try {	    	 
	       sb.append(msg);
	       if (msg.endsWith("\n")==false)
	    	   sb.append("\n");
	     } finally {
	       lock.unlock();
	     }
		Display.getDefault().syncExec(
				  new Runnable() {
				    public void run(){
						if (mw != null)
							mw.println(getLog());
				    }
				  });
	}
	static String getLog()
	{
	     lock.lock();  // block until condition holds
	     try {
	       String ret = sb.toString();
	       sb = new StringBuffer();
	       return ret;
	     } finally {
	       lock.unlock();
	     }	     
	}

	static String makeFullUrl(String page_url, String resource)
	{
		if (resource.contains("http://"))
			return resource;
		int pos = page_url.indexOf('/', 7);
		if (pos>0)
		{
			resource = page_url.substring(0, pos)+resource;
		}
		else
		{
			resource = page_url + resource;
		}
		return resource;
	}
	static void parsePage(String page_url) throws IOException, BadLocationException
	{
	    URL url = new URL( page_url ); 
	    HTMLEditorKit kit = new HTMLEditorKit(); 
	    HTMLDocument doc = (HTMLDocument) kit.createDefaultDocument(); 
	    doc.putProperty("IgnoreCharsetDirective", Boolean.TRUE);
	    Reader HTMLReader = new InputStreamReader(url.openConnection().getInputStream()); 
	    kit.read(HTMLReader, doc, 0); 
	    //  Get an iterator for all HTML tags.
	    ElementIterator it = new ElementIterator(doc); 
	    Element elem; 	    
	    while( (elem = it.next()) != null  )
	    { 
	      if( elem.getName().equals(  "img") )
	      { 
	        String s = (String) elem.getAttributes().getAttribute(HTML.Attribute.SRC);
	        String ww = (String) elem.getAttributes().getAttribute(HTML.Attribute.WIDTH);
	        String hh = (String) elem.getAttributes().getAttribute(HTML.Attribute.HEIGHT);
	        if (ww!= null && hh != null &&
	        		ww.length()>0 && hh.length()>0 
	        		&& Integer.parseInt(ww)<min_img_width 
	        		&& Integer.parseInt(hh)<min_img_height)
	        {
	        	println("Skip " + s + " size " + ww + "," + hh);
	        	continue;
	        }
	        else
	        {
	        	s = makeFullUrl(page_url, s);
	        	System.out.println(elem.toString());
	        	System.out.println( s + " size " + ww + "," + hh);
	        	try
	        	{
	        	downloadImg(s, null);
	        	}
	        	catch (IOException e) {
	    			println("Exception when download " + s);
	    			e.printStackTrace();
	    		}
	        }
	      } 
	    }
	}
	static void downloadImg(String img_url, String file_path)throws IOException
	{
		int start_time =(int) new GregorianCalendar().getTimeInMillis();
		if (file_path==null)
		{
			int name_pos = img_url.lastIndexOf("/");
			if (name_pos>=0)
			{
				file_path = img_url.substring(name_pos+1);
			}
		}
		if (file_path==null)
		{
			println("Invalid file path.");
			return;
		}
		try {
			if (ImageDB.isProcessedImageUrl(img_url))
			{
				println("Already processed " + img_url);
				return;
			}
		} catch (SQLException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		if (file_path.indexOf(File.separator)<0 && mw.getDestination().length()>0)
		{
			file_path = mw.getDestination()+File.separator+file_path;
		}
        URL url = new URL(img_url);
        BufferedImage bi = ImageIO.read(url);
        int used_time = (int)new GregorianCalendar().getTimeInMillis()-start_time;
        if (bi==null)
        {
        	println(String.format("Fail to read %s, used %d ms.\n", img_url, used_time));
			return;
        }
        if (bi.getHeight()<400 || bi.getWidth()<400)
        {
        	println(String.format("Skip image (%d*%d) %s.\n", bi.getWidth(), bi.getHeight(), img_url));
        	// save url to database in order to skip next time.
            try {
            	ImageDB.saveImageProperty(img_url,bi.getWidth(),bi.getHeight(), 
    					used_time);
    		} catch (SQLException e) {    			
    			e.printStackTrace();
    		}
        	return;
        }
        File f = new File(file_path);
        if (f.exists()) f.delete();
        ImageIO.write(bi,"jpg", f);  
        println(String.format("Downloaded %s in %d ms.\n", img_url, used_time));
        try {
        	ImageDB.saveImageProperty(img_url,bi.getWidth(),bi.getHeight(), 
					used_time);
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	static void downloadFile(String file_url, String file_path)
			throws IOException {
		byte buffer[] = new byte[1024 * 32];
		InputStream is = null; 
		int i = 0;
		int timeout = 6000;
		if (file_path==null)
		{
			int name_pos = file_url.lastIndexOf("/");
			if (name_pos>=0)
			{
				file_path = file_url.substring(name_pos+1);
			}
		}
		if (file_path==null)
		{
			System.out.println("Invalid file path.");
			return;
		}
		URL url = new URL(file_url); // "http://www.java2s.com"
		URLConnection connection = url.openConnection();
		connection .setConnectTimeout(timeout);
		connection .setReadTimeout(timeout*2 );
		is = connection.getInputStream(); // 
		is = new BufferedInputStream(is);
		int contentLength = connection.getContentLength();
	    System.out.printf("%s Content length %d.\n", file_url, contentLength);
	    if (contentLength<0)
	    {    	
	    	return;
	    }
		File fstream = null;
		DataOutputStream out = null;
		int total_bytes = 0;
		while ((i = is.read(buffer)) != -1) {
			total_bytes += i;
			if (fstream == null) {
				fstream = new File(file_path);
				out = new DataOutputStream(new FileOutputStream(fstream));
			}
			System.out.printf("Read %d bytes, total %d.\n", i, total_bytes);
			out.write(buffer, 0, i);
		}
		if (out != null)
		{
			System.out.printf("Read %d bytes in total.\n", total_bytes);
			out.close();
		}
	}
}
