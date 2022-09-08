package asm;

import literal.Address;
import literal.Label;
import opcode.Mnemonic;
import opcode.Opcode;
import register.Register;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;;
import java.util.Arrays;
import java.util.List;

public class Main {

    @FunctionalInterface
    interface OutputWriter {
        void write(byte[] code, int org, OutputStream out);
    }

    public enum OutputAs {
        BIN("bin", (code, org, out) -> {}),
        HEX("hex", (code, org, out) -> {}),
        ROM("rom", (code, org, out) -> {}),//Also builds z80 too
        Z80("z80", (code, org, out) -> {}),//doesn't build rom
        C("c", (code, org, out) -> {}),
        PRETTY("txt", (code, org, out) -> {});//like a really well printed assembly listing

        String as;
        OutputWriter writer;

        OutputAs(String as, OutputWriter writer) {
            this.as = as;
            this.writer = writer;
        }
    }

    public static void main(String[] argv) throws Exception {
        try {
            OutputWriter format = null;
            for (OutputAs as: OutputAs.values()) {
                if(as.as == argv[1]) {
                    format = as.writer;
                    break;
                }
            }
            if(format == null) {
                error(Errors.BAD_OUT_FORMAT);
                System.exit(-1);//error
            }
            List<String> in = Files.readAllLines(Paths.get(argv[2]),
                    StandardCharsets.UTF_8);
            lines = new String[in.size()];
            for (int i = 0; i < lines.length; i++) {
                //pass 1
                //build syntax tree and symbol table
                lines[i] = in.get(i);//head removal better
                parseLine(lines[i], false);
            }
            in = null;
            //pass 2
            lineNumber = 0;
            for (int i = 0; i < lines.length; i++) {
                //pass 2
                //TODO code gen with actual address values instead of some zeros
                parseLine(lines[i], true);// should have defined them all
            }
            //TODO report
        } catch(Exception e) {
            error(Errors.INTERNAL);//also does an ...
            throw e;
        }
    }

    public static void labelProxy(String name) {
        Label l = new Label(name);
        Label.setLocation(name, new Address(org));
    }

    static int org, initialOrg;//also page
    final static int[] pageOrder = {
        16, 5, 2, 0, 4, 6, 1, 3, 7
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
            return 8;// an error
        }
        return x;
    }

    public static boolean isROMorSafe(int org) {
        return safes[getPage(org)];
    }

    static int lineNumber;//line number
    static String[] lines;
    static boolean printedScreenNotice5, printedScreenNotice7;

    //use magic "" quote in quotes
    public static String[] splitComma(String args) {
        String[] quotes = args.split("\"");
        for(int i = 1; i < quotes.length; i += 2) {
            quotes[i] = quotes[i].replace(",", "\u0100");
        }
        args = Arrays.stream(quotes).reduce((a, b) -> a + "\"" + b).get();
        quotes = args.split(",");
        for (int i = 0; i < quotes.length; i++) {
            quotes[i] = quotes[i].replace("\u0100", ",");
            //ok
        }
        return quotes;
    }

    public static String removeComment(String line) {
        String[] quote = line.split("\"");
        if(quote.length % 2 == 0) error(Errors.QUOTE_CLOSE);
        line = "";
        for(int i = 0; i < quote.length; i+=2) {
            quote[i] = quote[i].replace("//", ";");
            String[] split = quote[i].split(";");
            if(split.length > 1) {
                line += split[0];//strip comments
                break;// as comment after
            } else {
                line += quote[i];//all to quote
                if(i + 1 < quote.length) {
                    line += "\"" + quote[i + 1] + "\"";//and a literal
                }
            }
        }
        return line;
    }

    public enum ErrorKinds {
        INFO("\\u001b[32mInformation\\u001b[0m"),
        WARN("\\u001b[33mWarning\\u001b[0m"),
        ERROR("\\u001b[31mError\\u001b[0m");

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
        ORG_SET(ErrorKinds.ERROR, "Can only set org once at start of file"),
        INTERNAL(ErrorKinds.ERROR, "Internal error"),
        BAD_OUT_FORMAT(ErrorKinds.ERROR, "The output format asked for is not supported"),
        QUOTE_CLOSE(ErrorKinds.ERROR, "All \"\" must be paired and joined are one"),
        DUPE_LABEL(ErrorKinds.ERROR, "All label names must be unique or contextually determined"),
        LABEL_NOT_FOUND(ErrorKinds.ERROR, "An undefined label was not found"),
        NULL_REGISTER(ErrorKinds.ERROR, "No argument present");

        ErrorKinds kind;
        String msg;
        Errors(ErrorKinds kind, String msg) {
            this.kind = kind;
        }
    }

    public static void error(Errors error) {
        System.err.println(error.kind.kind
            + ": " + error.msg + ".");
        if(lineNumber != 0)
            System.err.println(lineNumber + "> " + lines[lineNumber - 1]);
    }

    public static byte[] parseLine(String line, boolean allowDupe) {
        lineNumber++;// starts at line 1
        line = removeComment(line);
        if(line == null || line.equals("")) return new byte[0];
        line = line.replace(":", ": ");//split for label
        Opcode op = new Opcode();
        String lastLine = line;
        //space prefixes of :
        while((line = line.replace(" :", ":")) != lastLine);
        while(line.length() > 0) {
            int spc = line.indexOf(" ");
            if(spc == -1) spc = line.length();// might not be any ...
            String word = line.substring(0, spc);
            line = line.substring(spc).trim();
            if(word.charAt(word.length() - 1) == ':') {
                word = word.substring(0, word.length() - 1).trim();
                //label:
                if(!allowDupe && Label.exits(word)) {
                    error(Errors.DUPE_LABEL);
                }
                labelProxy(word);
                continue;//multi-label lines
            }
            //handle mnemonic
            if(op.mnemonic() == null) {
                op.setMnemonic(Mnemonic.getMnemonic(word, org, allowDupe));
                if(op.mnemonic() == null) {
                    error(Errors.MNEMONIC_NOT_FOUND);
                    break;
                }
                if(op.mnemonic() == Mnemonic.LABEL) {
                    line = word + line;//line should be nothing but ...
                }
            }
            //handle registers/literals
            String[] comma = splitComma(line);
            for (String s: comma) {
                op.setRegisters(Register.getRegister(s.trim(), org, allowDupe));
            }
            byte[] compiled;
            if((compiled = op.compile(org)) == null) {
                break;//error
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
            if(!printedScreenNotice5) {
                printedScreenNotice5 = true;
                if (getPage(org) == 5) {
                    error(Errors.SCREEN_0);//could be an issue
                }
            }
            if(!printedScreenNotice7) {
                printedScreenNotice7 = true;
                if (getPage(org) == 7) {
                    error(Errors.SCREEN_1);//could be an issue
                }
            }
            return compiled;
        }
        error(Errors.LINE_DOES_NOT_ASSEMBLE);
        return null;//no can do
    }
}
