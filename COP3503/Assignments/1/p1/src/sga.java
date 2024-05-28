import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

public class sga {
    public static void main(String[] args) {
        //create a scanner object to take input
        Scanner scanner = new Scanner(System.in);

        //read the first input line and pass it to an integer
        int n = scanner.nextInt();
        scanner.nextLine();

        //create a map that will store the count of each name
        Map<String, Integer> names = new HashMap<>();
        //create a map that will store the count of names starting with the same letter
        Map<Character, Integer> firstLetters = new HashMap<>();

        //count each name with common starting letters
        for (int i = 0; i < n; i++) {
            String name = scanner.nextLine();
            names.put(name, names.getOrDefault(name, 0) + 1);
            char firstLetter = name.charAt(0);
            firstLetters.put(firstLetter, firstLetters.getOrDefault(firstLetter, 0) + 1);
        }

        //calculate the result including the invalid pairs
        long result = 0;
        for (int count : firstLetters.values()) {
            result += count * (count - 1); //update the totalPairs
        }

        //calculate the total number of invalid pairs
        long totalInvalidPairs = 0;
        for(int count : names.values()){
            if(count > 1){
                totalInvalidPairs += count * (count - 1);
            }
        }

        //subtract the total invalid pairs from the result(output)
        result -= totalInvalidPairs;

        //output the result
        System.out.println(result);

        //close the scanner
        scanner.close();
    }
}