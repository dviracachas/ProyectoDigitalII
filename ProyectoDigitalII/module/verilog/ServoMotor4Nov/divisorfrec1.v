

module	divisorfrec1	(input  clk, output	reg	CLKOUT1); // 50 MHz to 180 kHz

parameter top=9'b010001010;// top=50MHz/180kHz=277,77
  reg [8:0] count_138;
	//
	initial
	begin
		CLKOUT1=1'b0;
		count_138=0;
	end
	//
	always @(posedge clk)
	begin
		count_138 <= count_138 + 1;
      if(count_138 == top)
		begin
			count_138<=0;
			CLKOUT1 <= ~CLKOUT1;
		end
	end
 
 endmodule