module graytobin1(
    input [3:0] g,
    output [3:0] b
);

    assign b[3] = g[3];
    xor(b[2], b[3], g[2]);
    xor(b[1], b[2], g[1]);
    xor(b[0], b[1], g[0]);

endmodule


module main();

    reg [3:0] gray;
    wire [3:0] bin;

    integer i;

    // Instantiate the Gray-to-Binary converter
    graytobin1 test(gray, bin);

    // Testbench
    initial begin

        $display("Gray    Binary");

        for (i = 0; i < 16; i = i + 1) begin

            gray = i;

            #2;

            $display("%b    %b", gray, bin);

        end

        $finish;

    end

endmodule

//Outputs -
// Gray    Binary
// 0000    0000
// 0001    0001
// 0010    0011
// 0011    0010
// 0100    0111
// 0101    0110
// 0110    0100
// 0111    0101
// 1000    1111
// 1001    1110
// 1010    1100
// 1011    1101
// 1100    1000
// 1101    1001
// 1110    1011
// 1111    1010
