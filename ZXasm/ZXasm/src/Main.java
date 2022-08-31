import opcode.CCode;
import opcode.Mnemonic;
import opcode.Opcode;
import register.Register;

public class Main {

    public static void main(String[] argv) {

    }

    public static Opcode parseLine(String line) {
        if(line == null || line.equals("")) return null;
        line = line.split(";")[0];//strip comments
        line = line.split("//")[0].trim();//strip alt comments
        Opcode op = new Opcode();
        while(line.length() > 0) {
            int spc = line.indexOf(" ");
            String word = line.substring(0, spc);
            line = line.substring(spc).trim();
            //handle mnemonic
            if(op.mnemonic() == null) {
                op.setMnemonic(Mnemonic.getMnemonic(word));
                continue;
            }
            //handle registers/literals
            String[] comma = word.split(",");
            switch (comma.length) {
                case 1: op.setRegisters(Register.getRegister(
                        comma[0].trim()), false
                    );
                    break;
                case 2: if(op.mnemonic().hasFlagFormat()) {
                        op.setRegisters(Register.getRegister(
                                comma[1].trim()),
                                CCode.getCCode(comma[0].trim()));
                        break;
                    }
                    op.setRegisters(Register.getRegister(
                            comma[0].trim()), true
                    );
                    op.setRegisters(Register.getRegister(
                            comma[1].trim()), false
                    );
                    break;

                default: break;//invalid count
            }
            if(!op.compile()) {
                //alternates
            }
            //have opcode sequence??
            return op;
        }
        return null;//no can do
    }
}
