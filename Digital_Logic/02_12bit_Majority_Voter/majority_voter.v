/* * Project: 12-bit Majority Voter
 * Description: Counts 0s and 1s in a 12-bit input and determines the majority.
 * Logic: 
 * - Y = 100 if count(0) > count(1)
 * - Y = 010 if count(0) == count(1)
 * - Y = 001 if count(0) < count(1)
 */

module majority_12bit(
    input [11:0] A,
    output reg [2:0] Y
);
    integer i;
    integer count1; // Number of 1s
    integer count0; // Number of 0s

    always @(*) begin
        count1 = 0;
        count0 = 0;
        
        // Count bits
        for(i=0; i<12; i=i+1) begin
            if(A[i] == 1'b1)
                count1 = count1 + 1;
            else
                count0 = count0 + 1;
        end

        // Compare and set output
        if (count0 > count1)
            Y = 3'b100;      // More 0s
        else if (count0 == count1)
            Y = 3'b010;      // Equal
        else
            Y = 3'b001;      // More 1s
    end
endmodule