import java.awt.Color;
import java.awt.image.BufferedImage;
import java.awt.image.RenderedImage;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.ArrayList;
import java.util.Scanner;
import javax.imageio.ImageIO;

/**
 * Represents a map
 */
public class CSE222Map {

    private int[][] map;
    private int startX;
    private int startY;
    private int goalX;
    private int goalY;

    /**
     * Constructs a CSE222Map object by reading the map from a file.
     *
     * @param filename The name of the file containing the map.
     * @param X_SIZE   The size of the map in the X-axis.
     * @param Y_SIZE   The size of the map in the Y-axis.
     */
    public CSE222Map(String filename, int X_SIZE, int Y_SIZE) {
        this.readMapsFromFile(filename);
    }

    /**
     * Gets the map array.
     *
     * @return The map array.
     */
    public int[][] getMap() {
        return map;
    }

    /**
     * Gets the starting X-coordinate.
     *
     * @return The starting X-coordinate.
     */
    public int getStartX() {
        return startX;
    }

    /**
     * Gets the starting Y-coordinate.
     *
     * @return The starting Y-coordinate.
     */
    public int getStartY() {
        return startY;
    }

    /**
     * Gets the goal X-coordinate.
     *
     * @return The goal X-coordinate.
     */
    public int getGoalX() {
        return goalX;
    }

    /**
     * Gets the goal Y-coordinate.
     *
     * @return The goal Y-coordinate.
     */
    public int getGoalY() {
        return goalY;
    }

    /**
     * Converts the map to a PNG image and saves it to a file.
     *
     * @param filename The name of the output PNG image file.
     */
    public void convertPNG(String filename) {
        int rows = map.length;
        int cols = map[0].length;
        BufferedImage image = new BufferedImage(cols, rows, BufferedImage.TYPE_INT_RGB);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (map[i][j] == 1) {
                    image.setRGB(j, i, Color.BLACK.getRGB());
                } else {
                    image.setRGB(j, i, Color.WHITE.getRGB());
                }
            }
        }

        try {
            ImageIO.write(image, "png", new File(filename.substring(0, filename.length() - 4) + ".png"));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Draws a path on the map and saves it as a PNG image.
     *
     * @param path     The path to be drawn on the map.
     * @param filename The name of the output PNG image file.
     * @param pathname The path name to be added to the output file name.
     */
    public void drawLine(List<int[]> path, String filename, String pathname) {
        try {
            ImageIO.write(createImage(path), "png",
                    new File(filename.substring(0, filename.length() - 4) + pathname + ".png"));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Creates an image witth a given path marked.
     *
     * @param path The path to be marked on the image.
     * @return The created image.
     */
    private RenderedImage createImage(List<int[]> path) {
        int rows = map.length;
        int cols = map[0].length;
        BufferedImage image = new BufferedImage(cols, rows, BufferedImage.TYPE_INT_RGB);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (map[i][j] == 1) {
                    image.setRGB(j, i, Color.BLACK.getRGB());
                } else {
                    image.setRGB(j, i, Color.WHITE.getRGB());
                }
            }
        }

        for (int[] coordinate : path) {
            image.setRGB(coordinate[0], coordinate[1], Color.RED.getRGB());
        }

        return image;
    }

    /**
     * Writes the path coordinates to a text file.
     *
     * @param path     The path coordinates to be written.
     * @param filename The name of the output text file.
     * @param pathname The path name to be added to the output file name.
     */
    public void writePath(List<int[]> path, String filename, String pathname) {
        try (FileWriter writer = new FileWriter(
                filename.substring(0, filename.length() - 4) + pathname + "_path.txt")) {
            for (int[] point : path) {
                writer.write(point[0] + " " + point[1] + "\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Reads the map from a file and initializes the map and start/goal coordinates.
     *
     * @param filename The name of the file containing the map.
     */
    private void readMapsFromFile(String filename) {
        int startX = 0, startY = 0, goalX = 0, goalY = 0;
        List<List<Integer>> map = new ArrayList<>();

        try (Scanner scanner = new Scanner(new File(filename))) {
            int i = 0;
            while (scanner.hasNextLine()) {
                String line = scanner.nextLine();
                String[] parts = line.split(",");

                for (int j = 0; j < parts.length; j++) {
                    if (parts[j].trim().equals("-1")) {
                        parts[j] = "1";
                    }
                }
                if (i == 0) {
                    startX = Integer.parseInt(parts[0].trim());
                    startY = Integer.parseInt(parts[1].trim());
                } else if (i == 1) {
                    goalX = Integer.parseInt(parts[0].trim());
                    goalY = Integer.parseInt(parts[1].trim());
                } else {
                    List<Integer> row = new ArrayList<>();
                    for (String part : parts) {
                        row.add(Integer.parseInt(part.trim()));
                    }
                    map.add(row);
                }
                i++;
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        this.map = convert2DListToArray(map);
        this.startX = startY;
        this.startY = startX;
        this.goalX = goalY;
        this.goalY = goalX;
    }

    /**
     * Converts a 2D List to a 2D array.
     *
     * @param list The 2D List to be converted.
     * @return The converted 2D array.
     */
    private int[][] convert2DListToArray(List<List<Integer>> list) {
        int[][] array = new int[list.size()][];
        for (int i = 0; i < list.size(); i++) {
            List<Integer> row = list.get(i);
            array[i] = new int[row.size()];
            for (int j = 0; j < row.size(); j++) {
                array[i][j] = row.get(j);
            }
        }
        return array;
    }
}
