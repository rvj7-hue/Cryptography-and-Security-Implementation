//Rising edge implies change happens when clock changes from 0 to 1.
//Synchronous reset means the reset happens at the next active clock
//High level means that reset happens at 1

// 1. Rising Edge Simple Flip-flop
module dff_rising(d, clk, q);
    input d, clk;
    output reg q;
    `define DELAY #2
    always @(posedge clk) begin
        q <= d;
    end
endmodule


// 2. Rising Edge Synchronous Reset
module dff_sync(d, clk, reset, q);
    input d, clk, reset;
    output reg q;
    `define DELAY #2
    always @(posedge clk) begin
        if (reset)
            q <= 0;
        else
            q <= `DELAY d;
    end
endmodule


// 3. Rising Edge Asynchronous Reset High
module dff_async_high(d, clk, reset, q);
    input d, clk, reset;
    output reg q;
    `define DELAY #2
    always @(posedge clk or posedge reset) begin
        if (reset)
            q <= 0;
        else
            q <= `DELAY d;
    end
endmodule


// 4. Rising Edge Asynchronous Reset Low
module dff_async_low(d, clk, reset, q);
    input d, clk, reset;
    output reg q;
    `define DELAY #2
    always @(posedge clk or negedge reset) begin
        if (!reset)
            q <= 0;
        else
            q <= `DELAY d;
    end
endmodule

// 5. Falling Edge Simple Flip-flop
module dff_falling(d, clk, q);
    input d, clk;
    output reg q;
    `define DELAY #2
    always @(negedge clk) begin
        q <= `DELAY d;
    end
endmodule


// 6. Falling Edge Synchronous Reset
module dff_sync_f(d, clk, reset, q);
    input d, clk, reset;
    output reg q;
    `define DELAY #2
    always @(negedge clk) begin
        if (reset)
            q <= 0;
        else
            q <= `DELAY d;
    end
endmodule


// 7. Falling Edge Asynchronous Reset High
module dff_async_high_f(d, clk, reset, q);
    input d, clk, reset;
    output reg q;
    `define DELAY #2
    always @(negedge clk or posedge reset) begin
        if (reset)
            q <= 0;
        else
            q <= `DELAY d;
    end
endmodule


// 8. Falling Edge Asynchronous Reset Low
module dff_async_low_f(d, clk, reset, q);
    input d, clk, reset;
    output reg q;
    `define DELAY #2
    always @(negedge clk or negedge reset) begin
        if (!reset)
            q <= 0;
        else
            q <= `DELAY d;
    end
endmodule

`timescale 1ns/1ps

module main();

    reg d, clk;
    wire q;

    // Instantiate the module
    dff_rising test(d, clk, q);

    // Generate clock
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // Apply inputs
    initial begin
        $monitor("Time=%0t clk=%b d=%b q=%b", $time, clk, d, q);

        d = 0;

        #3 d = 1;
        #10 d = 0;
        #10 d = 1;
        #10 d = 0;

        #10 $finish;
    end

  //Output

//   Time=0 clk=0 d=0 q=x
// Time=3000 clk=0 d=1 q=x
// Time=5000 clk=1 d=1 q=1
// Time=10000 clk=0 d=1 q=1
// Time=13000 clk=0 d=0 q=1
// Time=15000 clk=1 d=0 q=0
// Time=20000 clk=0 d=0 q=0
// Time=23000 clk=0 d=1 q=0
// Time=25000 clk=1 d=1 q=1
// Time=30000 clk=0 d=1 q=1
// Time=33000 clk=0 d=0 q=1
// Time=35000 clk=1 d=0 q=0
// Time=40000 clk=0 d=0 q=0
// main.v:135: $finish called at 43000 (1ps)
