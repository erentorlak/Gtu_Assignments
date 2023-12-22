package hw7;

import java.util.ArrayList;
import java.util.LinkedHashMap;

/**
 * This class is used to sort the map using quicksort
 */
public class quickSort {
    private myMap originalMap;
    private myMap sortedMap;

    /**
     * Constructor for the quickSort object
     *
     * @param originalMap
     */
    public quickSort(myMap originalMap) {
        this.originalMap = originalMap;
        this.sortedMap = new myMap(originalMap.getStr());
    }

    /**
     * This method sorts the map using quicksort
     * The idea is to select a pivot element and partition the array around the
     * pivot
     * so that all elements on the left are smaller and all elements on the right
     * are larger.
     * Then recursively apply quicksort on the two partitions.
     */
    public void sortMap() {
        ArrayList<String> keys = new ArrayList<>(originalMap.getMap().keySet());
        quickSortHelper(keys, 0, keys.size() - 1);
        for (String key : keys) {
            sortedMap.getMap().put(key, originalMap.getMap().get(key)); // add the sorted keys to the sorted map
        }
    }

    /**
     * This method is a helper method for quicksort
     *
     * @param keys
     * @param low
     * @param high
     */
    private void quickSortHelper(ArrayList<String> keys, int low, int high) {
        if (low < high) {
            int partitionIndex = partition(keys, low, high);
            quickSortHelper(keys, low, partitionIndex - 1);
            quickSortHelper(keys, partitionIndex + 1, high);
        }
    }

    /**
     * This method selects the last element as the pivot and partitions the array
     * such that all elements on the left are smaller and all elements on the right
     * are larger.
     * we dont swap equal elements so it is stable
     * @param keys
     * @param low
     * @param high
     * @return the partition index
     */
    private int partition(ArrayList<String> keys, int low, int high) {
        String pivot = keys.get(high);
        int i = low - 1;
        for (int j = low; j < high; j++) {
            String key = keys.get(j);
            if (originalMap.getMap().get(key).getCount() < originalMap.getMap().get(pivot).getCount()) {
                i++;
                swap(keys, i, j);
            }
        }
        swap(keys, i + 1, high);
        return i + 1;
    }

    /**
     * This method swaps the elements at the given indices in the keys array
     *
     * @param keys
     * @param i
     * @param j
     */
    private void swap(ArrayList<String> keys, int i, int j) {
        String temp = keys.get(i);
        keys.set(i, keys.get(j));
        keys.set(j, temp);
    }

    /**
     * This method prints the original map
     */
    public void printOriginalMap() {
        System.out.println("The original (unsorted) map:");
        originalMap.printMap();
    }

    /**
     * This method prints the original map and the sorted map
     */
    public void printSortedMap() {
        System.out.println("The sorted map:");
        sortedMap.printMap();
    }

    /**
     * This method returns the sorted map
     * 
     * @return
     */
    public LinkedHashMap<String, info> getSortedMap() {
        return sortedMap.getMap();
    }

}
