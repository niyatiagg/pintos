public class DecoyDuck extends Duck {
    public DecoyDuck() {
        check_fly = new FlyNoWay();
        check_quack = new NoQuack();
    }

    @Override
    public void display() {
        System.out.println("I am a decoy duck");
    }

    @Override
    public void swim() {
        System.out.println("But I can swim. Yayayay!");
    }
}
