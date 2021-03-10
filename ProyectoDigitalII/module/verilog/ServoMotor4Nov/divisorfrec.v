
module	divisorfrec	(input  clk, output	reg	CLKOUT2); // 50 MHz to 100 kHz

 
  parameter top=8'b11111010;// top=50MHz/100kHz=500
  reg [7:0] count_250;
	//
	initial
	begin
		CLKOUT2=1'b0;
		count_250=0;
	end
	//
	always @(posedge clk)
	begin
		count_250 <= count_250 + 1;
      if(count_250 == top)
		begin
			count_250<=0;
			CLKOUT2 <= ~CLKOUT2;
		end
	end
	
	
	
endmodule