import java.util.List;

public class TestCases implements Runnable {
    private String FileName;
    private int X_SIZE;
    private int Y_SIZE;

    public TestCases(String FileName, int X_SIZE, int Y_SIZE) {
        this.FileName = FileName;
        this.X_SIZE = X_SIZE;
        this.Y_SIZE = Y_SIZE;
    }

    public void test() {
        System.out.println("\n\n*******************\nMap is " + this.FileName + " with X_SIZE " + this.X_SIZE
                + " and Y_SIZE " + this.Y_SIZE + "\n********************\n");

        /*************************************************************************** */
        CSE222Map map = new CSE222Map(this.FileName, this.X_SIZE, this.Y_SIZE);
        int startX = map.getStartX();
        int startY = map.getStartY();
        int goalX = map.getGoalX();
        int goalY = map.getGoalY();

        CSE222Graph graph = new CSE222Graph(map);
        CSE222Dijkstra Dijkstra = new CSE222Dijkstra(graph);
        CSE222BFS BFS = new CSE222BFS(graph);
        /******************** Dijkstra ********************************************** */

        long startTime = System.currentTimeMillis();
        List<int[]> DijkstraPath = Dijkstra.findPath(startX, startY, goalX, goalY);

        long endTime = System.currentTimeMillis();
        long totalTime = endTime - startTime;
        System.out.println("Total time for : " + FileName + " Dijkstra ->" + totalTime + " ms");

        /************************* BFS************************************* */

        long startTime2 = System.currentTimeMillis();
        List<int[]> BFSPath = BFS.findPath(startX, startY, goalX, goalY);

        long endTime2 = System.currentTimeMillis();
        long totalTime2 = endTime2 - startTime2;
        System.out.println("Total time for : " + FileName + "BFS ->" + totalTime2 + " ms");

        /*************************************************************************** */
        map.convertPNG(FileName);

        map.drawLine(DijkstraPath, FileName, "dijkstra_path");
        map.drawLine(BFSPath, FileName, "bfs_path");
        map.writePath(BFSPath, FileName, "bfs_path");
        map.writePath(DijkstraPath, FileName, "dijkstra_path");

        System.out.println(FileName + " Dijkstra Path: " + DijkstraPath.size());
        System.out.println(FileName + " BFS Path: " + BFSPath.size());

    }

    @Override
    public void run() {
        test();
    }

}