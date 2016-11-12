import java.awt.Component;

import javax.swing.DefaultListCellRenderer;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.SwingConstants;

// class to force right alignment in JComboBoxes
class ListCellRenderer extends DefaultListCellRenderer
{
	protected int align;
	
	public ListCellRenderer()
	{
		this(SwingConstants.LEFT);
	}
	
	public ListCellRenderer(int align)
	{
		super();
		this.align = align;
	}
	
	public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean cellHasFocus)
	{
		JLabel lbl = (JLabel)super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);
		lbl.setHorizontalAlignment(align);
		return lbl;
	}
}