package oz.html;
//import java.awt.datatransfer.Clipboard;
import java.io.File;
import java.io.IOException;
import javax.swing.text.BadLocationException;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Widget;
// http://www.java2s.com/Code/Java/SWT-JFace-Eclipse/SWTTabControl.htm
public class MainWnd extends SelectionAdapter{
    private Text text;
    protected Shell shell;
    private Text pathtext;
    private Text ignorelist;
    private Text includelist;
    private Text logWnd;
    private Button clearLogBtn;
    private Button urlBtn;
    private Clipboard cb;
    Button button;
    Button  dst;
    TabFolder tabFolder;
    /**
     * Open the window
     */
    public void open() {
        final Display display = Display.getDefault();
        cb = new Clipboard(display);
        createContents();
        shell.open();
        shell.layout();
        while (!shell.isDisposed()) {
            if (!display.readAndDispatch())
                display.sleep();
        }
    }

    protected void createContents() {
        shell = new Shell();
        FillLayout fl = new FillLayout(SWT.VERTICAL);        
        shell.setLayout(fl );
        
        tabFolder = new TabFolder(shell, SWT.BORDER);
        TabItem tabItem = new TabItem(tabFolder, SWT.NULL);
        tabItem.setText("Download ");
        
        Composite composite = new Composite(tabFolder, SWT.NONE);
        GridLayout layout = new GridLayout();
        composite.setLayout(layout);
        tabItem.setControl(composite);
        
        tabItem = new TabItem(tabFolder, SWT.NULL);
        tabItem.setText("options");
        OptionsTab opt= new OptionsTab(tabFolder, (int)SWT.NULL);
        tabItem.setControl(opt);
        
        layout.numColumns = 2;
        layout.makeColumnsEqualWidth = false;
      
        urlBtn = new Button(composite, SWT.NULL);
        urlBtn.setText("URL...");
        urlBtn.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
        urlBtn.addSelectionListener(this);
        
//        Label label = new Label(composite, SWT.NULL);
//        label.setText("URL: ");
//        label.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
        
        GridData data = new GridData(GridData.FILL_BOTH);
        text = new Text(composite, SWT.BORDER);
        text.setLayoutData(data);
        
        dst = new Button(composite, SWT.NULL);
        dst.setText("Destination...");
        dst.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
        dst.addSelectionListener(this);
        
        data = new GridData(GridData.FILL_HORIZONTAL);
        pathtext = new Text(composite, SWT.MULTI | SWT.WRAP | SWT.BORDER);
        pathtext.setLayoutData(data);
        
        Label label_ignore = new Label(composite, SWT.NULL);
        label_ignore.setText("Block: ");
        label_ignore.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
        
        ignorelist = new Text(composite, SWT.BORDER);
        data = new GridData(GridData.FILL_BOTH);
        data.horizontalSpan = 1;
        data.verticalSpan = 1;
        data.horizontalAlignment = GridData.FILL;
        ignorelist.setLayoutData(data);
        
        Label label_include = new Label(composite, SWT.NULL);
        label_include.setText("Include: ");
        label_include.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
        
        
        includelist = new Text(composite, SWT.BORDER);
        data = new GridData(GridData.FILL_BOTH);
        data.horizontalSpan = 1;
        data.verticalSpan = 1;
        data.horizontalAlignment = GridData.FILL;
        includelist.setLayoutData(data);    
        
        
        button = new Button(composite, SWT.NONE);
        button.addSelectionListener(this);
        button.setText("Download");
        button.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
        
        clearLogBtn = new Button(composite, SWT.NONE);
        clearLogBtn.setText("Clear log");
        clearLogBtn.addSelectionListener(this);
        button.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));

        logWnd = new Text(composite,SWT.MULTI | SWT.WRAP| SWT.BORDER);
        data = new GridData(GridData.FILL_BOTH);
        data.horizontalSpan = 2;
        data.verticalSpan = 3;
        data.horizontalAlignment = GridData.FILL;
        logWnd.setLayoutData(data);   
//        shell.pack(); 
        shell.open(); 
        shell.setText("SWT Application");
        shell.setSize(600, 260);
    }
    public void println(String msg)
    {
    	logWnd.append(msg);
    }
	public void widgetSelected(final SelectionEvent e) {
		if (e.getSource().equals(button))
		{
			OnDownload();
		}
		if (e.getSource()==(dst))
		{
			onChooseDestination();
		}
		if (e.getSource()==clearLogBtn)
		{
			logWnd.setText("");
		}
		if (e.getSource()==urlBtn)
		{
	        TextTransfer transfer = TextTransfer.getInstance();
	        String data = (String) cb.getContents(transfer);
	        if (data != null) {
	          text.setText(data);
	        }
		}
	}

	private void onChooseDestination() {
		DirectoryDialog dlg = new DirectoryDialog(shell);
		dlg.setFilterPath(text.getText());
		dlg.setText("Save to");
		dlg.setMessage("Select a directory");
		String dir = dlg.open();
		if (dir != null) {
			pathtext.setText(dir);
			destination_path = dir;
		}
	}
	void OnDownload()
	{
		if (DownloadThread.running)
		{
			System.out.println("Another thread is still running.");
			return;
		}
		destination_path = pathtext.getText();
		if (destination_path.length()>0)
		{
			File dest_dir = new File(destination_path);
			if (dest_dir.exists()==false)
			{
				boolean success = dest_dir.mkdirs();
				if (!success)
				{
					System.out.println("Failed to create directory - " + destination_path);
					success = dest_dir.mkdirs();
					{
						System.out.println("Failed to create directory - " + destination_path);
						return;
					}					
				}

			}
		}
		if (0 < text.getText().length()) {
			button.setEnabled(false);
			DownloadThread dt = new DownloadThread(text.getText(), pathtext.getText());
			dt.button = button;
			dt.start();
		}
	}
	public String getDestination()
	{
		return destination_path;
	}
	String destination_path;
}

class DownloadThread extends Thread {
	static boolean running;
	String name;
	String dst;
	Button button;
	public DownloadThread(String n, String dt) {
		name = n;
		dst = dt;
	}
	public void run() {
		running = true;
		try {
			Application.parsePage(name);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (BadLocationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		running = false;
		Application.println("Download " + name + " finished.");
		Display.getDefault().syncExec(
				  new Runnable() {
				    public void run(){
						if (button != null)
							button.setEnabled(true);
				    }
				  });

	}
}
