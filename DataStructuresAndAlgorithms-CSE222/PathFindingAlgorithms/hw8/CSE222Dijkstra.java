import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;

/**
 * This class implements Dijkstra's algorithm to find the shortest path between
 */
public class CSE222Dijkstra {
    private CSE222Graph graph;

    /**
     * Constructs a Dijkstra object with the specified graph.
     *
     * @param graph The graph on which Dijkstra's algorithm will be applied.
     */
    public CSE222Dijkstra(CSE222Graph graph) {
        this.graph = graph;
    }

    /**
     * Finds the shortest path from a given start vertex to a given goal vertex
     * using Dijkstra's algorithm.
     *
     * @param startX The x-coordinate of the start vertex.
     * @param startY The y-coordinate of the start vertex.
     * @param goalX  The x-coordinate of the goal vertex.
     * @param goalY  The y-coordinate of the goal vertex.
     * @return The list of coordinates representing the shortest path.
     */
    public List<int[]> findPath(int startX, int startY, int goalX, int goalY) {
        Vertex startVertex = new Vertex(startX, startY);
        Vertex goalVertex = new Vertex(goalX, goalY);

        Map<String, Integer> distances = new HashMap<>();
        Map<String, String> previousVertices = new HashMap<>();

        for (Vertex vertex : graph.getVertices()) { // Initialize distances
            if (vertex.equals(startVertex)) {
                distances.put(vertex.getKey(), 0);
            } else {
                distances.put(vertex.getKey(), Integer.MAX_VALUE);
            }
        }
        // it does compare the distances of the vertices
        PriorityQueue<Vertex> queue = new PriorityQueue<>(
                Comparator.comparingInt(v -> distances.get(v.getKey())));
        queue.add(startVertex);

        while (!queue.isEmpty()) {
            Vertex currentVertex = queue.poll();

            if (currentVertex.equals(goalVertex)) {
                System.out.println("Reached the goalVertex");
                break;
            }

            List<Vertex> neighbors = graph.getAdjacencyList().get(currentVertex.getKey());

            if (neighbors != null) { // just in case
                for (Vertex neighbor : neighbors) {
                    int distance = distances.get(currentVertex.getKey()) + 1;
                    String neighborKey = neighbor.getKey();

                    if (!distances.containsKey(neighborKey) || distance < distances.get(neighborKey)) {
                        distances.put(neighborKey, distance);
                        previousVertices.put(neighborKey, currentVertex.getKey());

                        if (queue.contains(neighbor)) {
                            queue.remove(neighbor);
                        }
                        queue.add(neighbor);
                    }
                }
            }
        }

        if (!previousVertices.containsKey(goalVertex.getKey())) { // no path found
            System.out.println("\n\n\nNo feasible path is found.\n\n\n");
            return new ArrayList<>();
        }

        List<int[]> shortestPath = new ArrayList<>();
        String currentKey = goalVertex.getKey();

        while (previousVertices.containsKey(currentKey)) { // reconstruct the path
            String[] parts = currentKey.split("-");
            int x = Integer.parseInt(parts[0]);
            int y = Integer.parseInt(parts[1]);
            shortestPath.add(new int[] { x, y });
            currentKey = previousVertices.get(currentKey);
            if (currentKey.equals(startVertex.getKey())) {
                break;
            }
        }

        shortestPath.add(new int[] { Integer.parseInt(currentKey.split("-")[0]),
                Integer.parseInt(currentKey.split("-")[1]) });
        // reverse the path
        List<int[]> reversedPath = new ArrayList<>();
        for (int i = shortestPath.size() - 1; i >= 0; i--) {
            reversedPath.add(shortestPath.get(i));
        }
        shortestPath = reversedPath;

        return shortestPath;
    }
}