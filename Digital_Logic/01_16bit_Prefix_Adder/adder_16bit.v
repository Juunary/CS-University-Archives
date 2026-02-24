/* * Project: 16-bit Parallel Adder
 * Description: Implemented using full adders in a structural design.
 * Author: Junewoo Kang
 */

module half_adder(
    input a, b,
    output sum, carry
);
    xor(sum, a, b);
    and(carry, a, b);
endmodule

module full_adder(
    input a, b, cin,
    output sum, cout
);
    wire s1, c1, c2;
    half_adder HA1 (.a(a), .b(b), .sum(s1), .carry(c1));
    half_adder HA2 (.a(s1), .b(cin), .sum(sum), .carry(c2));
    or(cout, c1, c2);
endmodule

module adder_16bit(
    input [15:0] A,
    input [15:0] B,
    input Cin,
    output [15:0] Sum,
    output Cout,
    output Overflow // Optional: For portfolio completeness
);
    wire [16:0] c; // Carry wires
    assign c[0] = Cin;

    genvar i;
    generate
        for(i=0; i<16; i=i+1) begin : ADDER_LOOP
            full_adder FA (
                .a(A[i]), 
                .b(B[i]), 
                .cin(c[i]), 
                .sum(Sum[i]), 
                .cout(c[i+1])
            );
        end
    endgenerate

    assign Cout = c[16];
    assign Overflow = c[16] ^ c[15]; // Overflow detection for signed numbers
endmodule