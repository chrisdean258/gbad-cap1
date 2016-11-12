import java.awt.Color;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JTextField;

class IntegerKeyListener implements KeyListener
{
	public int min;
	public int max;
	private String toolTipText;
	private String exception;
	
	public IntegerKeyListener(String toolTipText)
	{
		this(toolTipText, Integer.MIN_VALUE, Integer.MAX_VALUE);
	}
	
	public IntegerKeyListener(String toolTipText, int min, int max)
	{
		this(toolTipText, min, max, null);
	}
	
	public IntegerKeyListener(String toolTipText, int min, int max, String exception)
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
					int value = Integer.parseInt(num);
				
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
				
				if((min == Integer.MIN_VALUE) && (max == Integer.MAX_VALUE))
				{
					textField.setToolTipText("Value must be a integer.");	
				}
				else if(min == Integer.MIN_VALUE)
				{
					textField.setToolTipText("Value must be a integer less than or equal to " + max + ".");
				}
				else if(max == Integer.MAX_VALUE)
				{
					textField.setToolTipText("Value must be a integer greater than or equal to " + min + ".");
				}
				else
				{
					textField.setToolTipText("Value must be a integer between " + min + " " + max + ".");
				}
			}
		}
	}
		
	public void keyTyped(KeyEvent e)
	{
		
	}
}