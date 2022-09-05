package literal;

import asm.Main;

import java.util.HashMap;

public class Label {

    String name;
    Address location;
    Label parent;//useful TODO

    static HashMap<String, Label> table;

    public Label(String name) {
        this.name = name;
        table.putIfAbsent(name, this);//prevent second pass overwrite address
    }

    public void setLocation(Address location) {
        this.location = location;
    }

    public static Address findLabel(String name, int org) {
        boolean page = false;
        if(name.length() > 0 && name.charAt(0) == '@') {//page of label
            page = true;
            name = name.substring(1);
        }
        Label l = table.get(name);
        if(l == null) return new Address(0);//first pass blag
        if(!Main.isROMorSafe(l.location.address)) {
            if(Main.getPage(l.location.address) != Main.getPage(org)) {
                Main.error(Main.Errors.LABEL_ADDRESS_PAGE);
            }
        }
        if(page && l.location != null) return new Address((l.location.address >> 16) | 8);// sets screen 7
        return l.location;
    }

    public static boolean existsLabel(String name) {
        return table.get(name) != null;
    }
}
