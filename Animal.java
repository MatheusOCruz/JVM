public interface Animal {
    void makeSound();

    default void eat() {
        System.out.println("Eating...");
    }
}