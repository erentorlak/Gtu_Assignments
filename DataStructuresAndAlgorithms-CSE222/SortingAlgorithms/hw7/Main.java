package hw7;

public class Main {
  public static void main(String[] args) {
    // Worst-case scenario input
    String worstCaseInput = "dddd ccc bb a";
    System.out.println("Worst-case Input: " + worstCaseInput);
    processAndSortMap(worstCaseInput);
    
    // Average-case scenario input
    String averageCaseInput = "abc abcde asvs ";
    System.out.println("Average-case Input: " + averageCaseInput);
    processAndSortMap(averageCaseInput);
    
    // Best-case scenario input
    String bestCaseInput = "aa b cc d"; 
    System.out.println("Best-case Input: " + bestCaseInput);
    processAndSortMap(bestCaseInput);
  }

  /**
   * Preprocesses the input string, builds the map, and sorts it using different
   * algorithms
   * 
   * @param input the input string
   */
  private static void processAndSortMap(String input) {
    // Step 1: Preprocessing the input string
    String processed = input.replaceAll("[^a-zA-Z\\s]", "").toLowerCase();
    System.out.println("Preprocessed String: " + processed);

    // Step 2: Build the map
    myMap map = new myMap(processed);
    map.buildMap(processed);

    // Step 3: Sort the map using different sorting algorithms

    // Merge Sort
    mergeSort mergeSortObject = new mergeSort(map);
    long startTime = System.nanoTime();
    mergeSortObject.sortMap();
    long endTime = System.nanoTime();
    System.out.println("Merge Sort:");
    mergeSortObject.printSortedMap();
    System.out.println("Time taken: " + (endTime - startTime) + " nanoseconds");

    // Selection Sort
    selectionSort selectionSortObject = new selectionSort(map);
    startTime = System.nanoTime();
    selectionSortObject.sortMap();
    endTime = System.nanoTime();
    System.out.println("Selection Sort:");
    selectionSortObject.printSortedMap();
    System.out.println("Time taken: " + (endTime - startTime) + " nanoseconds");

    // Insertion Sort
    insertionSort insertionSortObject = new insertionSort(map);
    startTime = System.nanoTime();
    insertionSortObject.sortMap();
    endTime = System.nanoTime();
    System.out.println("Insertion Sort:");
    insertionSortObject.printSortedMap();
    System.out.println("Time taken: " + (endTime - startTime) + " nanoseconds");

    // Bubble Sort
    bubbleSort bubbleSortObject = new bubbleSort(map);
    startTime = System.nanoTime();
    bubbleSortObject.sortMap();
    endTime = System.nanoTime();
    System.out.println("Bubble Sort:");
    bubbleSortObject.printSortedMap();
    System.out.println("Time taken: " + (endTime - startTime) + " nanoseconds");

    // Quick Sort
    quickSort quickSortObject = new quickSort(map);
    startTime = System.nanoTime();
    quickSortObject.sortMap();
    endTime = System.nanoTime();
    System.out.println("Quick Sort:");
    quickSortObject.printSortedMap();
    System.out.println("Time taken: " + (endTime - startTime) + " nanoseconds");

    // comparing the sorted maps
    System.out.println();
    if (mergeSortObject.getSortedMap().equals(bubbleSortObject.getSortedMap())
        && bubbleSortObject.getSortedMap().equals(insertionSortObject.getSortedMap())
        && insertionSortObject.getSortedMap().equals(quickSortObject.getSortedMap())
        && quickSortObject.getSortedMap().equals(selectionSortObject.getSortedMap())) {
      System.out.println("All the sorted maps are the same");
    } else {
      System.out.println("The sorted maps are not the same");
    }
    System.out.println();
  }
}
