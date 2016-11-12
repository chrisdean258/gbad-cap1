import java.awt.Color;
import java.awt.Font;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import java.io.File;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JComboBox;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.SwingConstants;

import javax.swing.border.TitledBorder;

import javax.swing.event.EventListenerList;

import java.util.List;


/*
This class handels the layout of the Parameters section of the GUI.
*/
public class Parameters extends JPanel implements ActionListener, ItemListener, ParameterInterface
{
	private JComboBox discoveryComboBox;
	private String[] discoveryDetectionValues = {"gbad-mdl", "gbad-fsm"};
	private String discoveryDetectionDescription = "Subgraph discovery engine.";
	
	MDL mdl;
	FSM fsm;
	
	EventListenerList listenerList;
	
	public Parameters()
	{
		super();
		
		listenerList = new EventListenerList();
		
		setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.blue), "Parameters", TitledBorder.LEFT, TitledBorder.TOP, new Font("Comic Sans MS", Font.BOLD, 15), Color.red));
		
		addContent();
	}
	
	// create and add all content to this jpanel
	private void addContent()
	{
		//JPanel content = new JPanel(new java.awt.BorderLayout());
		JPanel content = new JPanel();
		content.setLayout(new BoxLayout(content, BoxLayout.Y_AXIS));
		
		mdl = new MDL();
		mdl.addActionListener(this);
		
		fsm = new FSM();
		fsm.addActionListener(this);
		
		fsm.setVisible(false);
		
		discoveryComboBox = new JComboBox(discoveryDetectionValues);
		discoveryComboBox.setRenderer(new ListCellRenderer(SwingConstants.LEFT));
		discoveryComboBox.setToolTipText(discoveryDetectionDescription);
		discoveryComboBox.addItemListener(this);
		
		content.add(discoveryComboBox);
		content.add(new JSeparator());
		content.add(mdl);
		content.add(fsm);
		
		JScrollPane scroller = new JScrollPane(content);
		
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		add(scroller);
	}
	
	public String getAnomalyAlgorithm()
	{
		if(discoveryComboBox.getSelectedItem().toString().equals("gbad-mdl"))
		{
			return mdl.getAnomalyAlgorithm();
		}
		else
		{
			return fsm.getAnomalyAlgorithm();
		}
	}
	
	public List<String> getParameters(File inputFile, String append, String resultsDir) throws NumberFormatException
	{
		if(discoveryComboBox.getSelectedItem().toString().equals("gbad-mdl"))
		{
			return mdl.getParameters(inputFile, append, resultsDir);
		}
		else
		{
			return fsm.getParameters(inputFile, append, resultsDir);
		}
	}
	
	public void hideParameters(boolean visible)
	{
		mdl.hideParameters(visible);
		fsm.hideParameters(true);
	}
	
	public void actionPerformed(ActionEvent e)
	{
		e.setSource(this);
		fireActoinEvent(e);
	}
	
	public void itemStateChanged(ItemEvent evt)
	{
		String item = (String)evt.getItem();
		
		if(item.equals("gbad-mdl"))
		{
			mdl.setVisible(true);
			fsm.setVisible(false);
		}
		else if(item.equals("gbad-fsm"))
		{
			mdl.setVisible(false);
			fsm.setVisible(true);
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
