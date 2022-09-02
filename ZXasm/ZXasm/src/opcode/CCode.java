package opcode;

import register.Register;

public enum CCode {

    NZ("nz"),
    Z("z"),
    NC("nc"),
    C("c"),
    PO("po"),
    PE("pe"),
    P("p"),
    M("m");

    String name;

    CCode(String name) {
        this.name = name;
    }

    public static Register getCCode(String op) {
        for (CCode m: CCode.values()) {
            if(op.trim().toLowerCase().equals(m.name)) {
                return new Register(m);
            }
        }
        return null;
    }
}
