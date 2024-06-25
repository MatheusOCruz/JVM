
public class TestCompares {
	public static void main(String[] args) {
		for (int v = -3; v <= 3; v++) {
			System.out.println(v);
			System.out.println(":");
			if (v == 0) {
				System.out.println("Igual a 0");
			}
			if (v != 0) {
				System.out.println("Diff a 0");
			}
			if (v < 0) {
				System.out.println("Menor a 0");
			}
			if (v <= 0) {
				System.out.println("Menor igual a 0");
			}
			if (v > 0) {
				System.out.println("Maior a 0");
			}
			if (v >= 0) {
				System.out.println("Maior igual a 0");
			}
			System.out.println("");
		}
	}
}
