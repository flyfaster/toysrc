/**
 * The first thing to know about are types. The available types in Thrift are:
 *
 *  bool        Boolean, one byte
 *  byte        Signed byte
 *  i16         Signed 16-bit integer
 *  i32         Signed 32-bit integer
 *  i64         Signed 64-bit integer
 *  double      64-bit floating point value
 *  string      String
 *  binary      Blob (byte array)
 *  map<t1,t2>  Map from one type to another
 *  list<t1>    Ordered list of one type
 *  set<t1>     Set of unique elements of one type
 *
  */

/**
 * Thrift files can reference other Thrift files to include common struct
 * and service definitions. These are found using the current path, or by
 * searching relative to any paths specified with the -I compiler flag.
 *
 * Included objects are accessed using the name of the .thrift file as a
 * prefix. i.e. shared.SharedObject
 */
namespace cpp htmltest

/**
 * Thrift also lets you define constants for use across languages. Complex
 * types and structs are specified using JSON notation.
 */
const i32 CheckSumServiceDefaultPort = 9853

struct CheckSumResult {
  1: i32 status
  2: string checksum
  3: string errormessage
}

struct CheckSumProgress {
  1: i32 folders
  2: i32 files
  3: i32 processed_folders
  4: i32 processed_files
  5: i32 error_files
  6: i32 error_folders
  7: string errormessage
}

service CheckSumService {
   	CheckSumResult checksum(1:string srcfilename),	// 
   	binary datachecksum(1:binary data),
	i32 duplicated(1:string input_checksum),// 
   	oneway void addfolder(1:string pathname),
	CheckSumProgress getprogress(),
	void ping(),
	oneway void shutdown(1:string msg)
}
