public class Vertex {
    private String x;
    private String y;

    public Vertex(int x, int y) {
        this.x = String.valueOf(x);
        this.y = String.valueOf(y);
    }

    public Vertex(String string) {
        String[] parts = string.split("-");
        this.x = parts[0];
        this.y = parts[1];
    }

    public String getX() {
        return x;
    }

    public String getY() {
        return y;
    }

    public void setX(String x) {
        this.x = x;
    }

    public void setY(String y) {
        this.y = y;
    }

    public String getKey() {
        return x + "-" + y;
    }


}
