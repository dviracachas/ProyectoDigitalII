
module	divisorfrecgen	(input clk , output	reg	CLKOUT3	); // 50 MHz to 225KHz

  parameter top=9'b100010101;// top=50MHz/94.736kHz=527,78
  reg [8:0] count_278;
	//
	initial
	begin
		CLKOUT3=1'b0;
		count_278=0;
	end
	//
	always @(posedge clk)
	begin
		count_278 <= count_278 + 1;
      if(count_278 == top)
		begin
			count_278<=0;
			CLKOUT3 <= ~CLKOUT3;
		end
	end
endmodule