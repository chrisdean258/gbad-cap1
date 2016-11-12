import java.awt.Component;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import java.io.File;

import javax.swing.Box;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import javax.swing.border.TitledBorder;

import javax.swing.event.EventListenerList;

import java.util.List;
import java.util.Vector;


/*
This class handels the layout of the FSM parameters section of the GUI.
*/
public class FSM extends JPanel implements ActionListener, ItemListener, ParameterInterface
{
	private JComboBox anomalousDetectionComboBox;
	private String[] anomalousDetectionValues = {"None", "-mdl", "-mps", "-prob"};
	private String anomalousDetectionDescription = "Anomaly detection algorithm used.";
	
	private JLabel mdlLabel;
	private JTextField mdlTextField;
	private String mdlDefault = "0.3";
	private String mdlDescription = "Percentage of change allowed (0.0 - 1.0).";
	
	private JLabel mpsLabel;
	private JTextField mpsTextField;
	private String mpsDefault = "0.3";
	private String mpsDescription = "Percentage of change allowed (0.0 - 1.0).";
	
	private JLabel mstLabel;
	private JTextField mstTextField;
	private String mstDefault = "1";
	private String mstDescription = "See GBAD manual.";
	
	private JLabel subInstOutLabel;
	private JCheckBox subInstOutCheckBox;
	private String subInstOutDescription = "Create frequent substructure instances output file.";
	
	private JLabel bestSubInstOutLabel;
	private JCheckBox bestSubInstOutCheckBox;
	private String bestSubInstOutDescription = "Create best substructure instance file.";
		
	private UpdateGBADCount gbadCountLabel;
	
	private JButton restoreDefaults;
	private String restoreDefaultsText = "Restore Defaults";
	
	private JButton advanced;
	public static final String advancedText = "Advanced >>";
	public static final String simpleText = "<< Simple";
	
	private JButton startGBAD;
	public static final String startGBADText = "Start GBAD";
	
	EventListenerList listenerList;
	
	public FSM()
	{
		super();
		
		listenerList = new EventListenerList();
		
		//setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.blue), "Parameters", TitledBorder.LEFT, TitledBorder.TOP, new Font("Comic Sans MS", Font.BOLD, 15), Color.red));
		
		addContent();

		// hide the advanced parameters
		hideParameters(true);
	}
	
	// create and add all content to this jpanel
	private void addContent()
	{
		JPanel content = new JPanel();
		GridBagLayout gridbag = new GridBagLayout();
		content.setLayout(gridbag);
		
		addAHack(content, gridbag);
		
		anomalousDetectionComboBox = new JComboBox(anomalousDetectionValues);
		anomalousDetectionComboBox.setRenderer(new ListCellRenderer(SwingConstants.RIGHT));
		anomalousDetectionComboBox.setToolTipText(anomalousDetectionDescription);
		addJComboBoxPanel(content, gridbag, anomalousDetectionComboBox);
		
		mdlLabel = new JLabel("deviation");
		mdlTextField = new JTextField(10);
		mdlTextField.addKeyListener(new DoubleKeyListener(mdlDescription, 0.0, 1.0));
		mdlLabel.setToolTipText(mdlDescription);
		mdlTextField.setToolTipText(mdlDescription);
		addJTextFieldPanel(content, gridbag, mdlLabel, mdlTextField);
		mdlLabel.setVisible(false);
		mdlTextField.setVisible(false);
		
		mpsLabel = new JLabel("deviation");
		mpsTextField = new JTextField(10);
		mpsTextField.addKeyListener(new DoubleKeyListener(mpsDescription, 0.0, 1.0));
		mpsLabel.setToolTipText(mpsDescription);
		mpsTextField.setToolTipText(mpsDescription);
		addJTextFieldPanel(content, gridbag, mpsLabel, mpsTextField);
		mpsLabel.setVisible(false);
		mpsTextField.setVisible(false);
		
		anomalousDetectionComboBox.addItemListener(this);
		
		mstLabel = new JLabel("-mst");
		mstTextField = new JTextField(10);
		mstTextField.addKeyListener(new IntegerKeyListener(mstDescription, 1, Integer.MAX_VALUE));
		mstLabel.setToolTipText(mstDescription);
		mstTextField.setToolTipText(mstDescription);
		addJTextFieldPanel(content, gridbag, mstLabel, mstTextField);
		
		subInstOutLabel = new JLabel("-subInstOut");
		subInstOutCheckBox = new JCheckBox("");
		subInstOutLabel.setToolTipText(subInstOutDescription);
		subInstOutCheckBox.setToolTipText(subInstOutDescription);
		addJCheckBoxPanel(content, gridbag, subInstOutLabel, subInstOutCheckBox);
		
		bestSubInstOutLabel = new JLabel("-bestSubInstOut");
		bestSubInstOutCheckBox = new JCheckBox("");
		bestSubInstOutLabel.setToolTipText(bestSubInstOutDescription);
		bestSubInstOutCheckBox.setToolTipText(bestSubInstOutDescription);
		addJCheckBoxPanel(content, gridbag, bestSubInstOutLabel, bestSubInstOutCheckBox);
		
		/*
		// the following code reads in an image and prints out the raw
		// data as a java array
		
		
		byte[] data = new byte[0];
		try
		{
			FileInputStream reader = new FileInputStream("gbad.png");
			data = new byte[reader.available()];
			reader.read(data);
		}
		catch(Exception e)
		{
		
		}
		
		System.out.print("{");
		for(int i=0; i < data.length; i++)
		{
			System.out.print(data[i] + ",");
		}
		System.out.println("}");
		*/

		gbadCountLabel = new UpdateGBADCount();
		(new Thread(gbadCountLabel)).start();
		
		restoreDefaults();
		
		restoreDefaults = new JButton(restoreDefaultsText);
		restoreDefaults.setActionCommand(restoreDefaultsText);
		restoreDefaults.addActionListener(this);
		
		advanced = new JButton(advancedText);
		advanced.setActionCommand(advancedText);
		advanced.addActionListener(this);
		
		startGBAD = new JButton(startGBADText);
		startGBAD.setActionCommand(startGBADText);
		startGBAD.addActionListener(this);
		
		JButton[] buttons = {restoreDefaults, advanced, startGBAD};
		addJButtonPanel(content, gridbag, gbadCountLabel, buttons);
		
		//JScrollPane scroller = new JScrollPane(content);
		
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		add(content);
		
	}
	
	// this function restores the default values of every parameter
	private void restoreDefaults()
	{
		anomalousDetectionComboBox.setSelectedIndex(0);
		mdlTextField.setText(mdlDefault);
		mpsTextField.setText(mpsDefault);
		mstTextField.setText(mstDefault);
		subInstOutCheckBox.setSelected(false);
		bestSubInstOutCheckBox.setSelected(false);
		
		mstTextField.setBackground(new Color(255, 255, 255));
	}
	
	/*
	Add an invisible component to the layout that should be larger than
	any other component.  This forces a consistent minimum width no matter
	what is currently being displayed.
	*/
	private void addAHack(JPanel content, GridBagLayout gridbag)
	{
		GridBagConstraints c = new GridBagConstraints();
		c.insets = new Insets(0,1,5,1);
		
		Component aHack = Box.createRigidArea(new Dimension(275, 0));
		
		c.anchor = GridBagConstraints.CENTER;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.weightx = 1.0;
		gridbag.setConstraints(aHack, c);
		content.add(aHack);
	}
	
	// Add an input JTextField and a parameter description JLabel to the content.
	private void addJTextFieldPanel(JPanel content, GridBagLayout gridbag, JLabel label, JTextField textField)
	{
		GridBagConstraints c = new GridBagConstraints();
		c.insets = new Insets(0,1,5,1);
		
		c.anchor = GridBagConstraints.WEST;
		c.fill = GridBagConstraints.NONE;
		c.weightx = 0.0;
		gridbag.setConstraints(label, c);
		content.add(label);
		
		c.anchor = GridBagConstraints.WEST;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.weightx = 1.0;
		gridbag.setConstraints(textField, c);
		content.add(textField);

	}
	
	// Add an input JCheckBox and a parameter description JLabel to the content.
	private void addJCheckBoxPanel(JPanel content, GridBagLayout gridbag, JLabel label, JCheckBox checkBox)
	{
		GridBagConstraints c = new GridBagConstraints();
		c.insets = new Insets(0,1,5,1);
		
		c.anchor = GridBagConstraints.WEST;
		c.fill = GridBagConstraints.NONE;
		c.weightx = 0.0;
		gridbag.setConstraints(label, c);
		content.add(label);
		
		c.anchor = GridBagConstraints.WEST;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.weightx = 1.0;
		gridbag.setConstraints(checkBox, c);
		content.add(checkBox);
	}
	
	// Add an input JComboxBox to the content.
	private void addJComboBoxPanel(JPanel content, GridBagLayout gridbag, JComboBox comboBox)
	{
		GridBagConstraints c = new GridBagConstraints();
		c.insets = new Insets(0,1,5,1);
		
		c.anchor = GridBagConstraints.CENTER;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.weightx = 1.0;
		gridbag.setConstraints(comboBox, c);
		content.add(comboBox);
	}
	
	/*
	This function sets up the bottom of the parameters jpanel.  It adds, in order,
	the JLabel, and then each of the buttons to the content.  The label and the buttons
	will always be found on the last few rows of the girdbag layout.
	*/
	private void addJButtonPanel(JPanel content, GridBagLayout gridbag, JLabel label, JButton[] buttons)
	{
		JPanel panel = new JPanel();
		GridBagLayout tempBag = new GridBagLayout();
		panel.setLayout(tempBag);
		
		GridBagConstraints c = new GridBagConstraints();
		c.insets = new Insets(2,2,2,2);
		
		c.anchor = GridBagConstraints.WEST;
		c.fill = GridBagConstraints.NONE;
		c.gridwidth = GridBagConstraints.REMAINDER;
		
		tempBag.setConstraints(label, c);
		panel.add(label);
		
		c.anchor = GridBagConstraints.WEST;
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.weightx = 1.0;
		
		for(int index=0; index<buttons.length; index++)
		{
			tempBag.setConstraints(buttons[index], c);
			panel.add(buttons[index]);
		}
		
		c.anchor = GridBagConstraints.SOUTHWEST;
		c.fill = GridBagConstraints.NONE;
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.weightx = 0.0;
		c.gridheight = GridBagConstraints.REMAINDER;
		c.weighty = 1.0;
		
		gridbag.setConstraints(panel, c);
		content.add(panel);
	}
	
	// Set any advanced parameters to the specified visibility.
	public void hideParameters(boolean visible)
	{
		subInstOutLabel.setVisible(!visible);
		subInstOutCheckBox.setVisible(!visible);
		
		bestSubInstOutLabel.setVisible(!visible);
		bestSubInstOutCheckBox.setVisible(!visible);
	}
	
	/*
	Try to read an integer out of the JTextField and paramString follows by the 
	integer to args.  If the text is not a valid integer a NumberFormatException
	is thrown.  
	*/
	private void addTextFieldIntParam(List<String> args, JTextField field, String paramString, String defaultString) throws NumberFormatException
	{
		try
		{
			int value = Integer.parseInt(field.getText());
			
			if(value < 0)
			{
				throw new NumberFormatException("");
			}
			
			args.add(paramString);
			args.add(field.getText());
		}
		catch(NumberFormatException nfe)
		{
			String value = field.getText();
			
			if(value.trim().equals(""))
			{
				value = "";
			}
			
			throw new NumberFormatException(paramString + " parameter (" + value + ") is not a valid positive integer.\nThe default for this parameter is " + defaultString + ".");
		}
	}
	
	/*
	Try to read a number between 0.0 and 1.0 out of the JTextField and paramString
	follows by the integer to args.  If the text is not a valid integer a
	NumberFormatException is thrown.  
	*/
	private void addTextFieldDoubleParam(List<String> args, JTextField field, String paramString, String defaultString) throws NumberFormatException
	{
		try
		{
			double value = Double.parseDouble(field.getText());
			
			if((value < 0.0) || (value > 1.0))
			{
				throw new NumberFormatException("");
			}
			
			args.add(paramString);
			args.add(field.getText());
		}
		catch(NumberFormatException nfe)
		{
			String value = field.getText();
			
			if(value.trim().equals(""))
			{
				value = "";
			}
			
			throw new NumberFormatException(paramString + " parameter (" + value + ") is not valid.\nThe default for this parameter is " + defaultString + ".");
		}
	}
	
	/*
	If checkBox is checked, add paramString to args.
	*/
	private void appendCheckBoxParam(List<String> args, JCheckBox checkBox, String paramString)
	{
		if(checkBox.isSelected())
		{
			args.add(paramString);
		}
	}
	
	public String getAnomalyAlgorithm()
	{
		String alg = anomalousDetectionComboBox.getSelectedItem().toString();
		
		if(alg.equals("-mdl"))
		{
			return ".fsm.mdl";
		}
		else if(alg.equals("-mps"))
		{
			return ".fsm.mps";
		}
		else if(alg.equals("-prob"))
		{
			return ".fsm.prob";
		}
		else
		{
			return ".fsm";
		}
	}
	
	/*
	Return the selected item out of the anomalousDetectionComboBox.
	*/
	protected void addAnomalyAlgorithm(List<String> args) throws NumberFormatException
	{
		String selected = anomalousDetectionComboBox.getSelectedItem().toString();
		
		if(!selected.equals(anomalousDetectionValues[0]))
		{
			if(selected.equals("-mdl"))
			{
				addTextFieldDoubleParam(args, mdlTextField, "-mdl", mdlDefault);
			}
			else if(selected.equals("-mps"))
			{
				addTextFieldDoubleParam(args, mpsTextField, "-mps", mpsDefault);
			}
		}
		else
		{
			args.add("-phase");
			args.add("1");
		}
	}
	
	/*
	returns a List of all the user defined parameters needed to started bad
	*/
	public List<String> getParameters(File inputFile, String append, String resultsDir) throws NumberFormatException
	{
		List<String> args = new Vector<String>();
		
		String gbadEXE;
		
		try
		{
			gbadEXE = System.getProperty("gbad-fsm");
			if(gbadEXE == null)
			{
				throw new SecurityException();
			}
		}
		catch(SecurityException se)
		{
			gbadEXE = "gbad-fsm";
			JOptionPane.showMessageDialog(this, "Unable to determine location of gbad-fsm.  Assuming in path.", "Security Exception", JOptionPane.ERROR_MESSAGE);
		}
				
		args.add(gbadEXE);
		
		addAnomalyAlgorithm(args);
		
		addTextFieldIntParam(args, mstTextField, "-mst", mstDefault);
		
		if(subInstOutCheckBox.isSelected() || anomalousDetectionComboBox.getSelectedItem().toString().equals(anomalousDetectionValues[0]))
		{
			args.add("-subInstOut");
			args.add("-nameBestSubG");
			args.add(resultsDir + inputFile.getName() + append + getAnomalyAlgorithm() + "_best_sub.g");
		}
		
		if(bestSubInstOutCheckBox.isSelected())
		{
			args.add("-bestSubInstOut");
			args.add("-nameBestSubINST");
			args.add(resultsDir + inputFile.getName() + append + getAnomalyAlgorithm() + "_best_sub.inst");
		}
		
		args.add("-nameAnomSub");
		args.add(resultsDir + inputFile.getName() + append + getAnomalyAlgorithm() + "_anom_sub.g");
		
		args.add("-dot");
		args.add(resultsDir + inputFile.getName() + append + getAnomalyAlgorithm() + ".dot");
		
		args.add("-graph");
		args.add(inputFile.toString());
		
		return args;
	}
	
	// handles the advaced/simple, restore defaults, and start gbad buttons
	public void actionPerformed(ActionEvent e)
	{
		if(e.getActionCommand().equals(advancedText))
		{
			advanced.setActionCommand(simpleText);
			advanced.setText(simpleText);
			
			hideParameters(false);
		}
		else if(e.getActionCommand().equals(simpleText))
		{
			advanced.setActionCommand(advancedText);
			advanced.setText(advancedText);
			
			hideParameters(true);
		}
		else if(e.getActionCommand().equals(restoreDefaultsText))
		{
			restoreDefaults();
		}
		else if(e.getActionCommand().equals(startGBADText))
		{
			ActionEvent newEvent = new ActionEvent(this, 0, startGBADText);
			fireActoinEvent(newEvent);
		}
		else
		{
			return;
		}
		
		validate();
		repaint();
	}
	
	public void itemStateChanged(ItemEvent evt)
	{
		String item = (String)evt.getItem();
		
		if(item.equals("-mdl"))
		{
			mdlLabel.setVisible(true);
			mdlTextField.setVisible(true);
			
			mpsLabel.setVisible(false);
			mpsTextField.setVisible(false);
		}
		else if(item.equals("-mps"))
		{
			mdlLabel.setVisible(false);
			mdlTextField.setVisible(false);
			
			mpsLabel.setVisible(true);
			mpsTextField.setVisible(true);
		}
		else if(item.equals("-prob"))
		{
			mdlLabel.setVisible(false);
			mdlTextField.setVisible(false);
			
			mpsLabel.setVisible(false);
			mpsTextField.setVisible(false);
		}
		else
		{
			mdlLabel.setVisible(false);
			mdlTextField.setVisible(false);
			
			mpsLabel.setVisible(false);
			mpsTextField.setVisible(false);
		}
	}
		
	public void addActionListener(ActionListener l)
	{
		listenerList.add(ActionListener.class, l);
	}
	
	public void removeActionListener(ActionListener l)
	{
		listenerList.remove(ActionListener.class, l);
	}
	
	protected void fireActoinEvent(ActionEvent e)
	{
		Object[] listeners = listenerList.getListenerList();
		for (int i = listeners.length-2; i>=0; i-=2)
		{
			if (listeners[i]==ActionListener.class)
			{
				((ActionListener)listeners[i+1]).actionPerformed(e);
			}
		}
	}
}

