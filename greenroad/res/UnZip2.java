//javac UnZip2.java
//java UnZip2.java HookDemo.zip
import java.io.*;
import java.util.*;
import java.util.zip.*;

public class UnZip2 
{
   static final int BUFFER = 2048;
   public UnZip2()
   {
   }
   public static void main (String argv[]) 
   {
      try 
      {
         //String zip_file_name = argv[0];
         //unzipFile(zip_file_name);
         UnZip2 zipobj = new UnZip2();
//         zipobj.zip("c:\\test.zip","C:\\runtime"); 
			zipobj.addFile("C:\\test\\java\\compress\\22");
			zipobj.addFile("C:\\test\\java\\compress\\2");
			zipobj.compress("c:\\test\\java\\compress\\2.zip");
			zipobj.unzipFile("c:\\test\\java\\compress\\2.zip","c:\\test\\java\\compress\\3");
      } catch(Exception e) {
         e.printStackTrace();
      }
   }
   static void unzipFile(String zip_file_name,String dest_path) throws Exception
   {
         BufferedOutputStream dest = null;
         BufferedInputStream is = null;
         File f = new File(zip_file_name);
         System.out.println(f.getName());
         System.out.println(f.getAbsolutePath());
         String unzip_path = f.getAbsolutePath();
         //unzip_path = unzip_path.substring(0,unzip_path.length()-f.getName().length());
         unzip_path = dest_path;
         File dest_dir = new File(dest_path);
         if(!dest_dir.exists())
         	dest_dir.mkdirs();
         if(!unzip_path.endsWith(File.separator))
         {
         	unzip_path += File.separator;
         }
         System.out.println(unzip_path);
         ZipEntry entry;
         ZipFile zipfile = new ZipFile(zip_file_name);
         
         Enumeration e = zipfile.entries();
         while(e.hasMoreElements()) 
         {
            entry = (ZipEntry) e.nextElement();
            System.out.println("Extracting: " +entry);
            	String pathname_to_create = unzip_path+entry.getName();
            	pathname_to_create = pathname_to_create.replace('/',File.separatorChar);
            if(entry.isDirectory())
            {
            	File dir = new File(pathname_to_create);
            	if(dir.exists()==false)
            	{
            		dir.mkdirs();  
            		//System.out.println("create dir:"+pathname_to_create);
            	}
            	else
            	{
            		//System.out.println("existed dir:"+pathname_to_create);
            	}

        	}
	        else
	        {
	        	//create dir
	        	int dir_pos = pathname_to_create.lastIndexOf(File.separator);
	        	String pathname = pathname_to_create.substring(0,dir_pos);
	        	File file_dir=new File(pathname);
	        	if(file_dir.exists()==false)
	        		file_dir.mkdirs();
	            is = new BufferedInputStream(zipfile.getInputStream(entry));
	            int count;
	            byte data[] = new byte[BUFFER];
	            FileOutputStream fos = new FileOutputStream(pathname_to_create);
	            dest = new BufferedOutputStream(fos, BUFFER);
	            while ((count = is.read(data, 0, BUFFER)) != -1) 
	            {
	               dest.write(data, 0, count);
	            }
	            dest.flush();
	            dest.close();
	            is.close();        	
	        	
	        }

         }   	
   }
	public void zip(ZipOutputStream out,File f,String base)throws Exception
	{ 
		System.out.println("Zipping  "+f.getName()); 
		if (f.isDirectory()) 
		{ 
			File[] fl=f.listFiles(); 
			base=base.length()==0?"":base+"/"; 
			out.putNextEntry(new ZipEntry(base+f.getName()+"/")); 
			for (int i=0;i<fl.length ;i++ ) 
			{ 
				zip(out,fl[i],base+f.getName()); 
			} 
		} 
		else 
		{ 
			String my_entry_name = "";
			//base=base.length()==0?"":base+"/";
			if(base.length()>0) 
				my_entry_name = base+"/"+f.getName(); 
			else
				my_entry_name = f.getName();
			System.out.println(my_entry_name);
			out.putNextEntry(new ZipEntry(my_entry_name));
			FileInputStream in=new FileInputStream(f); 
			int count; 
			byte data[] = new byte[4096]; 
			while ( (count = in.read(data, 0,4096) )  != -1) 
			{ 
				out.write(data, 0, count); 
			} 
			in.close(); 
		} 
	
	} 
   	public void zip(String zipFileName,String inputFile)throws Exception
   	{ 
		zip(zipFileName,new File(inputFile)); 
	} 
	public void zip(String zipFileName,File inputFile)throws Exception
	{ 
		ZipOutputStream out=new ZipOutputStream(new FileOutputStream(zipFileName)); 
		zip(out,inputFile,""); 
		System.out.println("zip done"); 
		out.close(); 
	} 
	
	private Vector<String> fileList = new Vector<String>();
	
	public void addFile(String s)
	{
		fileList.add(s);
	}
	public boolean compress(String destPath)
	{		
		if(destPath.length()<1)
			return false;
		try
		{
			ZipOutputStream out=new ZipOutputStream(new FileOutputStream(destPath)); 
			Iterator<String> it = fileList.iterator();
			while(it.hasNext())
			{
				String file_name = it.next();
				zip(out, new File(file_name),"");
			}
			out.close();
		}
		catch(Exception e)
		{
			return false;
		}
		return true;
	}
}

class Zip {
   static final int BUFFER = 2048;
   public static void zipFolder (String src_folder_name, String dest_file_name) {
      try {
         BufferedInputStream origin = null;
         FileOutputStream dest = new 
           FileOutputStream(dest_file_name);
         CheckedOutputStream checksum = new 
           CheckedOutputStream(dest, new Adler32());
         ZipOutputStream out = new 
           ZipOutputStream(new 
             BufferedOutputStream(checksum));
         //out.setMethod(ZipOutputStream.DEFLATED);
         byte data[] = new byte[BUFFER];
         // get a list of files from current directory
         File f = new File("src_folder_name");
         String files[] = f.list();

         for (int i=0; i < files.length; i++) 
         {
            System.out.println("Adding: "+files[i]);
            FileInputStream fi = new 
              FileInputStream(files[i]);
            origin = new 
              BufferedInputStream(fi, BUFFER);
            ZipEntry entry = new ZipEntry(files[i]);
            out.putNextEntry(entry);
            int count;
            while((count = origin.read(data, 0, 
              BUFFER)) != -1) {
               out.write(data, 0, count);
            }
            origin.close();
         }
         out.close();
         System.out.println("checksum: "+checksum.getChecksum().getValue());
      } catch(Exception e) {
         e.printStackTrace();
      }
   }
} 

