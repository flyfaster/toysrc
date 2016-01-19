import java.io.File;
import java.io.IOException;
import java.util.*;
import java.text.SimpleDateFormat;
import java.text.DateFormat;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.Path;

public class HDFSHelloWorld {

  public static final String theFilename = "hello.txt";
  public static final String message = "Hello, hadoop-2.6.3.tar.gz!\n";

  public static void main (String [] args) throws IOException {

    Configuration conf = new Configuration();
    FileSystem fs = FileSystem.get(conf);
    Path filenamePath = new Path(theFilename);
	DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss ");
	Date date = new Date();
	String curtime =dateFormat.format(date); 
    try {
      if (fs.exists(filenamePath)) {        
        System.out.format("remove the file first %s\n", theFilename);
        fs.delete(filenamePath, true);
      }

      FSDataOutputStream out = fs.create(filenamePath);
      out.writeUTF(curtime + message);
              Map<String, String> env = System.getenv();
        for (String envName : env.keySet()) {
			String sysenv = String.format("%s=%s\n",
                              envName,
                              env.get(envName));
            out.writeUTF(sysenv);
            System.out.format("%s=%s\n",
                              envName,
                              env.get(envName));
        }
      out.close();
      FSDataInputStream in = fs.open(filenamePath);
      String messageIn = in.readUTF();
      System.out.print(messageIn);
      in.close();
    } catch (IOException ioe) {
      System.err.println("IOException during operation: " + ioe.toString());
      System.exit(1);
    }
  }
}

/*
[onega@localhost hadoop-2.6.3]$ /home/onega/hadoop-2.6.3/sbin/start-dfs.sh
Incorrect configuration: namenode address dfs.namenode.servicerpc-address or dfs.namenode.rpc-address is not configured.
Starting namenodes on []
The authenticity of host 'localhost (::1)' can't be established.
ECDSA key fingerprint is 6f:80:a0:f0:15:0a:88:89:d7:b2:70:33:e9:dd:63:3b.
Are you sure you want to continue connecting (yes/no)? yes
localhost: Warning: Permanently added 'localhost' (ECDSA) to the list of known hosts.
onega@localhost's password: 
localhost: starting namenode, logging to /home/onega/hadoop-2.6.3/logs/hadoop-onega-namenode-localhost.localdomain.out
onega@localhost's password: 
localhost: starting datanode, logging to /home/onega/hadoop-2.6.3/logs/hadoop-onega-datanode-localhost.localdomain.out
Starting secondary namenodes [0.0.0.0]
The authenticity of host '0.0.0.0 (0.0.0.0)' can't be established.
ECDSA key fingerprint is 6f:80:a0:f0:15:0a:88:89:d7:b2:70:33:e9:dd:63:3b.
Are you sure you want to continue connecting (yes/no)? yes
0.0.0.0: Warning: Permanently added '0.0.0.0' (ECDSA) to the list of known hosts.
onega@0.0.0.0's password: 
0.0.0.0: starting secondarynamenode, logging to /home/onega/hadoop-2.6.3/logs/hadoop-onega-secondarynamenode-localhost.localdomain.out
[onega@localhost hadoop-2.6.3]$ 
[onega@localhost hadoop-2.7.1]$ /home/onega/hadoop-2.7.1/bin/hadoop jar /tmp/HDFSHelloWorld.jar HDFSHelloWorld
[onega@localhost hadoop-2.7.1]$ /home/onega/hadoop-2.7.1/bin/hadoop fs -cat hello.txt
*/
 
