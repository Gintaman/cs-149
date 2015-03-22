import java.util.Random;

public class Process
{
	private int size;
	private int duration;
	private char name;
	
	public Process(int name) {
		Random rand = new Random();
		this.name = (char) ((name % 26) + 65);
		this.size = rand.nextInt(4);
		switch(this.size) {
			case 0: this.size = 5;
				break;
			case 1: this.size = 11;
				break;
			case 2: this.size = 17;
				break;
			case 3: this.size = 33;
				break;
		}
		this.duration = rand.nextInt(5) + 1;
	}
	public int duration() {
		return this.duration;
	}
	public int size() {
		return this.size;
	}
	public char name() {
		return this.name;
	}
	public void setSize(int size) {
		this.size = size;
	}
	public void setName(char name) {
		this.name = name;
	} 
	public void setDuration(int duration) {
		this.duration = duration;
	}
}


