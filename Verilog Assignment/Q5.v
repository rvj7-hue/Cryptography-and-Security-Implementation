module mux4to1(
    input [3:0] data,
    input [1:0] sel,
    output y
);

    assign y = data[sel];

endmodule


module main();

    reg [3:0] data;
    reg [1:0] sel;
    wire out1;

    integer i, j;

    // Instantiate the MUX
    mux4to1 m1(data, sel, out1);

    // Testbench
    initial begin

        $display("Data     Sel     Output");

        for (i = 0; i < 16; i = i + 1) begin

            for (j = 0; j < 4; j = j + 1) begin

                data = i;
                sel = j;

                #2;

                $display("%b     %b      %b", data, sel, out1);

            end

        end

        $finish;

    end

endmodule

//Outputs -
// 1011     01      1
// 1011     10      0
// 1011     11      1
// 1100     00      0
// 1100     01      0
// 1100     10      1
// 1100     11      1
// 1101     00      1
// 1101     01      0
// 1101     10      1
// 1101     11      1
// 1110     00      0
// 1110     01      1
// 1110     10      1
// 1110     11      1
// 1111     00      1
// 1111     01      1
// 1111     10      1
// 1111     11      1
