import java.util.ArrayList;

public class merge_sort {
    static ArrayList<Integer> merge(ArrayList<Integer> left, ArrayList<Integer> right) {
        ArrayList<Integer> result = new ArrayList<>();
        int i = 0, j = 0;
        while (i < left.size() && j < right.size()) {
            if (left.get(i) <= right.get(j)) result.add(left.get(i++));
            else result.add(right.get(j++));
        }
        while (i < left.size()) result.add(left.get(i++));
        while (j < right.size()) result.add(right.get(j++));
        return result;
    }

    static ArrayList<Integer> mergeSort(ArrayList<Integer> arr) {
        if (arr.size() <= 1) return arr;
        int mid = arr.size() / 2;
        ArrayList<Integer> left = new ArrayList<>(arr.subList(0, mid));
        ArrayList<Integer> right = new ArrayList<>(arr.subList(mid, arr.size()));
        return merge(mergeSort(left), mergeSort(right));
    }

    public static void main(String[] args) {
        long start = System.nanoTime();
        ArrayList<Integer> arr = new ArrayList<>();
        for (int i = 0; i < 5000; i++) arr.add(5000 - i);
        ArrayList<Integer> sorted = mergeSort(arr);
        
        System.out.println("First: " + sorted.get(0));
        System.out.println("Last: " + sorted.get(sorted.size() - 1));
        double elapsed = (System.nanoTime() - start) / 1e9;
        System.out.println("Time: " + elapsed);
    }
}
