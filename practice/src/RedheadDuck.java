public class RedheadDuck extends Duck {
    public RedheadDuck () {
        check_fly = new FlyWithWings();
        check_quack = new Quack();
    }

    @Override
    public void display() {
        System.out.println("I am a Redhead Duck");
    }
}
