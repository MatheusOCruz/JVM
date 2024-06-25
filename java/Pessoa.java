public class Pessoa {
    private int idade;

   
    public Pessoa(int idade) {
        this.idade = idade;
    }

   
    public int getIdade() {
        return idade;
    }

   
    public void exibirInfo() {
        System.out.println(idade);
    }
}
