package hw7;

import java.util.ArrayList;
import java.util.LinkedHashMap;

/**
 * This class is used to sort the map using selection sort
 */
public class selectionSort {
    private myMap originalMap;
    private myMap sortedMap;

    /**
     * Constructor for the selectionSort object
     *
     * @param originalMap
     */
    public selectionSort(myMap originalMap) {
        this.originalMap = originalMap;
        this.sortedMap = new myMap(originalMap.getStr());
    }

    /**
     * This method sorts the map using selection sort
     * The idea is to repeatedly find the minimum key in the unsorted portion
     * and swap it with the first unsorted key.
     */
    public void sortMap() {
        ArrayList<String> keys = new ArrayList<>(originalMap.getMap().keySet());
        int keysSize = keys.size();
        for (int i = 0; i < keysSize - 1; i++) {
            int minIndex = i;
            for (int j = i + 1; j < keysSize; j++) {
                if (originalMap.getMap().get(keys.get(j)).getCount() < originalMap.getMap().get(keys.get(minIndex)).getCount()) {
                    minIndex = j;
                }
            }
            if (minIndex != i) {
                swap(keys, i, minIndex);
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

    public LinkedHashMap<String, info> getSortedMap() {
        return sortedMap.getMap();
    }

}
