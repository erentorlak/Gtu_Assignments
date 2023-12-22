import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class CSE222Graph {
    private List<Vertex> vertices;
    private Map<String, List<Vertex>> adjacencyList;

    public CSE222Graph(CSE222Map map) {
        int rows = map.getMap().length;
        int cols = map.getMap()[0].length;
        int size = rows * cols;
        vertices = new ArrayList<>();
        adjacencyList = new HashMap<>();

        // Create the vertices and adjacency list
        for (int i = 0; i < size; i++) {
            int y = i / cols;
            int x = i % cols;
            if (map.getMap()[y][x] == 0) {
                Vertex vertex = new Vertex(x, y);
                vertices.add(vertex);
                String key = getKey(x, y);
                adjacencyList.put(key, new ArrayList<>());
            }
        }
        // // print vertices
        // for (Vertex vertex : vertices) {
        // System.out.println(vertex.getX() + "-" + vertex.getY());
        // }

        // Create the edges
        for (int i = 0; i < size; i++) {
            int y = i / cols;
            int x = i % cols;
            if (map.getMap()[y][x] != 0) {
                continue;
            }

            List<int[]> neighbors = getValidNeighbors(x, y, rows, cols, map.getMap());
            for (int[] neighbor : neighbors) {
                Vertex neighborVertex = new Vertex(neighbor[0], neighbor[1]);
                Vertex currentVertex = new Vertex(x, y);
                adjacencyList.get(currentVertex.getX() + "-" + currentVertex.getY()).add(neighborVertex);
            }
        }

        // print vertices and their neighbors in string format
      // for (String key : adjacencyList.keySet()) {

      //     System.out.print(key + ": ");
      //     for (Vertex vertex : adjacencyList.get(key)) {
      //         System.out.print(vertex.getX() + "-" + vertex.getY() + " ");
      //     }
      //     System.out.println();

      // }

    }

    private String getKey(int x, int y) {
        return String.valueOf(x) + "-" + String.valueOf(y);
    }

    private List<int[]> getValidNeighbors(int row, int col, int numRows, int numCols, int[][] map) {
        // control 8 neighbors of the current vertex 
        int[][] neighbors = { { row - 1, col - 1 }, { row - 1, col }, { row - 1, col + 1 }, { row, col - 1 },
                { row, col + 1 }, { row + 1, col - 1 }, { row + 1, col }, { row + 1, col + 1 } };
        List<int[]> validNeighbors = new ArrayList<>();

        for (int[] neighbor : neighbors) {
            int x = neighbor[0];
            int y = neighbor[1];
            if (x >= 0 && x < numCols && y >= 0 && y < numRows && map[y][x] == 0) {
                validNeighbors.add(neighbor);
            }
        }

        return validNeighbors;
    }



    public List<Vertex> getVertices() {
        return vertices;
    }

    public Map<String, List<Vertex>> getAdjacencyList() {
        return adjacencyList;
    }
}
