package oz.html;

import java.util.Properties;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Widget;

public class OptionsTab extends Composite implements SelectionListener {
	private Text txt_min_img_width;
	private Text txt_min_img_height;
	private Text txt_http_proxy_host;
	private Text txt_http_proxy_port;
	private Button setProxyBtn;
	public OptionsTab(Widget parent, int style) {
		super((Composite) parent, style);
		GridLayout layout = new GridLayout();
		setLayout(layout);
		layout.numColumns = 2;
        layout.makeColumnsEqualWidth = false;
        {
        Label label = new Label(this, SWT.NULL);
        label.setText("Min width: ");
        label.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
        
        GridData data = new GridData(GridData.BEGINNING);
        txt_min_img_width = new Text(this, SWT.BORDER);
        txt_min_img_width.setLayoutData(data);
        }
        {
        Label label2 = new Label(this, SWT.NULL);
        label2.setText("Min height: ");
        label2.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
        
        GridData data = new GridData(GridData.BEGINNING);
        txt_min_img_height = new Text(this, SWT.BORDER);
        txt_min_img_height.setLayoutData(data);
        }
        {
            Label label2 = new Label(this, SWT.NULL);
            label2.setText("Http proxy host: ");
            label2.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
            
            GridData data = new GridData(GridData.BEGINNING);
            txt_http_proxy_host = new Text(this, SWT.BORDER);
            txt_http_proxy_host.setLayoutData(data);
            }
        {
            Label label2 = new Label(this, SWT.NULL);
            label2.setText("Http proxy port: ");
            label2.setLayoutData(new GridData(GridData.VERTICAL_ALIGN_BEGINNING));
            
            GridData data = new GridData(GridData.BEGINNING);
            txt_http_proxy_port = new Text(this, SWT.BORDER);
            txt_http_proxy_port.setLayoutData(data);
            }
        {
        setProxyBtn = new Button(this, SWT.NULL);
        setProxyBtn.setText("OK");
        setProxyBtn.addSelectionListener(this);
        GridData data = new GridData(GridData.BEGINNING);
        setProxyBtn.setLayoutData(data);
        }
	}
	@Override
	public void widgetDefaultSelected(SelectionEvent arg0) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public void widgetSelected(SelectionEvent e) {
		if (e.getSource()==setProxyBtn)
		{
			Properties props = System.getProperties();
			props.put("http.proxyHost", txt_http_proxy_host.getText());
			props.put("http.proxyPort", txt_http_proxy_port.getText());	
			System.out.println("Set proxy");
		}		
	}
}
