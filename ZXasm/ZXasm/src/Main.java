import literal.Address;
import literal.Label;
import opcode.CCode;
import opcode.Mnemonic;
import opcode.Opcode;
import register.Register;

public class Main {

    public static void main(String[] argv) {

    }

    public static void labelProxy(String name) {
        Label l = new Label(name);
        l.setLocation(new Address(org));
    }

    static char org;

    public static byte[] parseLine(String line) {
        if(line == null || line.equals("")) return null;
        line = line.split(";")[0];//strip comments
        line = line.split("//")[0].trim();//strip alt comments
        Opcode op = new Opcode();
        while(line.length() > 0) {
            int spc = line.indexOf(" ");
            String word = line.substring(0, spc);
            line = line.substring(spc).trim();
            if(word.charAt(word.length() - 1) == ':') {
                //label:
                labelProxy(word.substring(0, word.length() - 1).trim());
            }
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
            byte[] compiled;
            if((compiled = op.compile()) == null) {
                //alternates
            }
            //have opcode sequence??
            org += compiled.length;
            return compiled;
        }
        return null;//no can do
    }
}
