package hw6;
public class Main {
    public static void main(String[] args) {
        String input = "'Hush, hush!' whispered the rushing wind.";
        System.out.println("Original String: " + input);

        // Step 1: Preprocessing the input string

        // [^a-zA-Z\\s] : ^ means not, a-zA-Z means all letters, \\s means space

        String processed = input.replaceAll("[^a-zA-Z\\s]", "").toLowerCase();
        System.out.println("Preprocessed String: " + processed);

        // Step 2: Build the map
        myMap map = new myMap(processed);
        map.buildMap(processed);

        // Step 3: Sort the map
        mergeSort sortObject = new mergeSort(map);
        sortObject.sortMap();

        sortObject.printOriginalMap();
        sortObject.printSortedMap();

        // print aux array
        /*
         * for (String s : sortObject.aux) {
         * System.out.print(s + " ");
         * }
         */

    }
}
