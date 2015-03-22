import java.util.*;
public class BestFit 
{
	public static LinkedList<Process> queue = new LinkedList<Process>();
	public static LinkedList<Process> segments = new LinkedList<Process>();
	public BestFit() {
		//begin init
		for(int i = 0; i < 200; i++) {
			queue.addLast(new Process(i));
		}
		int index = 0;
		int last = 0;
		/*while(index < 100) {
			Process p = queue.removeFirst();
			System.out.println(p.name + " " + p.size + " " + p.duration);
			if(index + p.size < 100) {
				segments.addLast(p);
				index += p.size;
			} else {
				//index++;
				//last++;
				index += p.size;
				last = p.size;
			}
		}*/
		Process hole = new Process(0);
		hole.name = '.';
		hole.size = 100;
		hole.duration = 0;
		segments.addLast(hole);

		for(int i = 0; i < 10; i++) {
			System.out.println(queue.get(i).name + " " + queue.get(i).size + " " + queue.get(i).duration);
		}
			for(int j = 0; j < segments.size(); j++) {
				for(int k = 0; k < segments.get(j).size; k++) {
					System.out.print(segments.get(j).name);
				}
			}
			System.out.println();
			System.out.println("NUM SEGMENTS: " + segments.size());
		for(int i = 0; i < 1; i++) { //for each time quanta to 60
			for(int j = 0; j < segments.size(); j++) {
				System.out.println("before swap: " + segments.get(j).name + " " + segments.get(j).size + " " + segments.get(j).duration);
			}	
			for(int j = 0; j < segments.size(); j++) { //decrement/remove 
				if(segments.get(j).duration > 0)
					segments.get(j).duration = segments.get(j).duration - 1;
				if(segments.get(j).duration <= 0 && segments.get(j).name != '.') {
					System.out.println("Process " + segments.get(j).name + " swapped out.");
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
			for(int j = 0; j < segments.size(); j++) {
				System.out.println("after swap: " + segments.get(j).name + " " + segments.get(j).size + " " + segments.get(j).duration);
			}	
			for(int j = 0; j < segments.size(); j++) {
				for(int k = 0; k < segments.get(j).size; k++) {
					System.out.print(segments.get(j).name);
				}
			}
			System.out.println();
			//int size = segments.size();
			int smallest = 0;
			int smallestindex = 0;
			int temp = 0;
			for(int j = 0; j < segments.size(); j++) { //first hole
				Process p = queue.getFirst();
				if(segments.get(j).name == '.' && p.size <= segments.get(j).size) {
					smallestindex = j;
					smallest = segments.get(j).size - p.size;
					System.out.println("index " + smallestindex + " size = " + smallest);
					break;
				}
			}
			for(int j = smallestindex + 1; j < segments.size(); j++) {
				Process p = queue.getFirst();
				if(segments.get(j).name == '.' && p.size <= segments.get(j).size) {
					
					if((segments.get(j).size - p.size) < smallest) {
						smallest = segments.get(j).size - p.size;
						smallestindex = j;
						System.out.println("new smallest index " + smallestindex + " size = " + smallest);
					}
				}
			}
			//Process p = queue.removeFirst();
			//System.out.println("REMOVED PROCESS: " + p.name + " SIZE " + p.size);
			Process p = queue.getFirst();
			if(segments.get(smallestindex).size >= p.size) {
				p = queue.removeFirst();
				System.out.println("REMOVED PROCESS: " + p.name + " SIZE " + p.size);
				Process newHole = new Process(0);
				newHole.name = '.';
				newHole.size = segments.get(smallestindex).size - p.size;
				newHole.duration = 0;
				segments.remove(smallestindex);
				segments.add(smallestindex, p);
				//segments.set(smallestindex, p);
				segments.add(smallestindex+1, newHole);
				System.out.println("Process " + p.name + " swapped in.");
				
			}
			for(int j = 0; j < segments.size(); j++) {
				for(int k = 0; k < segments.get(j).size; k++) {
					System.out.print(segments.get(j).name);
				}
			}
			System.out.println();
			/*for(int j = 0; j < segments.size(); j++) { //swap in new process
				Process p = queue.getFirst();	
				if(segments.get(j).name == '.' && p.size <= segments.get(j).size) {
					if(segments.get(j).size == p.size) { //best fit
						queue.removeFirst();
						segments.set(j, p);
						System.out.println("Process " + p.name + " swapped in");
					} 
				}
			}*/
			//merge all holes
			System.out.println("BEFORE MERGING: " + segments.size());
			for(int j = 0; j < segments.size(); j++) {
				System.out.println("before merging: " + segments.get(j).name + " " + segments.get(j).size + " " + segments.get(j).duration);
			}	
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
			System.out.println("AFTER MERGING: " + segments.size());
			
			for(int j = 0; j < segments.size(); j++) {
				System.out.println("after merge: " + segments.get(j).name + " " + segments.get(j).size + " " + segments.get(j).duration);
			}	
		}
	}

}
