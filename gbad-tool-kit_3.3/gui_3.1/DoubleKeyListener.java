import java.awt.Color;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JTextField;

class DoubleKeyListener implements KeyListener
{
	public double min;
	public double max;
	private String toolTipText;
	private String exception;
	
	public DoubleKeyListener(String toolTipText)
	{
		this(toolTipText, Double.MIN_VALUE, Double.MAX_VALUE);
	}
	
	public DoubleKeyListener(String toolTipText, double min, double max)
	{
		this(toolTipText, min, max, null);
	}
	
	public DoubleKeyListener(String toolTipText, double min, double max, String exception)
	{
		this.min = min;
		this.max = max;
		this.toolTipText = toolTipText;
		this.exception = exception;
	}
	
	public void keyPressed(KeyEvent e)
	{
	
	}
	
	public void keyReleased(KeyEvent e)
	{
		JTextField textField = (JTextField)e.getSource();
		
		String num = textField.getText();
		
		if(!num.equals(""))
		{
			try
			{
				if((exception == null) || !exception.equals(num))
				{
					double value = Double.parseDouble(num);
					
					if(value < min)
					{
						throw new NumberFormatException(null);
					}
					else if(value > max)
					{
						throw new NumberFormatException(null);
					}
				}
				
				textField.setBackground(new Color(255, 255, 255));
				textField.setToolTipText(toolTipText);
			}
			catch(NumberFormatException nfe)
			{
				textField.setBackground(new Color(255, 255, 190));
				
				if((min == Double.MIN_VALUE) && (max == Double.MAX_VALUE))
				{
					textField.setToolTipText("Value must be a real number.");	
				}
				else if(min == Double.MIN_VALUE)
				{
					textField.setToolTipText("Value must be a real number less than or equal to " + max + ".");
				}
				else if(max == Double.MAX_VALUE)
				{
					textField.setToolTipText("Value must be a real number greater than or equal to " + min + ".");
				}
				else
				{
					textField.setToolTipText("Value must be a real number between " + min + " " + max + ".");
				}
			}
		}
	}
		
	public void keyTyped(KeyEvent e)
	{
		
	}
}