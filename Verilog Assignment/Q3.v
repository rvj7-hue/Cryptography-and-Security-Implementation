module decoder2x4(a, b, y0, y1, y2, y3);

    input a, b;
    output y0, y1, y2, y3;

    wire na, nb;

    // NOT gates
    not (na, a);
    not (nb, b);

    // AND gates
    and (y0, na, nb);
    and (y1, na, b);
    and (y2, a, nb);
    and (y3, a, b);

endmodule

module main();

    reg a, b;
    wire y0, y1, y2, y3;

    decoder2x4 d1(a, b, y0, y1, y2, y3);

    initial begin
        $monitor("a=%b b=%b | y0=%b y1=%b y2=%b y3=%b",
                  a, b, y0, y1, y2, y3);

        a=0; b=0;
        #5 a=0; b=1;
        #5 a=1; b=0;
        #5 a=1; b=1;
    end

endmodule

//Outputs -
// a=0 b=0 | y0=1 y1=0 y2=0 y3=0
// a=0 b=1 | y0=0 y1=1 y2=0 y3=0
// a=1 b=0 | y0=0 y1=0 y2=1 y3=0
// a=1 b=1 | y0=0 y1=0 y2=0 y3=1
