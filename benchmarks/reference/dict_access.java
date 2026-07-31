import java.util.HashMap;

public class dict_access {
    public static void main(String[] args) {
        long start = System.nanoTime();
        HashMap<String, Integer> d = new HashMap<>();
        for (int i = 0; i < 10000; i++) d.put("key" + i, i);
        
        long totalSum = 0;
        for (int i = 0; i < 10000; i++) totalSum += d.get("key" + i);
        
        System.out.println("Dict Sum: " + totalSum);
        double elapsed = (System.nanoTime() - start) / 1e9;
        System.out.println("Time: " + elapsed);
    }
}
