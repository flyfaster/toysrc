import java.io.*;
import java.util.jar.JarFile;
import java.util.jar.JarEntry;
import java.util.zip.ZipEntry;
import java.util.LinkedList;
import java.util.Enumeration;
import java.util.List;

public class findjar {

    private static String CLASS_FILE_TO_FIND =
            "org.apache.log4j.RollingFileAppender";
    private static boolean mVerbose = false;
	private static String CLASS_SUFFIX = ".class";
    
    /**
     * @param args the first argument is the directory of the jar files to search in. 
     * The second may be the class name to find.
     */
    public static void main(String[] args) {
        if (args.length < 2) {
        	System.err.printf("Usage: java -cp . findjar %s/.gradle RollingFileAppender\n", 
        			System.getenv("HOME"));
        	return;
        }
        File start = new File(args[0]);
        CLASS_FILE_TO_FIND = args[1];
        searchDirForClass(start);
    }
    
    private static void searchDirForClass(File start) {
        try {
            final FileFilter filter = new FileFilter() {
                public boolean accept(File pathname) {
                    return pathname.getName().endsWith(".jar") || pathname.isDirectory();
                }
            };
            for (File f : start.listFiles(filter)) {
                if (f.isDirectory()) {
                	searchDirForClass(f);
                } else {
                    searchJarForClass(f);
                }
            }
        } 
        catch (Exception e) {
            System.err.println("Error at: " + start.getPath() + " " + e.getMessage());
        }
    }
    
	private static void searchJarForClass(File f) {
		try {
			JarFile jarFile = new JarFile(f);
			Enumeration<JarEntry> entries = jarFile.entries();
			while (entries.hasMoreElements()) {
				JarEntry entry = entries.nextElement();
				String name = entry.getName();
				// org.apache.log4j.RollingFileAppender.class
				String className = name.replace('/', '.');
				if (className.contains(CLASS_FILE_TO_FIND)) {
					if (className.endsWith(CLASS_SUFFIX) && className.length() > CLASS_SUFFIX.length())
						className = className.substring(0, className.length() - CLASS_SUFFIX.length());
					System.out.printf("%s in %s\n", className, f.getAbsolutePath());
					return;
				}
			}
		} catch (java.util.zip.ZipException ex) {
			if (mVerbose)
				System.err.println(ex.toString() + " : " + f.getPath());
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}