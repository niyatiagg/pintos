import java.lang.String;
import java.lang.StringBuilder;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class Main {
    private static List<Integer> encode_lzw(String str) {
        int size = 256;
        final Map<String, Integer> map = new HashMap();
        for(int i=0; i<size; i++) {
            map.put(String.valueOf((char)i), i);
        }
        final List<Integer> list = new LinkedList();
        int i=0;
        while(i<str.length()) {
            int j=i;
            while(j < str.length() && map.containsKey(str.substring(i, j+1))) {
                j++;
            }
            list.add(map.get(str.substring(i, j)));
            if(j < str.length()) {
                map.put(str.substring(i, j+1), size++);
            }
            i=j;
        }

        return list;
    }

    private static String decode_lzw(List<Integer> list) {
        int size = 256;
        final Map<Integer, String> map = new HashMap();
        for(int i=0; i<size; i++) {
            map.put(i, String.valueOf((char)i));
        }
        StringBuilder sb = new StringBuilder();

        Integer old = list.get(0);
        String s = map.get(old);
        sb.append(s);
        for(int i=1; i<list.size(); i++) {
            Integer nEw = list.get(i);
            if(!map.containsKey(nEw)) {
                char ch = s.charAt(0);
                s = s.concat(String.valueOf(ch));
            }
            else {
               s = map.get(nEw);
            }
            sb.append(s);
            char c = s.charAt(0);
            String st = map.get(old);
            map.put(size++, st.concat(String.valueOf(c)));
            old = nEw;
        }

        return sb.toString();
    }

    public static void main(String[] args) {
        String s = "MorelMorelMorelMorelMorel";
        List<Integer> list = encode_lzw(s);
        System.out.println(Arrays.toString(list.toArray()));
        String ans = decode_lzw(list);
        System.out.println(ans);
    }
}
