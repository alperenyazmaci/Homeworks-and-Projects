import java.util.*;

public class Main {

    //Position class to use for drone and group positions
    static class Position {
        int x, y;

        Position(int x, int y) {
            this.x = x;
            this.y = y;
        }

        public String toString(){
            return "x: " + (x + 1) + ", y: " + (y + 1) + ". ";
        }
    }

    //static 2d int array that has 4 directions (right, left, up, down)
    static final int[][] directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    public static void main(String[] args) {
        //Scanner object to take input
        Scanner scanner = new Scanner(System.in);
        //Getting the first input
        int n = scanner.nextInt();
        scanner.nextLine();

        //2d String array to store grid information and position arrays to store drones and groups
        String[][] grid = new String[8][8];
        Position[] drones = new Position[n];
        Position[] groups = new Position[n];

        for (int i = 0; i < 8; i++) { //for loop that loops through each row
            String line = scanner.nextLine();
            //putting the input into a String array
            String[] cells = line.split(" ");
            for (int j = 0; j < 8; j++) { //for loop that loops through each column and store the drone and group information
                grid[i][j] = "__";
                if(cells[j].charAt(0) == 'D'){
                    int droneIndex = Integer.parseInt(String.valueOf(cells[j].charAt(1))) - 1;
                    drones[droneIndex] = new Position(j, i);
                }else if(cells[j].charAt(0) == 'G'){
                    grid[i][j] = cells[j];
                    int groupIndex = Integer.parseInt(String.valueOf(cells[j].charAt(1))) - 1;
                    groups[groupIndex] = new Position(j, i);
                }else{
                    grid[i][j] = cells[j];
                }
            }
        }

        //print out the result
        System.out.println(bfs(grid, groups, drones, calculateDronesInBinary(drones, n), n));

    }

    //bfs algorithm that finds the goal state
    static int bfs(String[][] grid, Position[] groups, Position[] drones, int initialState, int n) {
        //queue and visited to store the correspondding information
        Queue<Integer> queue = new LinkedList<>();
        Set<Integer> visited = new HashSet<>();

        //put the initial state into queue and visited
        queue.offer(initialState);
        visited.add(initialState);

        //integer variables that will be used to calculate how many moves were made
        int result = 0;
        int currentLevelNodes = 1;
        int nextLevelNodes = 0;

        while (!queue.isEmpty()) {
            int currentState = queue.poll();
            currentLevelNodes--;

            Position[] newDronesPositions = calculatePositionsFromBinary(currentState, n);

            //if all of the food are delivered, end the bfs algorithm
            if (isGoalState(groups, newDronesPositions, n)) {
                result++;
                break;
            }

            //try all of the 4 directions on each of drones at that level
            for (int i = 0; i < n; i++) {
                boolean isGoal = isGoalStateInd(groups[i], drones[i]);
                if(isGoal){

                }else{
                    for (int[] dir : directions) {
                        int newX = newDronesPositions[i].x + dir[0];
                        int newY = newDronesPositions[i].y + dir[1];

                        if (isValidMove(newX, newY, i+1, grid)) {
                            if(isNoFlyZone(newX, newY, grid)){

                            }else{
                                int newState = currentState & ~(63 << (6 * i));
                                newState |= ((newX * 8 + newY) << (6 * i));
                                if (!visited.contains(newState)) {
                                    queue.offer(newState);
                                    visited.add(newState);
                                    nextLevelNodes++;
                                }
                            }
                        }
                    }
                }
            }
            if (currentLevelNodes == 0) {
                currentLevelNodes = nextLevelNodes;
                nextLevelNodes = 0;
                result++;
            }
        }


        //return the moves calculated
        return result - 2;
    }

    //takes in Position array of drones and number of drones, then returns an integer representing drone position in binary
    static int calculateDronesInBinary(Position[] drones, int n){
        if(n == 1){
            return (
                    calculateDronePos(drones[0])
            );
        }else if(n == 2){
            return (
                    (calculateDronePos(drones[1]) << 6) + calculateDronePos(drones[0])
            );
        }else if(n == 3){
            return (
                    (calculateDronePos(drones[2]) << 12) + (calculateDronePos(drones[1]) << 6) + calculateDronePos(drones[0])
            );
        }else if(n == 4){
            return (
                    (calculateDronePos(drones[3]) << 18) + (calculateDronePos(drones[2]) << 12) + (calculateDronePos(drones[1]) << 6) + calculateDronePos(drones[0])
            );
        }else{
            return 0;
        }
    }

    //calculates a position in a single number instead of (x, y)
    static int calculateDronePos(Position drone){
        return drone.y * 8 + drone.x;
    }

    //takes in an int which represents positions of drones, then returns Position array of drones positions
    static Position[] calculatePositionsFromBinary(int binary, int n){
        Position[] result = new Position[n];

        for(int i = 0; i < n; i++){
            result[i] = calculatePosition(binary, n, i);
        }

        return result;
    }

    //calculates the position in (x, y) format from an int
    static Position calculatePosition(int binary, int n, int i){
        binary  = binary >> (6 * i);
        int tmp  = 63 & binary;
        int x = tmp % 8;
        tmp -= x;
        int y = tmp / 8;
        return new Position(x, y);
    }

    //returns true if the drone with the droneNumber can move to (x, y), returns false otherwise
    static boolean isValidMove(int x, int y, int droneNumber, String[][] grid) {
        boolean wrongGroup = false;

        if(isNoFlyZone(x, y, grid))
            return true;


        if(grid[x][y].charAt(0) == 'G'){
            int num = Integer.parseInt(String.valueOf(grid[x][y].charAt(1)));
            if(num != droneNumber) {
                wrongGroup = true;
            }
        }else if(grid[x][y].charAt(0) == 'A'){
            wrongGroup = true;
        }

        return !wrongGroup;
    }

    //returns true if the given (x, y) position is a no fly zone, returns false otherwise
    static boolean isNoFlyZone(int x, int y, String[][] grid) {
        return  (x < 0 || x >= 8 || y < 0 || y >= 8) || (grid[x][y].charAt(0) == 'X');
    }

    //returns true if the given values are complete, returns false otherwise
    static boolean isGoalState(Position[] groups, Position[] drones, int n) {
        for (int i = 0; i < n; i++){
            Position group = groups[i];
            Position drone = drones[i];
            if(group.x != drone.x || group.y != drone.y)
                return false;
        }
        return true;
    }

    //returns true if the given individual drone is in the goal group, returns false otherwise
    static boolean isGoalStateInd(Position group, Position drone){
        if(group.x == drone.x && group.y == drone.y)
            return true;
        else
            return false;
    }

    /*
    static void printGrid(String[][] grid) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                System.out.print(grid[i][j] + " ");
            }
            System.out.println();
        }
    }

    static void printPositions(Position[] drones, int n) {
        for (int i = 0; i < n; i++) {
            System.out.println(drones[i]);
        }
    }

    static void printStringArray(String[] strings){
        for(String string : strings){
            System.out.print(string + " ");
        }
        System.out.println("");
    }
    */

}