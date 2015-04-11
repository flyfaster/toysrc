package oz.html;

import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class ImageDB {
	static Connection conn;
	static boolean checkDerbyClasspath() throws InstantiationException,
	IllegalAccessException {
		try {
			Class.forName("org.apache.derby.jdbc.EmbeddedDriver").newInstance();
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			String os = System.getProperty("os.name").toLowerCase();
			boolean osIsWindows;
			osIsWindows = os != null && os.indexOf("windows") != -1;
			if (osIsWindows)
				System.out.println("Need to put glassfishv3\\javadb\\lib\\derby.jar in class path.");
			else
				System.out.println("Need to put glassfishv3/javadb/lib/derby.jar in class path.");
			return false;
		}
		return true;
	}
	static void createTables() throws SQLException {
		boolean has_pagetable =false;
		boolean has_imgtable = false;
		DatabaseMetaData dbm = conn.getMetaData();
		ResultSet tables = dbm.getTables(null, null, "%", null);
		while (tables.next()) {
		// Table exists
			if (tables.getString("TABLE_NAME").compareToIgnoreCase("pagetable")==0)
				{
				has_pagetable = true;
				}
			if (tables.getString("TABLE_NAME").compareToIgnoreCase("imgtable2")==0)
			{
			has_imgtable = true;
			}
			System.out.println(tables.getString("TABLE_NAME"));
		}
		if (has_pagetable == false)
		{
		Statement s = conn.createStatement(); 
		s.execute("create table pagetable(pageurl varchar(2048) )"); 
		conn.commit();
		}
		if (has_imgtable == false)
		{
		Statement s = conn.createStatement(); 
		s.execute("create table imgtable2(pageurl varchar(2048), width int, height int, downloadtime int )"); 
		conn.commit();
		}
	}
	static void saveImageProperty(String url, int w, int h, int downloadtime) throws SQLException
	{
		if (conn==null)
			return;
		String sql;
		sql = "insert into IMGTABLE2(pageurl, width, height, downloadtime) VALUES (?,?,?,?)";
		PreparedStatement ps = conn.prepareStatement(sql);
		ps.setString(1,url);
		ps.setInt(2, w);
		ps.setInt(3, h);
		ps.setInt(4, downloadtime);
		int c = ps.executeUpdate();		
		conn.commit();
	}
	static boolean isProcessedImageUrl(String s) throws SQLException
	{
		if (conn==null)
			return false;
		String sql;
		sql = "SELECT * FROM IMGTABLE2 where pageurl = ?";
		PreparedStatement ps = conn.prepareStatement(sql);
		ps.setString(1,s);
		ResultSet rs = ps.executeQuery();		
		return rs.next();
	}
}
