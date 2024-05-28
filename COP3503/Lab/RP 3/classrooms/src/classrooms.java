import java.util.*;

public class classrooms {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        int n = scanner.nextInt();
        int k = scanner.nextInt();

        TreeSet<TimeFrame> classes = new TreeSet<>();
        for (int i = 0; i < n; i++) {
            int start = scanner.nextInt();
            int end = scanner.nextInt();
            classes.add(new TimeFrame(start, end));
        }

        TreeSet<Integer> endTimes = new TreeSet<>();
        int count = 0;
        int i = 0;
        for (TimeFrame tf : classes) {
            int start = tf.start;
            int end = tf.end;

            while (!endTimes.isEmpty() && endTimes.first() <= start) {
                endTimes.pollFirst();
                System.out.println(i + "while " + endTimes);
            }

            if (endTimes.size() < k) {
                endTimes.add(end);
                count++;
                System.out.println(i + " if: " + endTimes);
            }
            i++;
        }

        System.out.println(endTimes);

        System.out.println(count);
    }

    static class TimeFrame implements Comparable<TimeFrame> {
        int start, end;

        TimeFrame(int start, int end) {
            this.start = start;
            this.end = end;
        }

        @Override
        public int compareTo(TimeFrame other) {
            if (this.end != other.end) {
                return Integer.compare(this.end, other.end);
            } else {
                return Integer.compare(this.start, other.start);
            }
        }

        public String toString(){
            return start + " " + end;
        }
    }
}
