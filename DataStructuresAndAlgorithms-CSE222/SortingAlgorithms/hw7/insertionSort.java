package hw7;

import java.util.ArrayList;
import java.util.LinkedHashMap;

/**
 * This class is used to sort the map using insertion sort
 */
public class insertionSort {
    private myMap originalMap;
    private myMap sortedMap;

    /**
     * Constructor for the insertionSort object
     *
     * @param originalMap
     */
    public insertionSort(myMap originalMap) {
        this.originalMap = originalMap;
        this.sortedMap = new myMap(originalMap.getStr());
    }

    /**
     * This method sorts the map using insertion sort
     * The idea is to iteratively insert each element into its correct position
     * in the sorted part of the array.
     */
    public void sortMap() {
        ArrayList<String> keys = new ArrayList<>(originalMap.getMap().keySet());
        int keysSize = keys.size();

        // Iterate over the keys starting from the second element
        for (int currentIndex = 1; currentIndex < keysSize; currentIndex++) {
            String currentKey = keys.get(currentIndex);
            int previousIndex = currentIndex - 1;

            // Shift elements to the right until the correct position is found for the
            // current key
            while (previousIndex >= 0
                    && (originalMap.getCount(keys.get(previousIndex)) > originalMap.getCount(currentKey))) {
                keys.set(previousIndex + 1, keys.get(previousIndex)); // Shift the element to the right
                previousIndex--; // Move to the previous index
            }
            keys.set(previousIndex + 1, currentKey);
        }

        for (String key : keys) {
            sortedMap.getMap().put(key, originalMap.getMap().get(key)); // Add the sorted keys to the sorted map
        }
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
