public class collatz {
    static int collatz(int n) {
        int steps = 0;
        while (n > 1) {
            if (n % 2 == 0) n /= 2;
            else n = 3 * n + 1;
            steps++;
        }
        return steps;
    }

    public static void main(String[] args) {
        long start = System.nanoTime();
        int maxSteps = 0;
        for (int i = 1; i <= 20000; i++) {
            int steps = collatz(i);
            if (steps > maxSteps) maxSteps = steps;
        }
        System.out.println("Max Collatz steps: " + maxSteps);
        double elapsed = (System.nanoTime() - start) / 1e9;
        System.out.println("Time: " + elapsed);
    }
}
