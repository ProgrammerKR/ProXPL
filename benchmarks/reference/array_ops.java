import java.util.ArrayList;

public class array_ops {
    public static void main(String[] args) {
        long start = System.nanoTime();
        ArrayList<Integer> arr = new ArrayList<>();
        for (int i = 0; i < 50000; i++) arr.add(i);
        
        long totalSum = 0;
        for (int i = 0; i < arr.size(); i++) totalSum += arr.get(i);
        
        for (int i = 0; i < 25000; i++) arr.remove(arr.size() - 1);
        
        System.out.println("Array Sum: " + totalSum);
        System.out.println("Array Length: " + arr.size());
        double elapsed = (System.nanoTime() - start) / 1e9;
        System.out.println("Time: " + elapsed);
    }
}
