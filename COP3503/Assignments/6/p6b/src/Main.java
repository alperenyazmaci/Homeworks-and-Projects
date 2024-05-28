import java.util.*;

public class Main {
    public static void main(String[] args) {
        //scanner object to take input
        Scanner scanner = new Scanner(System.in);
        //getting the first line input
        int n = scanner.nextInt();
        int k = scanner.nextInt();

        int[] nums = new int[n];
        for (int i = 0; i < n; i++) {
            nums[i] = scanner.nextInt();
        }

        //2d array to store gcd values
        int[][] dp = new int[n][2];

        //fill in the dp array
        for(int i = 0; i < n; i++){
            if(i == (n - 1)) {
                dp[i][0] = i;
                dp[i][1] = i;
            }else{
                dp[i][0] = i;
                int maximum = nums[i];
                int minimum = nums[i];
                for(int j = i; j < n; j++){
                    if(maximum < nums[j]){
                        maximum = nums[j];
                    }
                    if(minimum > nums[j]){
                        minimum = nums[j];
                    }
                    if((maximum - minimum) > k){
                        dp[i][1] = j - 1;
                        j = n;
                    }
                }
            }
        }

        scanner.close();
    }

    public static int[][] recursion(int[][] dp, int[] nums, int n, int k, int i){
//        if(i == (dp.length - 1)){
//
//        }
        return null;
    }

    public static int calculateSum(int nums[], int start, int end){
        int result = 0;
        for(int i = start; i <= end; i++){
            System.out.println(nums[i]);
            result += nums[i];
        }
        return result;
    }
}