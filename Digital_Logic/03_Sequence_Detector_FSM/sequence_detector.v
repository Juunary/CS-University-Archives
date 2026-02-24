/* * Project: Sequence Detector (FSM)
 * Description: Moore Machine to detect specific bit patterns (e.g., 101101).
 * Output Y becomes 1 at specific states (S3, S6).
 */

module sequence_detector(
    input X, CLK, RST,
    output reg Y,
    output reg Y1,
    output reg [2:0] current_state
);
    // State Encoding
    parameter S0 = 3'b000, S1 = 3'b001, S2 = 3'b010,
              S3 = 3'b011, S4 = 3'b100, S5 = 3'b101, S6 = 3'b110;

    reg [2:0] next_state;

    // Sequential Logic: State Transition
    always @(posedge CLK or posedge RST) begin
        if (RST)
            current_state <= S0;
        else
            current_state <= next_state;
    end

    // Combinational Logic: Next State Logic
    always @(*) begin
        case (current_state)
            S0: next_state = (X) ? S1 : S0;
            S1: next_state = (X) ? S1 : S2;
            S2: next_state = (X) ? S3 : S0;
            S3: next_state = (X) ? S4 : S2; // Detected '101' pattern start
            S4: next_state = (X) ? S5 : S0;
            S5: next_state = (X) ? S1 : S6;
            S6: next_state = (X) ? S3 : S0; // Detected '101101' pattern end
            default: next_state = S0;
        endcase
    end

    // Combinational Logic: Output Logic
    always @(*) begin
        // Default values
        Y = 1'b0;
        Y1 = 1'b0;

        case (current_state)
            S3: Y = 1'b1;        // Detected '101'
            S6: begin            // Detected '101101'
                Y = 1'b1; 
                Y1 = 1'b1; 
            end
        endcase
    end
endmodule