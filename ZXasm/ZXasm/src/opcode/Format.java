package opcode;

import asm.Main;
import register.Register;
import register.Register16;
import register.Register8;

@FunctionalInterface
interface Assembler {
    byte[] assemble(Opcode op, int org);
}
public enum Format {

    NUL((opcode, org) -> {
        //TODO
        return null;
    }),
    NUL_1((opcode, org) -> {
        if(opcode.args.size() != 0) return null;//invalid
        return opcode.mnemonic().baseOpcode;
    }),
    R_R((opcode, org) -> { return null; }),
    ALU_R((opcode, org) -> {
        if(opcode.args.size() != 1) return null;//invalid
        int baseOpcode;
        switch(opcode.mnemonic()) {
            case ADD: baseOpcode = 0x80; break;
            case ADC: baseOpcode = 0x88; break;
            case SUB: baseOpcode = 0x90; break;
            case SBC: baseOpcode = 0x98; break;
            case AND: baseOpcode = 0xa0; break;
            case XOR: baseOpcode = 0xa8; break;
            case OR: baseOpcode = 0xb0; break;
            case CP: baseOpcode = 0xb8; break;
            default: return null;
        }
        int source = getRegister8(opcode, 0).reg8.ordinal();
        if(source > 8) return null;
        baseOpcode |= source;
        byte[] ok = new byte[1];
        ok[0] = (byte)baseOpcode;
        if(getRegister8(opcode, 0).reg8 == Register8.IND_HL)
            ok = getRegister8(opcode, 0).withIXIY(ok);
        return ok;
    }),
    ID_R((opcode, org) -> {
        if(opcode.args.size() != 1) return null;//invalid
        int baseOpcode = opcode.mnemonic().baseOpcode[0];
        int source = getRegister8(opcode, 0).reg8.ordinal();
        if(source > 8) return null;
        baseOpcode |= source << 3;// by eights
        byte[] ok = new byte[1];
        ok[0] = (byte)baseOpcode;
        if(getRegister8(opcode, 0).reg8 == Register8.IND_HL)
            ok = getRegister8(opcode, 0).withIXIY(ok);
        return ok;
    }),
    ID_RR((opcode, org) -> {
        if(opcode.args.size() != 1) return null;//invalid
        int baseOpcode = opcode.mnemonic().baseDouble;
        int source = getRegister16(opcode, 0).reg16.ordinal();
        if(source > 4) return null;
        baseOpcode |= source << 4;// by eights
        byte[] ok = new byte[1];
        ok[0] = (byte)baseOpcode;
        return ok;
    }),
    PP_RR((opcode, org) -> {
        if(opcode.args.size() != 1) return null;//invalid
        int baseOpcode = opcode.mnemonic().baseOpcode[0];
        if(getRegister16(opcode, 0).reg16 == Register16.SP) return null;
        if(getRegister16(opcode, 0).reg16 == Register16.AF)
            getRegister16(opcode, 0).reg16 = Register16.SP;
        int source = getRegister16(opcode, 0).reg16.ordinal();
        if(source > 4) return null;
        baseOpcode |= source << 4;// by eights
        byte[] ok = new byte[1];
        ok[0] = (byte)baseOpcode;
        return ok;
    }),
    LD_R_R((opcode, org) -> {
        if(opcode.args.size() != 2) return null;//invalid
        if(getRegister8(opcode, 0).reg8 == Register8.IND_HL
            && getRegister8(opcode, 1).reg8 == Register8.IND_HL)
                return null;//halt
        int baseOpcode = 0x40;
        int source = getRegister8(opcode, 1).reg8.ordinal();
        if(source > 8) return null;
        int dest = getRegister8(opcode, 0).reg8.ordinal();
        if(dest > 8) return null;
        dest <<= 3;
        source |= dest;
        baseOpcode |= source;
        byte[] ok = new byte[1];
        ok[0] = (byte)baseOpcode;
        if(getRegister8(opcode, 0).reg8 == Register8.IND_HL)
            ok = getRegister8(opcode, 0).withIXIY(ok);
        if(getRegister8(opcode, 1).reg8 == Register8.IND_HL)
            ok = getRegister8(opcode, 1).withIXIY(ok);
        return ok;
    }),
    RR_RR((opcode, org) -> { return null; }),
    R_N((opcode, org) -> { return null; }),
    R((opcode, org) -> { return null; }),
    RR((opcode, org) -> { return null; }),
    N((opcode, org) -> { return null; }),
    NN((opcode, org) -> { return null; }),
    D((opcode, org) -> { return null; }),
    IM((opcode, org) -> { return null; }),
    RST((opcode, org) -> { return null; }),
    F_D((opcode, org) -> { return null; }),//jr f, d
    F_NN((opcode, org) -> { return null; }),
    RET_F((opcode, org) -> {
        if(opcode.args.size() != 1) return null;//invalid
        int baseOpcode = 0xc0;
        int source = getFlags(opcode, 0).flags.ordinal();
        if(source > 8) return null;
        baseOpcode |= source << 3;// 8 per flag
        byte[] ok = new byte[1];
        ok[0] = (byte)baseOpcode;
        return ok;
    }),
    N_R((opcode, org) -> { return null; }),// bit and register
    INDIRECT_RR_RR((opcode, org) -> { return null; }),
    R_INDIRECT_NN((opcode, org) -> { return null; }),
    INDIRECT_NN_R((opcode, org) -> { return null; }),
    RR_INDIRECT_NN((opcode, org) -> { return null; }),
    INDIRECT_NN_RR((opcode, org) -> { return null; }),
    R_INDIRECT_R((opcode, org) -> { return null; }),
    R_INDIRECT_RR((opcode, org) -> { return null; }),
    INDIRECT_RR_R((opcode, org) -> { return null; }),
    INDIRECT_R((opcode, org) -> { return null; }),
    INDIRECT_R_R((opcode, org) -> { return null; }),
    MACRO((opcode, org) -> { return null; });

    Assembler ass;

    Format(Assembler ass) {
        this.ass = ass;
    }

    public static Register getRegister8(Opcode opcode, int number) {
        Register r = opcode.args.get(number);
        if(r.reg8 == null) {
            //Main.error(Main.Errors.BAD_REGISTER);
            r.reg8 = Register8.BAD_REG;
        }
        return r;
    }

    public static Register getRegister16(Opcode opcode, int number) {
        Register r = opcode.args.get(number);
        if(r.reg16 == null) {
            //Main.error(Main.Errors.BAD_REGISTER);
            r.reg16 = Register16.BAD_REG;
        }
        return r;
    }

    public static Register getFlags(Opcode opcode, int number) {
        Register r = opcode.args.get(number);
        if(r.flags == null) {
            //Main.error(Main.Errors.BAD_REGISTER);
            r.flags = CCode.BAD_FLAG;
        }
        return r;
    }
}
