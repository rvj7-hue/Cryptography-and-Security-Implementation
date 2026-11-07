module fulladder1(a, b, c, sum, carry);

    input a, b, c;
    output sum, carry;

    wire sum, carry;

    assign sum = a ^ b ^ c;

    assign carry = (a & b) | (c & (a ^ b));

endmodule


module main();

reg a, b, c;

wire sum1, carry1;

fulladder1 add1(a, b,c, sum1, carry1);

always @(*)
  
begin

$monitor("a=%b b=%b c=%b sum=%b carry=%b", a, b,c, sum1, carry1);
  
end

initial

begin

a = 0; b = 0; c=0;

#5 a = 0; b = 0; c=1;

#5 a = 0; b = 1; c=0;

#5 a = 0; b = 1; c=1;

#5 a = 1; b = 0; c=0;

#5 a = 1; b = 0; c=1;

#5 a = 1; b = 1; c=0;

#5 a = 1; b = 1; c=1;

end

endmodule


//Output -
// a=0 b=0 c=0 sum=0 carry=0
// a=0 b=0 c=1 sum=1 carry=0
// a=0 b=1 c=0 sum=1 carry=0
// a=0 b=1 c=1 sum=0 carry=1
// a=1 b=0 c=0 sum=1 carry=0
// a=1 b=0 c=1 sum=0 carry=1
// a=1 b=1 c=0 sum=0 carry=1
// a=1 b=1 c=1 sum=1 carry=1
