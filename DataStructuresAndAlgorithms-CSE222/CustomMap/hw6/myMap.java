package hw6;
import java.util.LinkedHashMap;
import java.util.ArrayList;
/**
 * This class is used to store the count and the words of a letter
 */
public class myMap {
    private LinkedHashMap<String, info> map;
    private int mapSize;
    private String str;

    /**
     * Constructor for the myMap object
     * 
     * @param str
     */
    public myMap(String str) {
        map = new LinkedHashMap<>();
        mapSize = 0;
        this.str = str;
    }

    /**
     * This method prints the map
     */
    public void printMap() {
        for (String key : map.keySet()) {
            System.out.println(
                    "Letter: " + key + " - Count: " + map.get(key).getCount() + " - Words: "
                            + map.get(key).getWords());
        }
    }

    /**
     * This method builds the map
     * 
     * @param processed
     */
    public void buildMap(String processed) {
        String[] words = processed.split(" "); // split the string into words
        mapSize = words.length; // number of words in the string

        for (String word : words) {
            for (int i = 0; i < word.length(); i++) {
                String c = String.valueOf(word.charAt(i)); // get the character
                if (!map.containsKey(c)) { // if the character is not in the map
                    map.put(c, new info(1, new ArrayList<>()));
                    map.get(c).getWords().add(word);
                } else {
                    map.get(c).setCount(map.get(c).getCount() + 1);
                    map.get(c).getWords().add(word);
                }
            }
        }
    }

    /**
     * This method returns the map
     * 
     * @return
     */
    public LinkedHashMap<String, info> getMap() {
        return map;
    }

    /**
     * return str
     */
    public String getStr() {
        return str;
    }

    /**
     * Return the count of the key
     * 
     * @param leftKey
     * @return the count of the key
     */
    public int getCount(String leftKey) {
        return map.get(leftKey).getCount();
    }

}