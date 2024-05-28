import java.util.Scanner;

public class dobra {
    public static void main(String[] args) {
        Scanner stdin = new Scanner(System.in);
        String input = stdin.next();

        System.out.println(countPleasantWords(input));

    }


    public static int countPleasantWords(String input) {
        char[] letters = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
        return backtrack(input.toCharArray(), letters, 0);
    }


    public static int backtrack(char[] word, char[] letters, int index) {
        if (index == word.length) {
            if (isPleasantWord(new String(word))) {
                return 1;
            }
            return 0;
        }
        if (word[index] == '_') {
            int count = 0;
            for (char letter : letters) {
                word[index] = letter;

                if(index > 1){
                    if(isPleasantWordPartial(word[index - 2], word[index - 1], word[index]))
                        count += backtrack(word, letters, index + 1);
                }else{
                    count += backtrack(word, letters, index + 1);
                }
            }
            word[index] = '_'; // Backtrack
            return count;
        } else {
            return backtrack(word, letters, index + 1);
        }
    }

    private static boolean isPleasantWord(String word){
        boolean containsL = false;
        for(int i = 0; i < word.length(); i++){
            if(word.charAt(i) == 'L')
                containsL = true;
            if(i > 1){
                if(isVowel(word.charAt(i-2)) == isVowel(word.charAt(i-1)) && isVowel(word.charAt(i-1)) == isVowel(word.charAt(i))){
                    return false;
                }
            }
        }

        return containsL;
    }

    private static boolean isPleasantWordPartial(char c1, char c2, char c3){
        if(isVowel(c1) == isVowel(c2) && isVowel(c2) == isVowel(c3)) {
            return false;
        }else{
            return true;
        }
    }

    private static boolean isVowel(char c){
        if(c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U')
            return true;
        else
            return false;
    }
}