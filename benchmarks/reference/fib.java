
public class fib {
    static long fib(int n) {
        if (n < 2) return n;
        return fib(n - 1) + fib(n - 2);
    }

    public static void main(String[] args) {
        long start = System.nanoTime();
        
        System.out.println("Fibonacci(30): " + fib(30));
        
        long end = System.nanoTime();
        double elapsed = (end - start) / 1_000_000_000.0;
        
        System.out.println("Time: " + elapsed);
    }
}
