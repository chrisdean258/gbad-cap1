import java.io.File;

import java.util.List;

public interface ParameterInterface
{
	public static final String startGBADText = "Start GBAD";
	
	public String getAnomalyAlgorithm();
	
	public List<String> getParameters(File inputFile, String append, String resultsDir) throws NumberFormatException;
	
	public void hideParameters(boolean visible);
}