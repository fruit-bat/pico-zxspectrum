package literal;

import java.util.ArrayList;
import java.util.HashMap;

public class Label {

    String name;
    Address location;

    static HashMap<String, Label> table;

    public Label(String name) {
        this.name = name;
        table.putIfAbsent(name, this);//prevent second pass overwrite address
    }

    public void setLocation(Address location) {
        this.location = location;
    }

    public static Address findLabel(String name) {
        Label l = table.get(name);
        if(l == null) return new Address((char)0);//first pass blag
        return l.location;
    }

    public static boolean existsLabel(String name) {
        return table.get(name) != null;
    }
}
