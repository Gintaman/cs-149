import java.util.*;
public class BestFit 
{
	public static LinkedList<Process> queue = new LinkedList<Process>();
	public static LinkedList<Process> segments = new LinkedList<Process>();
	public BestFit() {
		//begin init
		for(int i = 0; i < 300; i++) {
			queue.addLast(new Process(i));
		}
		int index = 0;
		int last = 0;

		Process hole = new Process(0);
		hole.name = '.';
		hole.size = 100;
		hole.duration = 0;
		segments.addLast(hole);

			
		for(int i = 0; i < 60; i++) { //for each time quanta to 60

			for(int j = 0; j < segments.size(); j++) { //decrement/remove 
				if(segments.get(j).duration > 0)
					segments.get(j).duration = segments.get(j).duration - 1;
				if(segments.get(j).duration <= 0 && segments.get(j).name != '.') {
					System.out.println("Time " + i + ": Process " + segments.get(j).name + " swapped out.");
					hole = new Process(0);
					hole.name = '.';
					hole.size = segments.get(j).size;
					hole.duration = 0;
					segments.set(j, hole);
				}
			}
			//merge
			for(int j = 0; j < segments.size() - 1; j++) {
				if(segments.get(j).name == '.') {
					if(segments.get(j + 1).name == '.') {
						Process newHole = new Process(0);
						newHole.name = '.';
						newHole.size = segments.get(j).size + segments.get(j+1).size;
						newHole.duration = 0;
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

			int smallest = 0;
			int smallestindex = 0;
			int temp = 0;
			index = 0;
			Process p = queue.getFirst();
			//while(index + queue.getFirst().size < 100) {
			
			while(index < 100) {

				for(int j = 0; j < segments.size(); j++) { //first hole
					Process pp = queue.getFirst();
					if(segments.get(j).name == '.' && pp.size <= segments.get(j).size) {
						smallestindex = j;
						smallest = segments.get(j).size - pp.size;
						break;
					}
				}
				for(int j = smallestindex + 1; j < segments.size(); j++) {
					Process pp = queue.getFirst();
					if(segments.get(j).name == '.' && pp.size <= segments.get(j).size) {
						
						if((segments.get(j).size - pp.size) < smallest) {
							smallest = segments.get(j).size - pp.size;
							smallestindex = j;
						}
					}
				}
				index += smallestindex + queue.getFirst().size;
				//Process p = queue.removeFirst();
				//System.out.println("REMOVED PROCESS: " + p.name + " SIZE " + p.size);
				p = queue.getFirst();
				if(segments.get(smallestindex).size >= p.size) {
					p = queue.removeFirst();
					Process newHole = new Process(0);
					newHole.name = '.';
					newHole.size = segments.get(smallestindex).size - p.size;
					newHole.duration = 0;
					segments.remove(smallestindex);
					segments.add(smallestindex, p);
					//segments.set(smallestindex, p);
					segments.add(smallestindex+1, newHole);
					System.out.println("Time " + i + ": Process " + p.name + " swapped in.");
					index += smallestindex;
					
				}

				//print
				

				//merge holes
				for(int j = 0; j < segments.size() - 1; j++) {
					if(segments.get(j).name == '.') {
						if(segments.get(j + 1).name == '.') {
							Process newHole = new Process(0);
							newHole.name = '.';
							newHole.size = segments.get(j).size + segments.get(j+1).size;
							newHole.duration = 0;
							segments.remove(j);
							segments.set(j, newHole);
						}
					}
				}
			} //end while
			System.out.print("Time " + i + ": ");
			for(int j = 0; j < segments.size(); j++) {
				for(int k = 0; k < segments.get(j).size; k++) {
					System.out.print(segments.get(j).name);
				}
			}
			System.out.println();
		}
	}

}
