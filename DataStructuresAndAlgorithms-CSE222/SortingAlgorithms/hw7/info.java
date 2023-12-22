package hw7;
import java.util.ArrayList;

/**
 * This class is used to store the count and the words of a letter
 * 
 */
public class info {
    private int count;
    private ArrayList<String> words;

    /**
     * Constructor for the info object
     * 
     * @param count
     * @param words
     */
    public info(int count, ArrayList<String> words) {
        this.count = count;
        this.words = words;
    }

    /**
     * Prints the info object
     * 
     */
    public void printInfo() {
        System.out.println("Count: " + count + " - Words: " + words);
    }

    /**
     * This method returns the count
     * 
     * @return
     */
    public int getCount() {
        return count;
    }

    /**
     * This method returns the words
     * 
     * @return
     */
    public ArrayList<String> getWords() {
        return words;
    }

    /**
     * This method sets the count
     * 
     * @param count
     */
    public void setCount(int count) {
        this.count = count;
    }

    /**
     * This method sets the words
     * 
     * @param words
     */
    public void setWords(ArrayList<String> words) {
        this.words = words;
    }
}