public class ArrayPrintTests1 {
    // Função main
    public static void main(String[] args) {
                // Chamando o método myMethod()
        int[] numbers = new int[5];

                // Atribuindo valores ao array
                numbers[0] = 10;
                numbers[1] = 20;
                numbers[2] = 30;
                numbers[3] = 40;
                numbers[4] = 50;

                // Imprimindo os valores do array
                for (int i = 0; i < numbers.length; i++) {
                    System.out.println("Element at index ");
                    System.out.println(i);
                    System.out.println(": ");
                    System.out.println(numbers[i]);
                    System.out.println("\n");
                }
    }

    
}
