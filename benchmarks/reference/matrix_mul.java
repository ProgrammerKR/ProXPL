public class matrix_mul {
    public static void main(String[] args) {
        long start = System.nanoTime();
        int size = 50;
        int[][] a = new int[size][size];
        int[][] b = new int[size][size];
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                a[i][j] = i + j;
                b[i][j] = i - j;
            }
        }
        
        int[][] c = new int[size][size];
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int sumVal = 0;
                for (int k = 0; k < size; k++) {
                    sumVal += a[i][k] * b[k][j];
                }
                c[i][j] = sumVal;
            }
        }
        
        System.out.println("Matrix[0][0]: " + c[0][0]);
        double elapsed = (System.nanoTime() - start) / 1e9;
        System.out.println("Time: " + elapsed);
    }
}
