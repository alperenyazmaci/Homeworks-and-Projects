import java.util.*;

//I solved all tasks 1,2,3
public class Main {

    public static void main(String[] args) {
        //scanner object to take input
        Scanner scanner = new Scanner(System.in);
        //getting the first input
        int n = scanner.nextInt();
        int m = scanner.nextInt();
        scanner.nextLine();
        //creating string arrays to store the inputs in the next for loops
        List<String> drugs = new ArrayList<>();
        List<String> codes = new ArrayList<>();


        //getting the names of single drugs
        for(int i = 0; i < n; i++){
            String str = scanner.nextLine();
            drugs.add(str);
        }

        //getting the names of one code
        for(int i = 0; i < m; i++){
            String str = scanner.nextLine();
            codes.add(str);
        }

        //sort the string objects in codes arraylist
        Collections.sort(codes);

        //size is all vertices excluding sink and source
        int size = n + m;

        //set the source and sink indices
        int source = size;
        int sink = size + 1;


        FordFulkerson ff = new FordFulkerson(size);


        //adding edges from source to drugs
        for (int i = 0; i < n; i++) {
            ff.add(source, i, 1);
        }

        //adding edges from codes to sink
        for (int i = 0; i < m; i++) {
            ff.add(n + i, sink, 1);
        }

        //adding edges from drugs to codes if the drug contains code in it
        for (int i = 0; i < n; i++) {
            String drug = drugs.get(i);
            for (int j = 0; j < m; j++) {
                String code = codes.get(j);
                if (drug.contains(code)) {
                    ff.add(i, n + j, 1);
                }
            }
        }

        //determine if there exists a valid mapping.
        //if there exists a valid mapping print out "yes" and the codes in order that they match with drugs in order
        //otherwise print out "no"
        if (ff.ff() == n) {
            System.out.println("yes");

            //output the valid mapping
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    if (ff.cap[i][n + j] == 0 && ff.cap[n + j][i] == 1) {
                        System.out.println(codes.get(j));
                        break;
                    }
                }
            }
        } else {
            System.out.println("no");
        }

        scanner.close();

    }
}

class FordFulkerson {

    // Stores graph.
    public int[][] cap;
    public int n;
    public int source;
    public int sink;

    // "Infinite" flow.
    final public static int oo = (int)(1E9);

    // Set up default flow network with size+2 vertices, size is source, size+1 is sink.
    public FordFulkerson(int size) {
        n = size + 2;
        source = n - 2;
        sink = n - 1;
        cap = new int[n][n];
    }

    // Adds an edge from v1 -> v2 with capacity c.
    public void add(int v1, int v2, int c) {
        cap[v1][v2] = c;
    }

    // Wrapper function for Ford-Fulkerson Algorithm
    public int ff() {

        // Set visited array and flow.
        boolean[] visited = new boolean[n];
        int flow = 0;

        // Loop until no augmenting paths found.
        while (true) {

            // Run one DFS.
            Arrays.fill(visited, false);
            int res = dfs(source, visited, oo);

            // Nothing found, get out.
            if (res == 0) break;

            // Add this flow.
            flow += res;
        }

        // Return it.
        return flow;
    }

    // DFS to find augmenting math from v with maxflow at most min.
    public int dfs(int v, boolean[] visited, int min) {

        // got to the sink, this is our flow.
        if (v == sink)  return min;

        // We've been here before - no flow.
        if (visited[v])  return 0;

        // Mark this node and recurse.
        visited[v] = true;
        int flow = 0;

        // Just loop through all possible next nodes.
        for (int i = 0; i < n; i++) {

            // We can augment in this direction.
            if (cap[v][i] > 0)
                flow = dfs(i, visited, Math.min(cap[v][i], min));

            // We got positive flow on this recursive route, return it.
            if (flow > 0) {

                // Subtract it going forward.
                cap[v][i] -= flow;

                // Add it going backwards, so that later, we can flow back through this edge as a backedge.
                cap[i][v] += flow;

                // Return this flow.
                return flow;
            }
        }

        // If we get here there was no flow.
        return 0;
    }

    public void printCap() {
        for (int i = 0; i < cap.length; i++) {
            for (int j = 0; j < cap[i].length; j++) {
                System.out.print(cap[i][j] + " ");
            }
            System.out.println();
        }
    }
}