import java.util.*;

/**
 * This class implements the Breadth-First Search (BFS) algorithm for finding
 * the shortest path in a graph.
 */
public class CSE222BFS {
    private CSE222Graph graph;

    /**
     * Constructs a CSE222BFS object with the given graph.
     *
     * @param graph the graph to perform BFS on
     */
    public CSE222BFS(CSE222Graph graph) {
        this.graph = graph;
    }

    /**
     * Finds the shortest path between the given start and goal vertices using BFS.
     *
     * @param startX the x-coordinate of the start vertex
     * @param startY the y-coordinate of the start vertex
     * @param goalX  the x-coordinate of the goal vertex
     * @param goalY  the y-coordinate of the goal vertex
     * @return a list of int arrays representing the shortest path coordinates, or
     *         null if no path is found
     */
    public List<int[]> findPath(int startX, int startY, int goalX, int goalY) {
        Vertex startVertex = new Vertex(startX, startY);
        Vertex goalVertex = new Vertex(goalX, goalY);

        Map<String, String> previousVertices = new HashMap<>();
        Queue<Vertex> queue = new LinkedList<>();

        queue.add(startVertex);
        previousVertices.put(startVertex.getKey(), null); // null means no previous vertex

        while (!queue.isEmpty()) {
            Vertex currentVertex = queue.poll();

            if (currentVertex.equals(goalVertex)) {
                System.out.println("Reached the goal vertex!");
                break;
            }

            List<Vertex> neighbors = graph.getAdjacencyList().get(currentVertex.getKey());

            if (neighbors != null) {
                for (Vertex neighbor : neighbors) {
                    String neighborKey = neighbor.getKey();

                    if (!previousVertices.containsKey(neighborKey)) {
                        queue.add(neighbor);
                        previousVertices.put(neighborKey, currentVertex.getKey());
                    }
                }
            } else {
                System.out.println(currentVertex.getKey() + " has no neighbors");
            }
        }

        if (!previousVertices.containsKey(goalVertex.getKey())) {
            // No path found
            return null;
        }

        // Reconstruct the path from startVertex to goalVertex
        List<int[]> shortestPath = new ArrayList<>();
        String currentKey = goalVertex.getKey();

        while (currentKey != null) {
            String[] parts = currentKey.split("-");
            int x = Integer.parseInt(parts[0]);
            int y = Integer.parseInt(parts[1]);
            shortestPath.add(new int[] { x, y });
            currentKey = previousVertices.get(currentKey);
        }

        Collections.reverse(shortestPath); // reverse the list so that it starts from start
        return shortestPath;
    }

}

class CSE222DFS {
    private CSE222Graph graph;

    /**
     * Constructs a CSE222DFS object with the given graph.
     *
     * @param graph the graph to perform DFS on
     */
    public CSE222DFS(CSE222Graph graph) {
        this.graph = graph;
    }

    /**
     * Performs a Depth-First Search starting from the given vertex.
     *
     * @param startX the x-coordinate of the start vertex
     * @param startY the y-coordinate of the start vertex
     */
    public void performDFS(int startX, int startY) {
        Vertex startVertex = new Vertex(startX, startY);
        Set<String> visited = new HashSet<>();

        dfsHelper(startVertex, visited);
    }

    private void dfsHelper(Vertex currentVertex, Set<String> visited) {
        System.out.println(currentVertex.getKey());
        visited.add(currentVertex.getKey());

        List<Vertex> neighbors = graph.getAdjacencyList().get(currentVertex.getKey());

        if (neighbors != null) {
            for (Vertex neighbor : neighbors) {
                if (!visited.contains(neighbor.getKey())) {
                    dfsHelper(neighbor, visited);
                }
            }
        }
    }
}
