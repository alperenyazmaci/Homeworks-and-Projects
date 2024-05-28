import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner stdin = new Scanner(System.in);
        int[] puzzle = new int[12];
        int[] tmpPuzzle = new int[12];
        int[] used = new int[12];
        int sumOfAll = 0;

        //get the first input line
        for (int i=0; i<12; i++) {
            puzzle[i] = stdin.nextInt();
            sumOfAll += puzzle[i];
            tmpPuzzle[i] = 0;
            used[i] = 0;
        }

        while (!zero(puzzle)) {

            int magicSum = sumOfAll / 3;

            int output = backtrack(puzzle, tmpPuzzle, used, magicSum, 0);
            System.out.println(output/12);

            for (int i=0; i<12; i++) {
                puzzle[i] = stdin.nextInt();
                sumOfAll += puzzle[i];
                tmpPuzzle[i] = 0;
                used[i] = 0;
            }
        }



    }

    //a function that backtracks and tries every possible solution
    private static int backtrack(int[] puzzle, int[] tmpPuzzle, int[] used, int magicSum, int position) {
        //RECURSIVE BASE
        if (position == 12 ) {
            //returns 1 if tmpPuzzle is valid, returns 0 otherwise
            if(isValidHexagram(tmpPuzzle)) {
                return 1;
            }
            else {
                return 0;
            }
        }

        int count = 0;
        for (int i = 0; i < 12; i++) {
            if (used[i] == 0) {
                used[i] = 1;
                tmpPuzzle[position] = puzzle[i];

                //checks to see if the current state of the tmpPuzzle is valid or not
                if (isValidPartialHexagram(tmpPuzzle, position, magicSum)) {
                    count += backtrack(puzzle, tmpPuzzle, used, magicSum, position + 1); //RECURSIVE CALL
                }

                used[i] = 0;
            }
        }

        return count;
    }

    //checks if all of the sums equal each other at the given position
    private static boolean isValidPartialHexagram(int[] puzzle, int position, int magicSum){

        int firstSum = puzzle[1] + puzzle[5] + puzzle[8] + puzzle[11];
        int secondSum = puzzle[4] + puzzle[6] + puzzle[9] + puzzle[11];
        int thirdSum = puzzle[7] + puzzle[8] + puzzle[9] + puzzle[10];
        int fourthSum = puzzle[1] + puzzle[2] + puzzle[3] + puzzle[4];
        int fifthSum = puzzle[0] + puzzle[3] + puzzle[6] + puzzle[10];
        int sixthSum = puzzle[0] + puzzle[2] + puzzle[5] + puzzle[7];
        switch (position){
            case 0, 3, 2, 1:
                return true;
            case 4, 6, 5:
                return fourthSum == magicSum;
            case 7, 9, 8:
                return sixthSum == magicSum;
            case 10:
                return (thirdSum == magicSum) && (fifthSum == magicSum);
            case 11:
                return (firstSum == magicSum) && (secondSum == magicSum);
        }

        return false;
    }

    //checks if all of the six sums equal each other
    private static boolean isValidHexagram(int[] puzzle){
        int firstSum = puzzle[1] + puzzle[5] + puzzle[8] + puzzle[11];
        int secondSum = puzzle[4] + puzzle[6] + puzzle[9] + puzzle[11];
        int thirdSum = puzzle[7] + puzzle[8] + puzzle[9] + puzzle[10];
        int fourthSum = puzzle[1] + puzzle[2] + puzzle[3] + puzzle[4];
        int fifthSum = puzzle[0] + puzzle[3] + puzzle[6] + puzzle[10];
        int sixthSum = puzzle[0] + puzzle[2] + puzzle[5] + puzzle[7];
        return firstSum == secondSum && secondSum == thirdSum && thirdSum == fourthSum && fourthSum == fifthSum && fifthSum == sixthSum;
    }

    private static boolean zero(int[] puzzle) {
        for (int i = 0; i < puzzle.length; i++) {
            if (puzzle[i] != 0) {
                return false;  // If any element is not zero, return false.
            }
        }
        return true;  // All elements are zero.
    }

}