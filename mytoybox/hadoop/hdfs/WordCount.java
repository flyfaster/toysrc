import java.io.IOException;
import java.util.*;
import java.text.SimpleDateFormat;
import java.text.DateFormat;     
import java.net.*;
import java.net.URLClassLoader;   
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
        
public class WordCount {
        
 public static class Map extends Mapper<LongWritable, Text, Text, IntWritable> {
    private final static IntWritable one = new IntWritable(1);
    private Text word = new Text();
        
    public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
		String methodname = new Object(){}.getClass().getEnclosingMethod().getName();
		DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss ");		
		String start_time =dateFormat.format(new Date());
        String line = value.toString();
        StringTokenizer tokenizer = new StringTokenizer(line);
        while (tokenizer.hasMoreTokens()) {
            word.set(tokenizer.nextToken());
            context.write(word, one);
        }        
		String stop_time =dateFormat.format(new Date());
		System.out.format("%s start at %s, finished at %s\n", methodname, start_time, stop_time);
    }
 } 
        
 public static class Reduce extends Reducer<Text, IntWritable, Text, IntWritable> {

    public void reduce(Text key, Iterable<IntWritable> values, Context context) 
      throws IOException, InterruptedException {
		  		String methodname = new Object(){}.getClass().getEnclosingMethod().getName();
		DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss ");		
		String start_time =dateFormat.format(new Date());
        int sum = 0;
        for (IntWritable val : values) {
            sum += val.get();
        }
        context.write(key, new IntWritable(sum));
        		String stop_time =dateFormat.format(new Date());
		System.out.format("%s start at %s, finished at %s\n", methodname, start_time, stop_time);
    }
 }
        
 public static void main(String[] args) throws Exception {
	 ClassLoader cl = ClassLoader.getSystemClassLoader();
        URL[] urls = ((URLClassLoader)cl).getURLs();
        for(URL url: urls){
        	System.out.println(url.getFile());
        }
    String path = WordCount.class.getProtectionDomain().getCodeSource().getLocation().getPath();
	String decodedPath = URLDecoder.decode(path, "UTF-8");
	System.out.println(decodedPath);
	decodedPath = "/usr/onega/HDFSHelloWorld.jar"; // java.io.FileNotFoundException: File /usr/onega/HDFSHelloWorld.jar does not exist
	decodedPath = "/tmp/HDFSHelloWorld.jar";
    //Configuration conf = new Configuration();        
    //    Job job = new Job(conf, "wordcount"); //  warning: [deprecation] Job(Configuration,String) in Job has been deprecated
    Job job = Job.getInstance();
    job.setJarByClass(WordCount.class);
    job.setJobName("wordcount");
    //job.setJar(decodedPath);
//job.setJar("HDFSHelloWorld.jar"); // java.io.FileNotFoundException: File HDFSHelloWorld.jar does not exist
//    job.addFileToClassPath(new Path(decodedPath)); 
    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(IntWritable.class);
        
    job.setMapperClass(Map.class);
    job.setReducerClass(Reduce.class);
        
    job.setInputFormatClass(TextInputFormat.class);
    job.setOutputFormatClass(TextOutputFormat.class);
        
    FileInputFormat.addInputPath(job, new Path(args[0]));
    FileOutputFormat.setOutputPath(job, new Path(args[1]));
        
    job.waitForCompletion(true);
 }
        
}
