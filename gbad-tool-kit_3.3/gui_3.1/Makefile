JC = javac
JC_FLAG = -source 1.7 -target 1.7

JR = jar
JR_FLAG = -cvmf
MANIFEST = manifest.txt

SRC = CustomFileFilter.java DoubleKeyListener.java DraggableJScrollPane.java FSM.java GBADMenu.java GUI.java GraphDisplay.java GraphFileFilter.java GraphResults.java GraphText.java GraphVisualization.java Images.java IntegerKeyListener.java JGraphViz.java ListCellRenderer.java MDL.java Main.java ParameterInterface.java Parameters.java Preferences.java SliderModel.java StartProcess.java UpdateGBADCount.java

OBJ = CancelActionListener.class CreateGraphTask.class CustomFileFilter.class DoubleKeyListener.class DraggableJScrollPane.class FSM.class GBADMenu.class GBADProcessMonitor.class GUI.class GraphDisplay.class GraphFileFilter.class GraphResults.class GraphText.class GraphVisualization.class Images.class IntegerKeyListener.class JGraphViz.class ListCellRenderer.class MDL.class Main.class ParameterInterface.class Parameters.class PathMouseListener.class Preferences.class ResizeGraphTask.class RestoreDefaultsActionListener.class RestoreNamedDefaultActionListener.class SaveActionListener.class SliderModel.class StartProcess.class UpdateGBADCount.class

PROG = GUI.jar

all: $(PROG)

$(PROG): $(OBJ)
	$(JR) $(JR_FLAG) $(MANIFEST) $(PROG) $(SRC) $(OBJ)
	
$(OBJ): $(SRC)
	$(JC) $(JC_FLAG) $(SRC)

clean:
	rm -f $(OBJ)

tar:
	tar -czvf GUI.tar.gz ../gbad_gui/*.java ../gbad_gui/Makefile ../gbad_gui/manifest.txt ../gbad_gui/graph.g ../gbad_gui/mdl.g  ../gbad_gui/mps.g  ../gbad_gui/norm.g  ../gbad_gui/prob.g ../gbad_gui/bin/
