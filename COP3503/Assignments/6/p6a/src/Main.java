import java.util.*;

//I solved both tasks
public class Main {
    public static void main(String[] args) {
        //scanner object to take input
        Scanner scanner = new Scanner(System.in);
        //getting the first line input
        int n = scanner.nextInt();
        int k = scanner.nextInt();

        //nums array to store the input
        int[] nums = new int[n];
        for (int i = 0; i < n; i++) {
            nums[i] = scanner.nextInt();
        }

        //2d array to store gcd values
        int[][] dp = new int[n][n];

        //fill in the dp array
        for(int i = 0; i < n; i++){
            for(int j = 0; j < i; j++){
                dp[i][j] = -1;
            }
            dp[i][i] = nums[i];
            for(int j = i + 1; j < n; j++){
                dp[i][j] = gcd(dp[i][j - 1], nums[j]);
            }
        }

        //apply the recursion function to get the finalResult
        int[] tempResult = new int[k];
        int[] finalResultTemp = new int[k];
        int[] finalResult = recursion(n, k, 0, dp, tempResult, finalResultTemp);

        //output
        System.out.println(calculateGcd(finalResult, dp, n, k));
        for(int i = 0; i < k; i++){
            System.out.print((finalResult[i] + 1) + " ");
        }
        System.out.println("");

        //close scanner object
        scanner.close();
    }

    //function that finds gcd of a add b
    public static int gcd(int a, int b) {
        if (b == 0) return a;
        return gcd(b, Math.abs(a - b));
    }

    //a recursive function that
    public static int[] recursion(int n, int k, int tempK, int[][] dp, int[] tempResult, int[] finalResult){
        finalResult[0] = 0;
        tempResult[0] = 0;

        tempK++;

        //base case
        if(tempK == k){
            if(finalResult[1] == 0){
                System.arraycopy(tempResult, 0, finalResult, 0, k);
            }
            int gcdTempResult = calculateGcd(tempResult, dp, n, k);
            int gcdFinalResult = calculateGcd(finalResult, dp, n, k);

            if(gcdTempResult > gcdFinalResult){
                return tempResult;
            }else{
                return finalResult;
            }
        }

        //recursive case
        for(int i = (tempResult[tempK - 1] + 1); i < n - (k - tempK - 1); i++){
            tempResult[tempK] = i;
            System.arraycopy(recursion(n, k, tempK, dp, tempResult, finalResult), 0, finalResult, 0, k);
        }


        return finalResult;
    }

    //a function that calculates the sum of GCD's given the finalResult array which contains the starting indexes (0 based) of the partitions
    public static int calculateGcd(int[] finalResult, int[][] dp, int n, int k){
        int result = 0;
        for(int i = 0; i < finalResult.length; i++){
            int first = finalResult[i];
            int second;
            if(i + 1 >= k){
                second = n - 1;
            }
            else {
                second = finalResult[i + 1] - 1;
            }
            result += dp[first][second];
        }

        return result;
    }

}