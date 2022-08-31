package literal;

public class Address {

    char address;

    public Address(char address) {
        this.address = address;
    }

    public Address(Address base, char relative) {
        this.address = (char)(base.address + relative);
    }

    public Address(Address base, Byte relative) {
        this.address = (char)(base.address + relative.value);
    }
}
