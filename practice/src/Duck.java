public abstract class Duck {
    Quackable check_quack;
    Flyable check_fly;

    public void swim() {
        System.out.println("All ducks float, even decoys");
    }

    public abstract void display();

    public void performFly() {
        check_fly.fly();
    }

    public void setCheck_fly(Flyable flyable) {
        check_fly = flyable;
    }

    public void setCheck_quack(Quackable quackable) {
        check_quack = quackable;
    }

    public void performQuack() {
        check_quack.quack();
    }
}


