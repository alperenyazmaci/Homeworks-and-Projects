import java.util.*;

public class connect {

    public static void main(String[] args) {

        //Scanner object for taking input
        Scanner stdin = new Scanner(System.in);

        //taking the input in the first line
        int n = stdin.nextInt();
        int m = stdin.nextInt();

        //creating a disjoint set
        disjointSet djSet = new disjointSet(n);

        //for loop for taking rest of the input
        for (int i = 0; i < m; i++) {
            int operation = stdin.nextInt();
            // 1st scenario
            if (operation == 1) {
                int u = stdin.nextInt();
                int v = stdin.nextInt();
                djSet.union(u-1, v-1);
            }
            // 2nd scenario
            else if (operation == 2) {
                printOutput(djSet, n);
            }
        }

    }


    static class disjointSet{
        public int[] parent;
        int totalEdges;

        // Creates a disjoint set of size n (0, 1, ..., n-1)
        public disjointSet(int n){
            parent = new int[n];
            totalEdges = 0;

            for (int i=0; i<n; i++)
                parent[i] = i;
        }

        //finds the root of the given int
        public int find(int v) {

            // I am the club president!!! (root of the tree)
            if (parent[v] == v) return v;

            // Find my parent's root.
            int res = find(parent[v]);

            // Attach me directly to the root of my tree.
            parent[v] = res;
            return res;
        }

        //unions the given two integers, returns true if they are unioned otherwise returns false
        public boolean union(int v1, int v2) {

            // Find respective roots.
            int rootv1 = find(v1);
            int rootv2 = find(v2);

            // No union done, v1, v2 already together.
            if (rootv1 == rootv2) return false;

            // Attach tree of v2 to tree of v1.
            parent[rootv2] = rootv1;
            totalEdges++;
            return true;
        }
    }

    //finds the greatest common divisor of the two given integers
    public static long gcd(long a, long b) {
        while (b != 0) {
            long temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    //takes in the djSet and returns the output
    public static void printOutput(disjointSet djSet, int n){
        HashMap<Integer, Integer> map = new HashMap<>();
        for(int i = 0; i < n; i++){
            int tmp = djSet.parent[i];
            while(djSet.parent[tmp] != tmp){
                tmp = djSet.parent[tmp];
            }
            if(map.containsKey(tmp)){
                map.replace(tmp, map.get(tmp) + 1);
            }else{
                map.put(tmp, 1);
            }
        }

        long count = 0;
        long total = 0;
        for(Integer value: map.values()){
            total += value * value;
            count++;
        }
        long gcd = gcd(count, total);

        total = total/gcd;
        count = count/gcd;

        System.out.println(total + "/" + count);
    }

}