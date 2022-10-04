package literal;

public class Address {

    public int address;
    public boolean indirect = false;

    public Address(int address) {
        this.address = address;
    }

    public Address(String reg, int org, boolean allowIndirect) {
        if(allowIndirect) {
            if(reg.charAt(0) == '(' && reg.charAt(reg.length() - 1) == ')') {//indirect
                indirect = true;
                reg = reg.substring(1, reg.length() - 1);
            }
        }
        String based = reg.substring(1);
        try {
            if(reg.charAt(0) == '$') {
                if(reg.length() == 1) {
                    address = org;//special address of 1st byte of compile
                } else {
                    address = (char) Integer.parseInt(based, 16);
                }
            } else if(reg.charAt(0) == '%') {
                address = (char) Integer.parseInt(based, 2);
            } else {
                address = (char) Integer.parseInt(reg);
            }
        } catch(Exception e) {
            // leave as null
        }
    }
}
