package register;

public enum Register16 {

    // opcode ordering
    BC("bc"),
    DE("de"),
    HL("hl", "ix", "iy"),// jp (hl) write as jp hl for consistency
    SP("sp"),
    AF("af"),
    IND_SP("(sp)"),
    IND_BC("(bc)"),
    IND_DE("(de)"),
    BAD_REG(null);//bad

    String name;

    String ix, iy;
    Register16(String name) {
        this.name = name;
        this.ix = null;
        this.iy = null;
    }

    Register16(String name, String ix, String iy) {
        this(name);
        this.ix = ix;
        this.iy = iy;
    }

    public static Register getRegister(String reg) {
        for (Register16 r: Register16.values()) {
            if(reg.trim().toLowerCase().equals(r.name))
                return new Register(r);
            if(r.ix != null) {
                if(reg.trim().toLowerCase().equals(r.ix))
                    return new Register(r, true, false);
            }
            if(r.iy != null) {
                if(reg.trim().toLowerCase().equals(r.iy))
                    return new Register(r, false, true);
            }
        }
        return null;
    }
}
