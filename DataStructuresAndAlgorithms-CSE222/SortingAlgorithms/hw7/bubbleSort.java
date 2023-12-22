package hw7;

import java.util.ArrayList;
import java.util.LinkedHashMap;

/**
 * This class is used to sort the map using bubble sort
 */
public class bubbleSort {
    private myMap originalMap;
    private myMap sortedMap;

    /**
     * Constructor for the bubbleSort object
     *
     * @param originalMap
     */
    public bubbleSort(myMap originalMap) {
        this.originalMap = originalMap;
        this.sortedMap = new myMap(originalMap.getStr());
    }

    /**
     * This method sorts the map using bubble sort
     * The idea is to repeatedly swap adjacent keys if they are in the wrong order
     * until the entire array is sorted.
     * is it stable? yes because we are not swapping equal elements
     */
    public void sortMap() {
        ArrayList<String> keys = new ArrayList<>(originalMap.getMap().keySet());
        int keySize = keys.size();
        String keyLeft;
        String keyRight;
        for (int i = 0; i < keySize - 1; i++) {
            boolean isSwapped = false; // If no swaps are made in a pass, the array is already sorted
            for (int j = 0; j < keySize - i - 1; j++) {
                keyLeft = keys.get(j);
                keyRight = keys.get(j + 1);

                if (originalMap.getMap().get(keyLeft).getCount() > originalMap.getMap().get(keyRight).getCount()) {
                    swap(keys, j, j + 1);
                    isSwapped = true;
                }
            }
            if (!isSwapped) { // this makes time complexity O(n) for an already sorted array
                break;
            }
        }
        for (String key : keys) {
            sortedMap.getMap().put(key, originalMap.getMap().get(key)); // add the sorted keys to the sorted map
        }
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
