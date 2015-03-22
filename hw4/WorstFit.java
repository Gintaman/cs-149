import java.util.*;
public class WorstFit 
{
	private static LinkedList<Process> queue = new LinkedList<Process>();
	private static LinkedList<Process> segments = new LinkedList<Process>();
	
	public int swaps = 0;
	public WorstFit() {
		//begin init
		for(int i = 0; i < 300; i++) {
			queue.addLast(new Process(i));
		}
		int index = 0;
		int last = 0;

		Process hole = new Process(0);
		hole.setName('.');
		hole.setSize(100);
		hole.setDuration(0);
		segments.addLast(hole);

		for(int i = 0; i < 60; i++) { //for each time quanta to 60
			for(int j = 0; j < segments.size(); j++) { //decrement/remove 
				if(segments.get(j).duration() > 0)
					segments.get(j).setDuration(segments.get(j).duration() - 1);
				if(segments.get(j).duration() <= 0 && segments.get(j).name() != '.') {
					System.out.println("Time " + i + ": Process " + segments.get(j).name() + " swapped out.");
					hole = new Process(0);
					hole.setName('.');
					hole.setSize(segments.get(j).size());
					hole.setDuration(0);
					segments.set(j, hole);
				}
			}
			//merge
			for(int j = 0; j < segments.size() - 1; j++) {
				if(segments.get(j).name() == '.') {
					if(segments.get(j + 1).name() == '.') {
						Process newHole = new Process(0);
						newHole.setName('.');
						newHole.setSize(segments.get(j).size() + segments.get(j+1).size());
						newHole.setDuration(0);
						segments.remove(j);
						segments.set(j, newHole);
						//segments.add(j, newHole);	
					}
				}
			}
			
			//int size = segments.size();

			//START SWAPPING IN PROCESSES
			//
			//
			//
			//

			int largest = 0;
			int largestindex = 0;
			int temp = 0;
			index = 0;
			Process p = queue.getFirst();
			//while(index + queue.getFirst().size() < 100) {
			
			while(index < 100) {

				for(int j = 0; j < segments.size(); j++) { //first hole
					Process pp = queue.getFirst();
					if(segments.get(j).name() == '.' && pp.size() <= segments.get(j).size()) {
						largestindex = j;
						largest = segments.get(j).size() - pp.size();
						break;
					}
				}
				for(int j = largestindex + 1; j < segments.size(); j++) {
					Process pp = queue.getFirst();
					if(segments.get(j).name() == '.' && pp.size() <= segments.get(j).size()) {
						
						if((segments.get(j).size() - pp.size()) > largest) {
							largest = segments.get(j).size() - pp.size();
							largestindex = j;
						}
					}
				}
				index += largestindex + queue.getFirst().size();
				//Process p = queue.removeFirst();
				//System.out.println("REMOVED PROCESS: " + p.name() + " SIZE " + p.size());
				p = queue.getFirst();
				if(segments.get(largestindex).size() >= p.size()) {
					p = queue.removeFirst();
					Process newHole = new Process(0);
					newHole.setName('.');
					newHole.setSize(segments.get(largestindex).size() - p.size());
					newHole.setDuration(0);
					segments.remove(largestindex);
					segments.add(largestindex, p);
					//segments.set(largestindex, p);
					segments.add(largestindex+1, newHole);
					System.out.println("Time " + i + ": Process " + p.name() + " swapped in.");
					swaps++;
					index += largestindex;
					
				}

				//print
				

				//merge holes
				for(int j = 0; j < segments.size() - 1; j++) {
					if(segments.get(j).name() == '.') {
						if(segments.get(j + 1).name() == '.') {
							Process newHole = new Process(0);
							newHole.setName('.');
							newHole.setSize(segments.get(j).size() + segments.get(j+1).size());
							newHole.setDuration(0);
							segments.remove(j);
							segments.set(j, newHole);
						}
					}
				}
			} //end while
			System.out.print("Time " + i + ": ");
			for(int j = 0; j < segments.size(); j++) {
				for(int k = 0; k < segments.get(j).size(); k++) {
					System.out.print(segments.get(j).name());
				}
			}
			System.out.println();
		}
		segments.clear();
		queue.clear();
	}
	public int getAvg() {
		return swaps;
	}
}
