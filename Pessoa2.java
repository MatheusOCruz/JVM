public class Pessoa2 extends Pessoa {
    private int numero;

    public Pessoa2(int idade, int numero) {
        super(idade);
        this.numero = numero;
    }

    public int getNumero() {
        return this.numero;
    }

    public void exibirInfo2() {
        System.out.println("Idade: ");
        System.out.println(getIdade()*Pessoa.mul);
        System.out.println("\nNúmero: ");
        System.out.println(this.numero);
    }
    static void teste(){
    System.out.println("estaticooo\n");
    }
}
