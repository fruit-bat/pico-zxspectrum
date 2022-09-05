package opcode;

import literal.Address;
import register.Register;

import java.util.ArrayList;

public class Opcode {

    Mnemonic op;
    ArrayList<Register> args = new ArrayList<>();
    public Mnemonic mnemonic() {
        return op;
    }

    public void setMnemonic(Mnemonic op) {
        this.op = op;
    }

    public void setRegisters(Register reg) {
        args.add(reg);
    }

    public byte[] compile(int org) {//org for pages
        for (int i = 0; i < op.allows.length; i++) {
            byte[] b = op.allows[i].ass.assemble(this, org);
            if(b != null) return b;
        }
        return null;
    }
}
