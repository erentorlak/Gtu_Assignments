package hw6;
import java.util.ArrayList;

/**
 * This class is used to sort the map using merge sort
 */
public class mergeSort {
    private myMap originalMap;
    private myMap sortedMap;
    private String[] aux;

    /**
     * Constructor for the mergeSort object
     * 
     * @param originalMap
     */
    public mergeSort(myMap originalMap) {
        this.originalMap = originalMap;
        this.sortedMap = new myMap(originalMap.getStr());
        this.aux = new String[originalMap.getMap().size()];
    }

    /**
     * This method sorts the map using merge sort
     * The idea is to sort the keys of the map and then add the keys to the sorted
     * map
     */
    public void sortMap() {
        ArrayList<String> keys = new ArrayList<>(originalMap.getMap().keySet());
        mergeSortHelper(keys, 0, keys.size() - 1);
        for (String key : keys) {
            sortedMap.getMap().put(key, originalMap.getMap().get(key)); // add the sorted keys to the sorted map
        }
    }

    /**
     * This method is a helper method for merge sort
     * 
     * @param keys
     * @param left
     * @param right
     * 
     *              The idea is to split the array into two halves and sort each
     *              half recursively
     *              Then merge the two halves
     */
    private void mergeSortHelper(ArrayList<String> keys, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2; // find the middle index and split the array into two halves

            mergeSortHelper(keys, left, mid);
            mergeSortHelper(keys, mid + 1, right);

            merge(keys, left, mid, right);
        }
    }

    /**
     * This method merges the two halves of the array
     * 
     * The idea is to compare the counts of the two keys and add the smaller one to
     * the aux array
     * if the counts are equal, add the key that comes first in the original string
     * 
     * @param keys
     * @param left
     * @param mid
     * @param right
     */
    private void merge(ArrayList<String> keys, int left, int mid, int right) {
        int leftIter = left;
        int rightIter = mid + 1;

        int auxIter = left;

        // compare the counts of the two keys and add the smaller one to the aux array
        // if the counts are equal, add the key that comes first in the original string
        String keyLeft, keyRight;   
        while (leftIter <= mid && rightIter <= right) {
            keyLeft = keys.get(leftIter);
            keyRight = keys.get(rightIter);

            // if the counts are equal, left one comes first
            if (originalMap.getMap().get(keyLeft).getCount() <= originalMap.getMap().get(keyRight).getCount()) {
                aux[auxIter] = keyLeft; // add the key to the aux array and increment auxIter for the next key
                auxIter++;
                leftIter++; // increment leftIter for the loop to continue
            } else {
                aux[auxIter] = keyRight;
                auxIter++;
                rightIter++; // increment rightIter for the loop to continue
            }
        }
        // add the remaining keys. left to mid
        while (leftIter <= mid) {
            aux[auxIter] = keys.get(leftIter);
            auxIter++;
            leftIter++;
        }
        // add the remaining keys .mid to right
        while (rightIter <= right) {
            aux[auxIter] = keys.get(rightIter);
            auxIter++;
            rightIter++;
        }
        // copy the sorted aux array back to the original array
        for (int iter = left; iter <= right; iter++) {
            keys.set(iter, aux[iter]);
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

}