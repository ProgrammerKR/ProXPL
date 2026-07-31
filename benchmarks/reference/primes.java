public class primes {
    static boolean isPrime(int n) {
        if (n <= 1) return false;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) return false;
        }
        return true;
    }

    public static void main(String[] args) {
        long start = System.nanoTime();
        int count = 0;
        for (int i = 0; i < 5000; i++) {
            if (isPrime(i)) count++;
        }
        System.out.println("Primes up to 5000: " + count);
        double elapsed = (System.nanoTime() - start) / 1e9;
        System.out.println("Time: " + elapsed);
    }
}
