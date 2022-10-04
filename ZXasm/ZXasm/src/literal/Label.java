package literal;

import asm.Main;

import java.util.HashMap;

public class Label {

    String name;
    Address location;

    static HashMap<String, Label> table;
    static String lastGlobal = "";

    public Label(String name) {
        if(name.length() < 1) {
            name = lastGlobal + ".";// bad symbol null
        }
        if(name.charAt(0) == '.') {
            name = lastGlobal + name;//local
        } else {
            lastGlobal = name;//new global scope
        }
        this.name = name;
        table.putIfAbsent(name, this);//prevent second pass overwrite address
    }

    public static void setLocation(String name, Address location) {
        Label l = table.get(name);
        l.setLocation(location);
    }

    public void setLocation(Address location) {
        this.location = location;
    }

    public static boolean exits(String name) {
        return table.get(name) != null;
    }

    public static Address findLabel(String name, int org, boolean allowDupe) {
        boolean page = false;
        boolean upper = false;
        if(name.length() > 0 && name.charAt(0) == '@') {//page of label
            page = true;
            name = name.substring(1);
        }
        if(name.length() > 0 && name.charAt(0) == '^') {//page of label
            upper = true;
            name = name.substring(1);
        }
        if(name.length() > 0 && name.charAt(0) == '.') {
            name = lastGlobal + name;
        }
        Label l = table.get(name);
        if(l == null) {
            if(allowDupe) {//second pass
                Main.error(Main.Errors.LABEL_NOT_FOUND);
                return null;
            }
            return new Address(0);//first pass blag
        }
        if(!Main.isROMorSafe(l.location.address)) {
            if(Main.getPage(l.location.address) != Main.getPage(org)) {
                Main.error(Main.Errors.LABEL_ADDRESS_PAGE);
            }
        }
        if(page && l.location != null) return new Address((l.location.address >> 16));// sets screen 5
        if(upper && l.location != null) return new Address((l.location.address >> 8));
        return l.location;
    }
}
