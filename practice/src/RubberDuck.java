public class RubberDuck extends Duck {
    public RubberDuck() {
        check_fly = new FlyNoWay();
        check_quack = new Squeak();
    }

    @Override
    public void display() {
        System.out.println("I am a rubber duck");
    }
}
