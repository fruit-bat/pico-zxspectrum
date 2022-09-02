package asm;

import literal.Address;
import literal.Label;
import opcode.Mnemonic;
import opcode.Opcode;
import register.Register;

import java.util.Arrays;

public class Main {

    public static void main(String[] argv) {

    }

    public static void labelProxy(String name) {
        Label l = new Label(name);
        l.setLocation(new Address(org));
    }

    static int org, initialOrg;//also page
    final static int[] pageOrder = {
        8, 5, 2, 0, 4, 6, 1, 3, 7
    };

    final static boolean[] safes = {
        true, true, true, false, false, false, false, false, false
    };

    static boolean orgSet;

    public static void setOrg(int orgNew) {//set the origin start
        if(orgSet || org != initialOrg) {
            error(Errors.ORG_SET);
        } else {
            initialOrg = orgNew;//file start base
        }
        org = orgNew;
        orgSet = true;
    }

    public static int bank() {//move to next 16kB page
        int last = org;
        org &= 0xFC000;//restart at base of a bank
        last -= org;//amount used
        last = 0x3FFF - last;//to place
        org += 0x4000;//offset new bank
        if(!isROMorSafe(org)) {
            org |= 0xC000;// keep high bits as in banked RAM
        }
        return last;
    }

    public static int getPage(int org) {
        int x = pageOrder[(org & 0xFC000) >> 14];
        if(x > 8) {
            error(Errors.OUT_OF_MEMORY);
            return 9;
        }
        return x;
    }

    public static boolean isROMorSafe(int org) {
        return safes[getPage(org)];
    }

    static int lineNumber;//line number
    static String[] lines;
    static boolean printedScreenNotice;

    //use magic "" quote in quotes
    public static String[] splitComma(String args) {
        lineNumber++;
        String[] quotes = args.split("\"");
        for(int i = 1; i < quotes.length; i += 2) {
            quotes[i] = quotes[i].replace(",", "\n");
        }
        args = Arrays.stream(quotes).reduce((a, b) -> a + "\"" + b).get();
        quotes = args.split(",");
        for (int i = 0; i < quotes.length; i++) {
            quotes[i] = quotes[i].replace("\n", ",");
            //ok
        }
        return quotes;
    }

    public enum ErrorKinds {
        WARN("Warning"),
        ERROR("Error");

        String kind;
        ErrorKinds(String kind) {
            this.kind = kind;
        }
    }

    public enum Errors {
        BANK_CROSSING(ErrorKinds.ERROR,"Crossing a paged bank boundary"),
        OUT_OF_MEMORY(ErrorKinds.ERROR, "Out of memory"),
        LINE_DOES_NOT_ASSEMBLE(ErrorKinds.ERROR, "The line does not assemble"),
        MNEMONIC_NOT_FOUND(ErrorKinds.ERROR, "Mnemonic not found"),
        SCREEN_1(ErrorKinds.WARN, "Assembling into screen 1 page"),
        SCREEN_0(ErrorKinds.WARN, "Assembling into screen 0 page (OK?)"),
        LABEL_ADDRESS_PAGE(ErrorKinds.WARN, "Label is in a paged memory bank"),
        ORG_SET(ErrorKinds.ERROR, "Can only set org once at start of file");

        ErrorKinds kind;
        String msg;
        Errors(ErrorKinds kind, String msg) {
            this.kind = kind;
        }
    }

    public static void error(Errors error) {
        System.err.println(error.kind.kind
            + ": " + error.msg + ".");
        System.err.println(lineNumber + "> " + lines[lineNumber - 1]);
    }

    public static byte[] parseLine(String line) {
        if(line == null || line.equals("")) return null;
        line = line.split(";")[0];//strip comments
        line = line.split("//")[0].trim();//strip alt comments
        line = line.replace(":", ": ");//split for label
        Opcode op = new Opcode();
        String lastLine = line;
        //space prefixes of :
        while((line = line.replace(" :", ":")) != lastLine);
        while(line.length() > 0) {
            int spc = line.indexOf(" ");
            String word = line.substring(0, spc);
            line = line.substring(spc).trim();
            if(word.charAt(word.length() - 1) == ':') {
                //label:
                labelProxy(word.substring(0, word.length() - 1).trim());
                continue;//multi-label lines
            }
            //handle mnemonic
            if(op.mnemonic() == null) {
                op.setMnemonic(Mnemonic.getMnemonic(word));
                if(op.mnemonic() == null) {
                    error(Errors.MNEMONIC_NOT_FOUND);
                    break;
                }
            }
            //handle registers/literals
            String[] comma = splitComma(line);
            for (String s: comma) {
                op.setRegisters(Register.getRegister(s.trim(), org));
            }
            byte[] compiled;
            if((compiled = op.compile(org)) == null) {
                //alternates
                break;
            }
            //have opcode sequence??
            int lastOrg = org;
            org += compiled.length;
            if(!isROMorSafe(org)) {
                //correct org
                int skip = compiled.length >> 14;
                org += skip << 14;//pages skip
                org |= 0xC000;// keep high bits as in banked RAM
            }
            if(getPage(org - 1) != getPage(lastOrg)) {
                //crossed page on assembly
                if(!isROMorSafe(org - 1)) {
                    error(Errors.BANK_CROSSING);
                }
            }
            if(!printedScreenNotice) {
                printedScreenNotice = true;
                if (getPage(org) == 7) {
                    error(Errors.SCREEN_1);//could be an issue
                }
                if (getPage(org) == 5) {
                    error(Errors.SCREEN_0);//could be an issue
                }
            }
            return compiled;
        }
        error(Errors.LINE_DOES_NOT_ASSEMBLE);
        return null;//no can do
    }
}
