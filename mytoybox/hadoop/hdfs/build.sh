export HADOOP_HOME=/home/onega/hadoop-2.7.1
export JAVA_HOME=/etc/alternatives/java_sdk_1.7.0
export CLASSPATH=`$HADOOP_HOME/bin/hadoop classpath`
javac -Xlint:deprecation *.java
jar -cf /tmp/HDFSHelloWorld.jar *.class 
echo "$HADOOP_HOME/bin/hadoop jar /tmp/HDFSHelloWorld.jar HDFSHelloWorld"
echo "$HADOOP_HOME/bin/hadoop fs -cat hello.txt"
