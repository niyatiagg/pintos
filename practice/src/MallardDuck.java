public class MallardDuck extends Duck {
    public MallardDuck() {
        check_fly = new FlyWithWings();
        check_quack = new Quack();
    }

    @Override
    public void display() {
        System.out.println("I am a Mallard Duck");
    }
}
