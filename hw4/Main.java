public class Main 
{
	public static void main(String[] args) {
			System.out.println("BEST FIT ###################################################");
			BestFit b = new BestFit();
			BestFit b2 = new BestFit();
			BestFit b3 = new BestFit();
			BestFit b4 = new BestFit();
			BestFit b5 = new BestFit();
			System.out.println();
			System.out.println("AVERAGE PROCESSES SWAPPED IN OVER 5 RUNS FOR BEST FIT: " + (float) (b.getAvg() + b2.getAvg() + b3.getAvg() + b4.getAvg() + b5.getAvg()) / 5);
			System.out.println();
			System.out.println();

			System.out.println("WORST FIT ###################################################");
			WorstFit w = new WorstFit();
			WorstFit w2 = new WorstFit();
			WorstFit w3 = new WorstFit();
			WorstFit w4 = new WorstFit();
			WorstFit w5 = new WorstFit();
			System.out.println();
			System.out.println("AVERAGE PROCESSES SWAPPED IN OVER 5 RUNS FOR WORST FIT: " + (float) (w.getAvg() + w2.getAvg() + w3.getAvg() + w4.getAvg() + w5.getAvg()) / 5);
			System.out.println();
			System.out.println();
	}


}

//AAAA...BBB
