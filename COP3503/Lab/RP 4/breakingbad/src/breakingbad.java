import java.io.*;
import java.util.*;

public class breakingbad {
    //hash map for colorMap
    static Map<String, Integer> colorMap = new HashMap<>();
    //hash map for adjacency list (graph)
    static Map<String, List<String>> adjList = new HashMap<>();

    public static void main(String[] args) throws IOException {
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

        //take the number of inputs into n and assign each input string into a String array
        int n = Integer.parseInt(br.readLine());
        String[] items = new String[n];
        for (int i = 0; i < n; i++) {
            items[i] = br.readLine();
            adjList.put(items[i], new ArrayList<>());
        }

        //take the number of prohibited pair of items and add those pairs to the adjacency list (add edge between the nodes)
        int m = Integer.parseInt(br.readLine());
        for (int i = 0; i < m; i++) {
            String[] pair = br.readLine().split(" ");
            String item1 = pair[0];
            String item2 = pair[1];
            adjList.get(item1).add(item2);
            adjList.get(item2).add(item1);
        }

        //DFS for coloring the nodes
        boolean possible = true;
        for (String item : items) {
            if (!colorMap.containsKey(item)) {
                if (!dfs(item, 0)) {
                    possible = false;
                    break;
                }
            }
        }

        //print out the output accordingly
        if (!possible) { //print out impossible
            System.out.println("impossible");
        } else { //print out the divided item list
            List<String> walterItems = new ArrayList<>();
            List<String> jesseItems = new ArrayList<>();
            for (String item : items) {
                if (colorMap.get(item) == 0) {
                    walterItems.add(item);
                } else {
                    jesseItems.add(item);
                }
            }
            printList(walterItems);
            printList(jesseItems);
        }
    }

    //DFS search for the graph with int color input to make sure there are no problems at the graph
    static boolean dfs(String item, int color) {
        if (colorMap.containsKey(item)) {
            return colorMap.get(item) == color;
        }
        colorMap.put(item, color);
        for (String neighbor : adjList.get(item)) {
            if (!dfs(neighbor, 1 - color)) {
                return false;
            }
        }
        return true;
    }

    //takes a list of string and prints them
    static void printList(List<String> items) {
        for (String item : items) {
            System.out.print(item + " ");
        }
        System.out.println();
    }
}
